#include "../headers/scop.hpp"

glm::vec3 calculateCenter(const std::vector<float> &triangles) {
	glm::vec3 sum(0.0f);
	const size_t totalVertices = triangles.size() / 5;

	for (size_t i = 0; i < triangles.size(); i += 5) {
		sum.x += triangles[i];
		sum.y += triangles[i + 1];
		sum.z += triangles[i + 2];
	}
	return sum / static_cast<float>(totalVertices);
}
