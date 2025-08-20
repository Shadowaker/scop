#include <cmath>

#include "../../headers/model/model.hpp"

// UTILS

void load_vertex(Model &model, std::istringstream &stream) {
	Vertex vertex{};

	stream >> vertex.x >> vertex.y >> vertex.z;
	vertex.texX = vertex.x;
	vertex.texY = vertex.y;
	Model::pushVertices(model, vertex);
}

void load_faces(Model &model, std::istringstream &stream) {
	int i = 0, j = 0;
	bool slash = false;

	std::vector<int> faces, uvs;

	while (stream >> i) {
		faces.push_back(i);
		if (stream.peek() == '/') {
			slash = true;
			stream.ignore();
			stream >> j;
			uvs.push_back(j);
			stream.ignore(256, ' ');
		} else
			stream.ignore(256, ' ');
	}
	if (faces.size() >= 3) {
		Model::pushFaces(model, faces);
		if (slash == true) Model::pushTexturesIndices(model, uvs);
		model.setSlash(slash);
	}
	else
		throw Model::CreationError("Too many indexes: " + std::to_string(faces.size()));
}

// ---------------------------------------------


Model::Model() {
	vao = 0;
	vbo = 0;
	vbo_normal = 0;

	light_source = glm::vec3(LIGHT_POS_X, LIGHT_POS_Y, LIGHT_POS_Z);

	if constexpr (DEBUG) {
		std::cout << "Model created" << std::endl;
	}
}

Model::Model(const std::string &file_path) {
	vao = 0;
	vbo = 0;
	vbo_normal = 0;

	light_source = glm::vec3(LIGHT_POS_X, LIGHT_POS_Y, LIGHT_POS_Z);

	loadModel(*this, file_path);
	normalizeCoords(*this);
	triangleCreator(*this);
	filter(*this);
	squaredTriangles(*this);

	if constexpr (DEBUG) {
		std::cout << "Model created" << std::endl;
	}
}


void Model::loadModel(Model &self, const std::string &file_path) {

	if constexpr (DEBUG) {
		std::cout << "Loading model from file..." << std::endl;
	}

	std::string line;
	std::ifstream ofile(file_path);

	if (!ofile.is_open()) {
		throw FileError(file_path);
	}

	while (std::getline(ofile, line)) {
		std::string prefix;
		std::istringstream stream(line);
		float normalX, normalY, normalZ;

		stream >> prefix;
		if (prefix == "o") {
			stream >> self.name;
		}
		else if (prefix == "mtllib") {
			loadMaterialDefinitions(self, stream, prefix, file_path);
		}
		else if (prefix == "v") {
			load_vertex(self, stream);
		}
		else if (prefix == "vn") {
			stream >> normalX >> normalY >> normalZ;
			self.normals.emplace_back(normalX, normalY, normalZ);
		}
		else if (prefix == "vt") {
			UV uvVal{};
			stream >> uvVal.u >> uvVal.v >> uvVal.w;
			self.textures.push_back(uvVal);
		}
		else if (prefix == "f") {
			self.c++;
			load_faces(self, stream);
		}
	}
	ofile.close();

	if constexpr (DEBUG) {
		std::cout << "Done." << std::endl;
	}
}

Model::~Model() {
	if constexpr (DEBUG) {
		std::cout << "Model destroyed" << std::endl;
	}
}

std::string Model::getName() const {
	return name;
}

Model &Model::pushVertices(Model &self, Vertex &vertex) {
	self.vertices.push_back(vertex);
	if constexpr (DEBUG) {
		//std::cout << "Vertices added" << std::endl;
		//std::cout << self.vertices.size() << " vertices" << std::endl;
	}

	return self;
}

Model &Model::pushFaces(Model &self, std::vector<int> &faces) {
	self.faces.push_back(faces);

	return self;
}

Model &Model::pushTexturesIndices(Model &self, std::vector<int> &indices) {
	self.textures_indices.push_back(indices);

	return self;
}

// DOC: https://people.sc.fsu.edu/~jburkardt/data/mtl/mtl.html
// MORE DOC: http://www.paulbourke.net/dataformats/mtl/
void Model::loadMaterialDefinitions(
	Model &self, std::istringstream &stream, std::string &prefix, std::string file_path
	)
{
	stream >> file_path;
	std::string file = file_path;
	std::ifstream mfile(file);

	if (mfile.is_open()) {
		std::string mline;

		while (std::getline(mfile, mline)) {
			std::istringstream sstream(mline);
			sstream >> prefix;
			if (prefix == "Ka") {
				sstream >> self.material.ka.r >> self.material.ka.g >> self.material.ka.b;
			}
			else if (prefix == "Kd")
				sstream >> self.material.kd.r >> self.material.kd.g >> self.material.kd.b;
			else if (prefix == "Ks")
				sstream >> self.material.ks.r >> self.material.ks.g >> self.material.ks.b;
			else if (prefix == "d")
				sstream >> self.material.d;
			else if (prefix == "Tr")
				sstream >> self.material.tr;
			else if (prefix == "Ns")
				sstream >> self.material.ns;
			else if (prefix== "Ni")
				sstream >> self.material.ni;
			else if (prefix == "illum")
				sstream >> self.material.illum;
		}
		mfile.close();
	} else
		std::cerr << "Error opening the file: " << file << std::endl;
}

void Model::normalizeCoords(Model &self) {
	if constexpr (DEBUG) {
		std::cout << "Normalizing vertices..." << std::endl;
	}
	for (auto &vertex : self.vertices) {
		const float theta = std::atan2(vertex.z, vertex.x);
		const float phi =
			acos(vertex.y / std::sqrt(vertex.x * vertex.x + vertex.y * vertex.y + vertex.z * vertex.z));
		vertex.texX = (theta + M_PI) / (2.0f * M_PI);
		vertex.texY = phi / M_PI;
	}
	if constexpr (DEBUG) {
		std::cout << "Done." << std::endl;
	}
}

void Model::triangleCreator(Model &self) {
	if constexpr (DEBUG) {
		std::cout << "Load triangles..." << std::endl;
	}
	if (self.slash == true) {
		for (long unsigned int i = 0; i < self.faces.size(); ++i) {
			const std::vector<int> &face = self.faces[i];
			const std::vector<int> &texture_index = self.textures_indices[i];

			if (face.size() >= 3 && texture_index.size() >= 3) {
				std::vector<Vertex> triangle;
				for (long unsigned int j = 0; j < face.size(); ++j) {
					int x = face[j], y = texture_index[j];
					if (y > 0 && y <= static_cast<int>(self.textures.size())) {
						self.vertices[x - 1].texX = self.textures[y - 1].u;
						self.vertices[x - 1].texY = self.textures[y - 1].v;
					}
					triangle.push_back(self.vertices[x - 1]);
				}
				self.trigon.push_back(triangle);
			} else
				std::cerr << "Invalid face with less than 3 indices encountered. Ignoring.\n";
		}
	}
	else
		for (const std::vector<int> &face : self.faces) {
			if (face.size() >= 3) {
				std::vector<Vertex> triangle;
				for (int i : face) {
					if (!self.textures.empty()) {
						self.vertices[i - 1].texX = self.textures[i - 1].u;
						self.vertices[i - 1].texY = self.textures[i - 1].v;
					}
					triangle.push_back(self.vertices[i - 1]);
				}
				self.trigon.push_back(triangle);
			}
			else
				std::cerr << "Invalid face with less than 3 indices encountered. Ignoring.\n";
		}
	if constexpr (DEBUG) {
		std::cout << "Triangles loaded." << std::endl;
	}
}


void Model::filter(Model &self) {
	for (const auto &shape : self.trigon) {
		if (shape.size() == 3) {
			for (const auto &vertex : shape) {
				self.Triangles.push_back(vertex.x);
				self.Triangles.push_back(vertex.y);
				self.Triangles.push_back(vertex.z);
				self.Triangles.push_back(vertex.texX);
				self.Triangles.push_back(vertex.texY);
			}
		}
		else if (shape.size() == 4) {
			for (const auto &vertex : shape) {
				self.Squares.push_back(vertex.x);
				self.Squares.push_back(vertex.y);
				self.Squares.push_back(vertex.z);
				self.Squares.push_back(vertex.texX);
				self.Squares.push_back(vertex.texY);
			}
		}
	}
}

void Model::squaredTriangles(Model &self) {
	std::vector<float> triangles;

	for (size_t i = 0; i < self.Squares.size(); i += 20) {
		if (self.Squares.empty()) break;
		float x1 = self.Squares[i];
		float y1 = self.Squares[i + 1];
		float z1 = self.Squares[i + 2];
		float texX1 = self.Squares[i + 3];
		float texY1 = self.Squares[i + 4];

		float x2 = self.Squares[i + 5];
		float y2 = self.Squares[i + 6];
		float z2 = self.Squares[i + 7];
		float texX2 = self.Squares[i + 8];
		float texY2 = self.Squares[i + 9];

		float x3 = self.Squares[i + 10];
		float y3 = self.Squares[i + 11];
		float z3 = self.Squares[i + 12];
		float texX3 = self.Squares[i + 13];
		float texY3 = self.Squares[i + 14];

		float x4 = self.Squares[i + 15];
		float y4 = self.Squares[i + 16];
		float z4 = self.Squares[i + 17];
		float texX4 = self.Squares[i + 18];
		float texY4 = self.Squares[i + 19];

		self.Triangles.insert(self.Triangles.end(), {x1, y1, z1, texX1, texY1, x2, y2, z2, texX2, texY2, x3,
										   y3, z3, texX3, texY3});
		self.Triangles.insert(self.Triangles.end(), {x1, y1, z1, texX1, texY1, x3, y3, z3, texX3, texY3, x4,
										   y4, z4, texX4, texY4});
	}
}


bool Model::getSlash() const {
	return slash;
}

void Model::setSlash(const bool new_slash) {
	 slash = new_slash;
}

void Model::loadExtenalTextures(Model &self, char **paths) {
	int i = 4;
	while (paths[i] != nullptr) self.external_textures.push_back(paths[i++]);
}

Mtl Model::getMtl() const {
	return material;
}

std::vector<float>	Model::getTriangles() const {
	return Triangles;
}

std::vector<glm::vec3> Model::getNormals() const {
	return normals;
}

std::vector<char *>	Model::getExternalTextures() const {
	return external_textures;
}