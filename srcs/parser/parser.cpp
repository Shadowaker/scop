// created by Dan 13-05-2025

#include "parser/parser.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <stdexcept>

Parser::Parser() {
    throw Parser::FileNotFoundError();
}


Parser::Parser(const std::string& filepath) : filepath(filepath) {
    try {
        parse(filepath);
    }
    catch (Parser::ParserException &a){
        std::cerr << a.what() << std::endl;
    }
}


// Function to parse the .obj file.  Returns true on success, false on failure.
bool Parser::parse(const std::string& filepath) {

    std::ifstream   file(filepath);

    if (!file.is_open()) {
        throw FileNotFoundError();
    }

    std::string line;
    while (std::getline(file, line)) {
        parseLine(line);
    }
    file.close();
    return true;
}

const std::vector<Parser::VertexData>& Parser::getVertices() const {
    return (this->vertices);
}

const std::vector<std::array<float, 3>>& Parser::getNormals() const {
    return (this->normals);
}

const std::vector<std::array<float, 2>>& Parser::getTextures() const {
    return (this->textures);
}

const std::vector<std::array<unsigned int, 3>>& Parser::getFaces() const {
    return faces;
}

// Function to print the vertices on the std output
void Parser::printVertices() const {
    std::cout << "Vertices:" << std::endl;
    for (const auto& vertex : this->vertices) {
        std::cout << "Position: " << vertex.position[0] << ", " << vertex.position[1] << ", " << vertex.position[2] << std::endl;
        std::cout << "Normal: " << vertex.normal[0] << ", " << vertex.normal[1] << ", " << vertex.normal[2] << std::endl;
        std::cout << "TexCoord: " << vertex.texCoord[0] << ", " << vertex.texCoord[1] << std::endl;
        std::cout << "-------------------" << std::endl;
    }
}

// Helper function to parse a single line from the .obj file.
void Parser::parseLine(const std::string& line) {
    std::stringstream               ss(line);
    std::string                     prefix;
    
    ss >> prefix;

    if (prefix == "v") {
        std::array<float, 3> position;

        for (int i = 0; i < 3; ++i) {
            std::string valueStr;

            ss >> valueStr;
            position[i] = stf(valueStr);
        }

        VertexData vertex;
        vertex.position[0] = position[0];
        vertex.position[1] = position[1];
        vertex.position[2] = position[2];
        vertices.push_back(vertex);
    }
    else if (prefix == "vn") {
        std::array<float, 3> normal;

        for (int i = 0; i < 3; ++i) {
             std::string valueStr;
            ss >> valueStr;
            normal[i] = stf(valueStr);
        }
        normals.push_back(normal);
    } 
    else if (prefix == "vt") {
        std::array<float, 2> texCoord;

        for (int i = 0; i < 2; ++i) {
            std::string valueStr;
            ss >> valueStr;
            texCoord[i] = stf(valueStr);
        }
        textures.push_back(texCoord);
    } 
    else if (prefix == "f") {
        std::array<unsigned int, 3> face;
        std::string vertexDataStr;

        for (int i = 0; i < 3; ++i) {
            ss >> vertexDataStr;
            std::vector<std::string> parts = split(vertexDataStr, '/');

            unsigned int vertexIndex = stui(parts[0]) - 1; // .obj indices start at 1, so subtract 1.
            face[i] = vertexIndex;

            //check if normals and texture coordinates exists
            if (parts.size() > 1 && !parts[1].empty()) {
                unsigned int texCoordIndex = stui(parts[1]) - 1;
                vertices[vertexIndex].texCoord[0] = textures[texCoordIndex][0];
                vertices[vertexIndex].texCoord[1] = textures[texCoordIndex][1];
            }
            if (parts.size() > 2 && !parts[2].empty()) {
                 unsigned int normalIndex = stui(parts[2]) - 1;
                 vertices[vertexIndex].normal[0] = normals[normalIndex][0];
                 vertices[vertexIndex].normal[1] = normals[normalIndex][1];
                 vertices[vertexIndex].normal[2] = normals[normalIndex][2];
            }
        }
        faces.push_back(face);
    }
}

// Helper function to split a string by a delimiter.
std::vector<std::string>    Parser::split(const std::string& str, char delimiter) const {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Helper function to convert a string to a float.
float Parser::stf(const std::string& str) const {
    try {
        return std::stof(str);
    }
    catch (const std::invalid_argument& e) {
        throw Parser::ParseError("Error: Invalid float string in file.");
    }
}

// Helper function to convert a string to an unsigned int.
unsigned int Parser::stui(const std::string& str) const {
     try {
        return std::stoul(str);
    }
    catch (const std::invalid_argument& e) {
        throw Parser::ParseError("Error: Invalid unsigned int in file.");
    }
}
