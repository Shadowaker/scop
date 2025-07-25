#ifndef ModelRenderer_HPP
# define ModelRenderer_HPP

# include "../parser/parser.hpp"
# include <vector>
# include <GL/glew.h>
# include <GL/gl.h>

class ModelRenderer {
private:
    // Vertex Array Object
    GLuint VAO;
    // Vertex Buffer Object 
    GLuint VBO;
    // Element Buffer Object
    GLuint EBO; 

    size_t numIndices;


public:
    ModelRenderer();
    ~ModelRenderer();

    ModelRenderer(const ModelRenderer&) = delete;
    ModelRenderer& operator=(const ModelRenderer&) = delete;

    void setup(const Parser& parser);

    void draw() const;

    void cleanup();
};

#endif // ModelRenderer_HPP