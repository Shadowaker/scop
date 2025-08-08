#ifndef DATRIX_HPP
# define DATRIX_HPP

# include <glm/glm.hpp>
# include <glm/gtc/type_ptr.hpp>
# include <cmath>	//used on datrix.cpp

class Datrix {
	public:
		float data[16];

		Datrix();
		Datrix(const float num);

		~Datrix();

		Datrix operator*(const Datrix& other) const;

		static Datrix translate(Datrix &mat, glm::vec3 offset);
		static Datrix translate(glm::mat4 &mat, glm::vec3 offset);
		static Datrix perspective(float fov, float aspect, float near, float far);
		static Datrix lookAt(glm::vec3 position, glm::vec3 front, glm::vec3 up);

		static glm::mat4 lookAtGl(glm::vec3 position, glm::vec3 front, glm::vec3 up);
		static glm::mat4 convertMatrix(const Datrix &matrix);
		glm::mat4 getMatrix();

};

#endif
