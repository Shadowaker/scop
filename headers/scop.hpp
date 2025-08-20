#ifndef SCOP_HPP
# define SCOP_HPP

# ifndef DEBUG
#  define DEBUG 0
# endif

# define WINDOW_W 1200
# define WINDOW_H 1000

// OpenGl
# include <GL/glew.h>
# include <GLFW/glfw3.h>
# include <GL/gl.h>
# include <glm/glm.hpp>
# include <glm/common.hpp>
# include <glm/gtc/type_ptr.hpp>


// Model
# include "model/model.hpp"

// Drivers
# include "drivers/drivers.hpp"

// Shaders
# include "shaders/shaders.hpp"

// Camera
# include "camera/camera.hpp"

// key.cpp
void		key(GLFWwindow *window, int &version, float &light, Model &model, Camera &camera);

// math_utils.cpp
glm::vec3	calculateCenter(const std::vector<float> &triangles);

#endif