#include "../../headers/datrix/datrix.hpp"

Datrix::Datrix() {
	data[0] = 1.0f; data[4] = 0.0f; data[8] = 0.0f; data[12] = 0.0f;
	data[1] = 0.0f; data[5] = 1.0f; data[9] = 0.0f; data[13] = 0.0f;
	data[2] = 0.0f; data[6] = 0.0f; data[10] = 1.0f; data[14] = 0.0f;
	data[3] = 0.0f; data[7] = 0.0f; data[11] = 0.0f; data[15] = 1.0f;
}

Datrix::Datrix(const float num) {
	data[0] = num; data[4] = 0.0f; data[8] = 0.0f; data[12] = 0.0f;
	data[1] = 0.0f; data[5] = num; data[9] = 0.0f; data[13] = 0.0f;
	data[2] = 0.0f; data[6] = 0.0f; data[10] = num; data[14] = 0.0f;
	data[3] = 0.0f; data[7] = 0.0f; data[11] = 0.0f; data[15] = num;
}

Datrix::~Datrix() {
}

Datrix Datrix::operator*(const Datrix &other) const {
	Datrix result;

	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			result.data[row * 4 + col] =
				data[row * 4 + 0] * other.data[0 * 4 + col] +
				data[row * 4 + 1] * other.data[1 * 4 + col] +
				data[row * 4 + 2] * other.data[2 * 4 + col] +
				data[row * 4 + 3] * other.data[3 * 4 + col];
		}
	}
	return result;
}


Datrix Datrix::translate(glm::mat4 &mat, glm::vec3 offset) {
	Datrix result;
	std::memcpy(result.data, glm::value_ptr(mat), sizeof(result.data));

	// Apply translation: last column gets offset added
	result.data[12] += offset.x;
	result.data[13] += offset.y;
	result.data[14] += offset.z;

	return result;
}

Datrix Datrix::translate(Datrix &mat, glm::vec3 offset) {

	// Apply translation: last column gets offset added
	mat.data[12] += offset.x;
	mat.data[13] += offset.y;
	mat.data[14] += offset.z;

	return mat;
}

Datrix Datrix::lookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up) {
	glm::vec3 f = glm::normalize(center - eye);
	glm::vec3 s = glm::normalize(glm::cross(f, up));
	glm::vec3 u = glm::cross(s, f);

	Datrix result;

	// Column 0
	result.data[0] = s.x;
	result.data[1] = u.x;
	result.data[2] = -f.x;
	result.data[3] = 0.0f;

	// Column 1
	result.data[4] = s.y;
	result.data[5] = u.y;
	result.data[6] = -f.y;
	result.data[7] = 0.0f;

	// Column 2
	result.data[8]  = s.z;
	result.data[9]  = u.z;
	result.data[10] = -f.z;
	result.data[11] = 0.0f;

	// Column 3 (translation)
	result.data[12] = -glm::dot(s, eye);
	result.data[13] = -glm::dot(u, eye);
	result.data[14] = glm::dot(f, eye);
	result.data[15] = 1.0f;

	return result;
}

Datrix Datrix::perspective(const float fov, const float aspect, const float near, const float far) {
	Datrix result;

	const float f = 1.0f / std::tan(fov / 2.0f);

	for (int i = 0; i < 16; ++i) result.data[i] = 0.0f;

	result.data[0] = f / aspect;
	result.data[5] = f;
	result.data[10] = (far + near) / (near - far);
	result.data[11] = -1.0f;
	result.data[14] = (2.0f * far * near) / (near - far);

	return result;
}

glm::mat4 Datrix::lookAtGl(glm::vec3 position, glm::vec3 front, glm::vec3 up) {
	Datrix result = lookAt(position, front, up);
	return result.getMatrix();
}

glm::mat4 Datrix::getMatrix() {
	return glm::make_mat4(data);
}

glm::mat4 Datrix::convertMatrix(const Datrix &matrix) {
	return glm::make_mat4(matrix.data);
}

