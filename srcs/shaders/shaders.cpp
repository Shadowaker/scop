#include "../../headers/shaders/shaders.hpp"

// OpenGL Doc
unsigned int compileShader(unsigned int type, const char* source) {
	unsigned int shader_id  = glCreateShader(type);
	glShaderSource(shader_id, 1, &source, NULL);
	glCompileShader(shader_id);

	int success;
	char infoLog[512];
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader_id, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// TODO Create Error in the c++ way, and normalize string

	return shader_id;
}

Shader::Shader(): ka(0), kd(0), ks(0), d(0), tr(0), ns(0), illum(0){
	program_id = 0;
	vertex = 0;
	fragment = 0;

	if constexpr (DEBUG) {
		std::cout << "Shaders created" << std::endl;
	}
}


Shader::Shader(const char *vpath, const char *fpath, const Model &model) {
	std::string vert, frag;
	std::ifstream vfile, ffile;
	std::stringstream vstream, fstream;


	try {
		vfile.open(vpath);
		vstream << vfile.rdbuf();
		vfile.close();
		vert = vstream.str();
	} catch (std::ifstream::failure &e) {
		std::cout << "Vertex shaders error: " << e.what() << std::endl;
	}

	try {
		ffile.open(fpath);
		fstream << ffile.rdbuf();
		ffile.close();
		frag = fstream.str();
	} catch (std::ifstream::failure &e) {
		std::cout << "Fragment shaders error: " << e.what() << std::endl;
	}

	const char *v = vert.c_str(), *f = frag.c_str();

	vertex = compileShader(GL_VERTEX_SHADER, v);
	fragment = compileShader(GL_FRAGMENT_SHADER, f);

	program_id = glCreateProgram();

	glAttachShader(program_id, vertex);
	glAttachShader(program_id, fragment);
	glLinkProgram(program_id);

	int success;
	char infoLog[512];
	glGetProgramiv(program_id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program_id, 512, NULL, infoLog);
		std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
	loadMTLToFragment(*this, model);
}

Shader::~Shader() {
	glDeleteProgram(program_id);
	if constexpr (DEBUG) {
		std::cout << "Shader destroyed" << std::endl;
	}
}

void Shader::loadMTLToFragment(Shader &self, const Model &model) {
	const Mtl mtl = model.getMtl();

	self.ka = glGetUniformLocation(self.program_id, "Ka");
	self.kd = glGetUniformLocation(self.program_id, "Kd");
	self.ks = glGetUniformLocation(self.program_id, "Ks");
	self.d = glGetUniformLocation(self.program_id, "d");
	self.tr = glGetUniformLocation(self.program_id, "Tr");
	self.ns = glGetUniformLocation(self.program_id, "Ns");
	self.illum = glGetUniformLocation(self.program_id, "illum");
	const GLuint lightPosLoc = glGetUniformLocation(self.program_id, "LightPos");

	glUseProgram(self.program_id);
	glUniform3f(self.ka, mtl.ka.r, mtl.ka.g, mtl.ka.b);
	glUniform3f(self.kd, mtl.kd.r, mtl.kd.g, mtl.kd.b);
	glUniform3f(self.ks, mtl.ks.r, mtl.ks.g, mtl.ks.b);
	glUniform1f(self.tr, mtl.tr);
	glUniform1f(self.d, mtl.d);
	glUniform1f(self.ns, mtl.ns);
	glUniform1i(self.illum, mtl.illum);
	glUniform3f(lightPosLoc, model.light_source.x, model.light_source.y,model.light_source.z);
}

unsigned int Shader::getId() const {
	return program_id;
}