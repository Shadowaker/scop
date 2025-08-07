#include "../headers/scop.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "camera/camera.hpp"


void draw(Model &model) {
	const std::vector<float>	triangles = model.getTriangles();

	glBindVertexArray(model.vao);
	glDrawArrays(GL_TRIANGLES, 0, triangles.size()); // NOLINT(*-narrowing-conversions)
}


void createTexture(Model &model, int &stopper) {

	if (stopper != model.tex.type) {
		glGenTextures(1, &model.tex.id);
		glBindTexture(GL_TEXTURE_2D, model.tex.id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width, height, chs;
		const std::vector<char *> textures = model.getExternalTextures();

		unsigned char *data =
			stbi_load(textures[model.tex.type], &width, &height, &chs, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
						 data);
			glGenerateMipmap(GL_TEXTURE_2D);
		} else
			std::cout << "Failed to load texture" << std::endl;
		stbi_image_free(data);
		stopper = model.tex.type;
	}
}



void createVaoVbo(Model &model) {
	const std::vector<float>		triangles = model.getTriangles();
	const std::vector<glm::vec3>	normals = model.getNormals();

	glGenVertexArrays(1, &model.vao);
	glGenBuffers(1, &model.vbo);
	glBindVertexArray(model.vao);
	glBindBuffer(GL_ARRAY_BUFFER, model.vbo);
	glBufferData(GL_ARRAY_BUFFER, triangles.size() * sizeof(float), triangles.data(), // NOLINT(*-narrowing-conversions)
				 GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), static_cast<void *>(nullptr));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	if (!normals.empty()) {
		glGenBuffers(1, &model.vbo_normal);
		glBindBuffer(GL_ARRAY_BUFFER, model.vbo_normal);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), // NOLINT(*-narrowing-conversions)
					 normals.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, model.vbo_normal);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), static_cast<void *>(0));
	}
}

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


void rendererLoop(GLFWwindow *window, Shader &shader, Model &model, Camera &camera) {

	int light = 2;
	int v = 0;
	int stopper = -1;
	//glm::vec3 color(0.33f, 0.21f, 1.06f); //blue
	glm::vec3	color(1.33f, 1.0f, 1.06f); //blue
	float		axis = 0.0f;
	std::vector<float>	triangles = model.getTriangles();
	std::cout << model.getExternalTextures().size() << std::endl;

	while (!glfwWindowShouldClose(window)) {
		createTexture(model, stopper);

		model.matrix = glm::mat4(1.0f);
		std::vector<float> allTriangles;
		allTriangles.insert(allTriangles.end(), triangles.begin(), triangles.end());
		glm::vec3 objectCenter = calculateCenter(allTriangles);

		//std::cout << objectCenter.x << " " << objectCenter.y << " " << objectCenter.z << std::endl;

		model.matrix = glm::translate(model.matrix, objectCenter);
		model.matrix = glm::rotate(model.matrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model.matrix = glm::rotate(model.matrix, glm::radians(axis), glm::vec3(0.0f, 1.0f, 0.0f));
		model.matrix = glm::rotate(model.matrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model.matrix = glm::translate(model.matrix, -objectCenter);
		axis += 1.0f;

		glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, (
			model.mode == 0) ? GL_LINE : (model.mode == 1)  ? GL_POINT : GL_FILL
			);

		//glActiveTexture(GL_TEXTURE);

		// keyPressHandler(window, version, camera, object, shader, color);
		glUseProgram(shader.getId());
		glUniform1i(glGetUniformLocation(shader.getId(), "useTexture"), v);
		glUniform1i(glGetUniformLocation(shader.getId(), "useLight"), light);
		glUniform1i(glGetUniformLocation(shader.getId(), "texture"), 0);
		glUniform3fv(glGetUniformLocation(shader.getId(), "objectColor"),1 , &color[0]);

		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "model"), 1, GL_FALSE, &model.matrix[0][0]);

		glm::mat4 modelMatrix = glm::mat4(1.0f);

		glm::mat4 projectionMatrix =
			glm::perspective(glm::radians(45.0f),
							 static_cast<float>(WINDOW_W) / static_cast<float>(WINDOW_H),
							 0.1f, 1500.0f);

		glm::mat4 viewMatrix = camera.getView();
		//glm::mat4 viewMatrix = camera.createView(
		//	glm::vec3(0.0f, 0.0f, 3.0f),	// position
		//	glm::vec3(0.0f, 0.0f, -1.0f),	// front
		//	glm::vec3(0.0f, 0.5f, 0.0f)	// up
		//	);

		//glm::mat4 viewMatrix = glm::lookAt(
		//	glm::vec3(0.0f, 0.0f, -45.0f),
		//	glm::vec3(0.0f, 0.0f, 0.0f) + glm::vec3(0.0f, 0.0f, -1.0f),	// here should be position + front
		//	glm::vec3(0.0f, 0.5f, 0.0f));

		glm::mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;

		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "modelViewProjectionMatrix"), 1, GL_FALSE, &modelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "projection"), 1, GL_FALSE, &projectionMatrix[0][0]);

		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "view"), 1, GL_FALSE, &viewMatrix[0][0]);

		draw(model);

		key(window, v, model, camera);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}



int main(int argc, char **argv) {
	// validator here
	if (argc < 3) {
		std::cerr << "Usage: " << argv[0] << " <file.obj> <vector shaders> <fragment shaders> [textures]" << std::endl;
		// TODO Add default shaders
		return EXIT_FAILURE;
	}

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	const std::string file_path(argv[1]);

	Model model(file_path);
	Model::loadExtenalTextures(model, argv);

	GLFWwindow *window = create_window(model);

	Shader shader(argv[2], argv[3], model);
	Camera camera;

	createVaoVbo(model);

	rendererLoop(window, shader, model, camera);

	glDeleteVertexArrays(1, &model.vao);
	glDeleteBuffers(1, &model.vbo);
	glfwTerminate();
	return 0;
}