#include "../headers/scop.hpp"


std::vector<int> SUPPORTED_KEYS = {
	GLFW_KEY_LEFT_SHIFT, GLFW_KEY_T, GLFW_KEY_Y, GLFW_KEY_M,
	GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D,
	GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_LEFT, GLFW_KEY_RIGHT,
	GLFW_KEY_SPACE, GLFW_KEY_LEFT_CONTROL
};

std::vector<int> MOVEMENT_KEYS = {
	GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D,
	GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_LEFT, GLFW_KEY_RIGHT,
	GLFW_KEY_SPACE, GLFW_KEY_LEFT_CONTROL
};

void	movement_handler(Camera &camera, int key_pressed) {
	if (key_pressed == GLFW_KEY_W)
		camera.forward();
	else if (key_pressed == GLFW_KEY_S)
		camera.backward();
	else if (key_pressed == GLFW_KEY_A)
		camera.left();
	else if (key_pressed == GLFW_KEY_D)
		camera.right();
	else if (key_pressed == GLFW_KEY_SPACE)
		camera.goUp();
	else if (key_pressed == GLFW_KEY_LEFT_CONTROL)
		camera.goDown();
	else if (key_pressed == GLFW_KEY_UP)
		camera.pitchUp();
	else if (key_pressed == GLFW_KEY_DOWN)
		camera.pitchDown();
	else if (key_pressed == GLFW_KEY_RIGHT)
		camera.rotateRight();
	else if (key_pressed == GLFW_KEY_LEFT)
		camera.rotateLeft();
}


void	key(GLFWwindow *window, int &version, float &light, Model &model, Camera &camera) {
	static bool	t_key_locker = false;
	static bool	shift_key_locker = false;
	static bool	m_key_locker = false;
	static bool l_key_locker = false;


	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		return ;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && !shift_key_locker) {
		if (version == 4)
			version = 0;
		else
			version += 1;
		if constexpr (DEBUG) {
			std::cout << "Version: " << version << std::endl;
		}
		shift_key_locker = true;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		shift_key_locker = false;

	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !t_key_locker) {
		const std::vector<char *> textures = model.getExternalTextures();

		if (static_cast<int>(textures.size()) == model.tex.type + 1)
			model.tex.type = 0;
		else
			model.tex.type += 1;
		t_key_locker = true;
	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE)
		t_key_locker = false;

	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !m_key_locker) {
		if (model.mode == 2)
			model.mode = 0;
		else
			model.mode += 1;
		m_key_locker = true;
	}
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE)
		m_key_locker = false;

	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !l_key_locker) {
		if (light > 1.0)
			light = 0.1;
		else
			light += 0.1;
		if constexpr (DEBUG) {
			std::cout << "light: " << light << std::endl;
		}
		l_key_locker = true;
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE)
		l_key_locker = false;

	for (const auto& i : MOVEMENT_KEYS)
		if (glfwGetKey(window, i) == GLFW_PRESS) {
			movement_handler(camera, i);
		}
}