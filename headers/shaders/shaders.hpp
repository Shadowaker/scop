#ifndef SHADER_HPP
# define SHADER_HPP


# ifndef DEBUG
#  define DEBUG 0
# endif

# include <string>
# include <fstream>		// used by shader.cpp
# include <sstream>		// used by shader.cpp
# include <iostream>	// used by shader.cpp

# include "../model/model.hpp"	//	used by shader.cpp

# include <GL/glew.h>	// used by shader.cpp
# include <GL/gl.h>


class Shader {
	public:
		Shader();
		Shader(const char *vpath, const char *fpath, const Model &model);
		~Shader();

		unsigned int getId() const;

	private:
		GLuint	program_id;
		GLuint	vertex, fragment;
		GLuint	ka, kd, ks, d, tr, ns, illum;

	static void loadMTLToFragment(Shader &self, const Model &model);
};


#endif