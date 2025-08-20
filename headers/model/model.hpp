#ifndef MODEL_HPP
# define MODEL_HPP

# ifndef DEBUG
#  define DEBUG 0
# endif

//	openGL
# include <GL/glew.h>
# include <glm/glm.hpp>
# include <GLFW/glfw3.h>	// used by model.cpp

# include <iostream>		// used by model.cpp
# include <fstream>			// used by model.cpp
# include <sstream>			// used by model.cpp
# include <string>
# include <vector>
# include <array>			// used by model.cpp
# include <cmath>			// used by model.cpp

# define LIGHT_POS_X 0.0f
# define LIGHT_POS_Y 0.0f
# define LIGHT_POS_Z 0.0f
# define LIGHT_DIR_X 0.0f
# define LIGHT_DIR_Y 0.0f
# define LIGHT_DIR_Z 0.0f


struct UV {
	float u, v, w;
};

struct Vertex {
	float x, y, z, texX, texY, normalX, normalY, normalZ;
};

struct RGB {
	float r, g, b;
};

struct Mtl {

	RGB	ka, ks, kd;

	float ns, tr, d, ni;
	int illum;
};

struct Texture {
	int				type;
	unsigned int	id;

};


class Model {
	public:
		Model();
		Model(const std::string &file_path);
        ~Model();

		std::string				getName() const;
		bool					getSlash() const;
		Mtl						getMtl() const;
		std::vector<float>		getTriangles() const;
		std::vector<glm::vec3>	getNormals() const;
		std::vector<char *>		getExternalTextures() const;

		void					setSlash(bool new_slash);


		static Model			&pushVertices(Model &self, Vertex &vertex);
		static Model			&pushFaces(Model &self, std::vector<int> &faces);
		static Model			&pushTexturesIndices(Model &self, std::vector<int> &indices);

		static void				loadExtenalTextures(Model &self, char **paths);


		GLuint			vao, vbo, vbo_normal;
		int				mode = 0;
		glm::mat4		matrix{};
		glm::vec3		light_source{};
		Texture			tex{};

	class ModelException : public std::exception {
		protected:
			std::string _reason;

		public:
			ModelException(std::string reason) : _reason(reason) {};
			virtual ~ModelException() throw() {};
			virtual const char *what() const throw() {
				return (_reason.c_str());
			};
	};

	class CreationError final : public ModelException {

		public:
			explicit CreationError(const std::string &infos) : ModelException("[ERROR] Model creation failed: " + infos) {};
			virtual ~CreationError() throw() {};
			virtual const char *what() const throw() {
				return (_reason.c_str());
			};
		};

	class FileError final : public ModelException {
		public:
			explicit FileError(const std::string &fp) : ModelException("[ERROR] File " + fp + " could not be opened.") {};
			virtual ~FileError() throw() {};
			virtual const char *what() const throw() {
				return (_reason.c_str());
			}
		};


	private:
		int									c = 0;			// face counter
		bool								slash = false;
		std::string							file_path;
		std::string							name;

		std::vector<Vertex>					vertices;
		std::vector<UV>						textures;
		std::vector<std::vector<int>>		textures_indices;

		std::vector<char *>					external_textures;
		unsigned int						external_textures_index = 0;
		std::vector<glm::vec3>				normals;
		std::vector<std::vector<int>>		faces;
		std::vector<std::vector<Vertex> >	trigon;
		std::vector<float>					Triangles, Squares;

		Mtl									material{};

		static void		loadModel(Model &self, const std::string &file_path);
		static void		loadMaterialDefinitions(Model &self, std::istringstream &stream, std::string &prefix, std::string file_path);
		static void		normalizeCoords(Model &self);
		static void		triangleCreator(Model &self);
		static void		filter(Model &self);
		static void		squaredTriangles(Model &self);

};

#endif