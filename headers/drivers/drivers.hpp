#ifndef DRIVERS_HPP
# define DRIVERS_HPP

# include "../scop.hpp"

int				init_window();
GLFWwindow		*create_window(const Model &model);
void			frame_buffer_size(GLFWwindow *window, const int w, const int h);


#endif