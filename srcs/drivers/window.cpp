#include "../../headers/drivers/drivers.hpp"


void frame_buffer_size(GLFWwindow *window, const int w, const int h) {
	int x, y;
	if (window)
		x = 0;
	else
		x = 0;

	y = 0;
	glViewport(x, y, w, h);
}


int	init_window() {
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

	return 0;
}


GLFWwindow *create_window(const Model &model) {
	GLFWwindow *window =
		glfwCreateWindow(WINDOW_W, WINDOW_H, model.getName().c_str(), NULL, NULL);
	if (!window) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(window);

	#ifndef __APPLE__
		if (glewInit() != GLEW_OK) {
			glfwTerminate();
			std::cerr << "Failed to initialize GLEW" << std::endl;
			exit(-1);
		}
	#endif

	glfwSetFramebufferSizeCallback(window, frame_buffer_size);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	return window;
}