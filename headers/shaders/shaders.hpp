#ifndef SHADER_HPP
# define SHADER_HPP


# ifndef DEBUG
#  define DEBUG 0
# endif

# include <string>
# include <fstream>		// used by shader.cpp
# include <sstream>		// used by shader.cpp
# include <iostream>	// used by shader.cpp
#include <utility>

# include "../model/model.hpp"	//	used by shader.cpp

# include <GL/glew.h>	// used by shader.cpp
# include <GL/gl.h>


class Shader {
	public:
		Shader();
		Shader(const char *vpath, const char *fpath, const Model &model);
		~Shader();

		unsigned int getId() const;

	class ShaderException : public std::exception {
		protected:
			std::string _reason;

		public:
			explicit ShaderException(std::string reason) : _reason(std::move(reason)) {};
			virtual ~ShaderException() throw() {};
			virtual const char *what() const throw() {
				return (_reason.c_str());
			};
		};

	class CompilationError final : public ShaderException {

		public:
			explicit CompilationError(const std::string &infos) : ShaderException("[ERROR] Shader compilation failed: " + infos) {};
			virtual ~CompilationError() throw() {};
			virtual const char *what() const throw() {
				return (_reason.c_str());
			};
	};

	private:
		GLuint	program_id;
		GLuint	vertex, fragment;
		GLuint	ka, kd, ks, d, tr, ns, illum, light_pos;

	static void loadMTLToFragment(Shader &self, const Model &model);
};


#endif