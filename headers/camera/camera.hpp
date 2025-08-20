#ifndef CAMERA_HPP
# define CAMERA_HPP

# ifndef DEBUG
#  define DEBUG 0
# endif

# include <iostream>

# include <glm/gtc/type_ptr.hpp>
# include <glm/glm.hpp>

#include "datrix/datrix.hpp"

enum moves {
	UP, DOWN, LEFT, RIGHT, ROT_RIGHT, ROT_LEFT
};

constexpr float SPEED = 0.1f;
constexpr float ROT_SPEED = 0.02f;
constexpr glm::vec3 DEFAULT_UP(0.0f, 0.5f, 0.0f);


class Camera {

	public:

		Camera();
		~Camera();

		glm::mat4	createView(glm::vec3 new_position, glm::vec3 new_front, glm::vec3 new_up);

		glm::vec3	getPosition() const;
		glm::vec3	getFront() const;
		glm::vec3	getUp() const;
		glm::mat4	getView() const;
		Datrix		getView2() const;

		void	setPosition(glm::vec3 new_position);
		void	setFront(glm::vec3 new_front);
		void	setUp(glm::vec3 new_up);
		void	setView(const glm::mat4 &new_view);

		void	forward();
		void	backward();
		void	left();
		void	right();
		void	goUp();
		void	goDown();
		void	pitchUp();
		void	pitchDown();
		void	rotateRight();
		void	rotateLeft();


	private:

		glm::vec3 position;
		glm::vec3 front;
		glm::vec3 up;
		glm::vec3 side;
		glm::mat4 view;

};
#endif