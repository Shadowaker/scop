

#include "../../headers/modelRenderer/modelRenderer.hpp"
#include <iostream>
#include <cstddef>


ModelRenderer::ModelRenderer() : VAO(0), VBO(0), EBO(0), numIndices(0) {}

ModelRenderer::~ModelRenderer() {
    cleanup();
}

void ModelRenderer::setup(const Parser& parser) {
    cleanup();

    const std::vector<t_VertexData>& vertices = parser.getVertices();
    const auto& rawFaces = parser.getRawFaces();
    std::vector<unsigned int> indices;

    // DEBUG: Print important information
    std::cout << "=== ModelRenderer Setup Debug ===" << std::endl;
    std::cout << "Vertices count: " << vertices.size() << std::endl;
    std::cout << "Faces count: " << rawFaces.size() << std::endl;

    if (vertices.empty() || rawFaces.empty()) {
        std::cerr << "Warning: No vertex or face data to set up OpenGL buffers in ModelRenderer." << std::endl;
        return;
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(t_VertexData), vertices.data(), GL_STATIC_DRAW);

    for (const auto& face : rawFaces) {
        if (face.size() < 3) continue;

        // Triangulate polygon using triangle fan
        for (size_t i = 1; i + 1 < face.size(); ++i) {
            indices.push_back(face[0]);
            indices.push_back(face[i]);
            indices.push_back(face[i + 1]);
        }
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    numIndices = indices.size();
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), faces.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(t_VertexData), (void*)offsetof(t_VertexData, position));
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(t_VertexData), (void*)offsetof(t_VertexData, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(t_VertexData), (void*)offsetof(t_VertexData, texCoord));
    glEnableVertexAttribArray(2);

    // Unbind VAO (it's good practice to unbind after configuring)
    glBindVertexArray(0);
    
    // Unbind VBO and EBO (they are associated with the VAO, so they can be unbound safely)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // TODO Log method
    std::cout << "ModelRenderer: OpenGL buffers set up successfully." << std::endl;
}

void ModelRenderer::draw() const {
    if (VAO == 0 || EBO == 0 || numIndices == 0) {
        std::cerr << "Error: ModelRenderer OpenGL buffers not set up or empty." << std::endl;
        return;
    }

    // DEBUG: Print what we're about to draw
    static bool debugPrinted = false;
    if (!debugPrinted) {
        std::cout << "=== Drawing Debug ===" << std::endl;
        std::cout << "VAO: " << VAO << ", EBO: " << EBO << ", numIndices: " << numIndices << std::endl;
        debugPrinted = true;
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);

    // Check for OpenGL errors
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error in draw(): " << error << std::endl;
    }
    
    glBindVertexArray(0);
}

void ModelRenderer::cleanup() {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (EBO != 0) {
        glDeleteBuffers(1, &EBO);
        EBO = 0;
    }
    numIndices = 0;
    std::cout << "ModelRenderer: OpenGL buffers cleaned up." << std::endl;
}