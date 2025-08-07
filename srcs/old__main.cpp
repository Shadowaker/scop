#include "../headers/scop.hpp"

// Simple debug shaders (uncomment these to test)
const char* simpleVertexShader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

const char* simpleFragmentShader = R"(
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Solid red
}
)";

// Original complex shaders
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
)";

// OpenGL error checking function
void checkGLError(const std::string& operation) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error after " << operation << ": " << error << std::endl;
        switch(error) {
            case GL_INVALID_ENUM: std::cerr << "GL_INVALID_ENUM" << std::endl; break;
            case GL_INVALID_VALUE: std::cerr << "GL_INVALID_VALUE" << std::endl; break;
            case GL_INVALID_OPERATION: std::cerr << "GL_INVALID_OPERATION" << std::endl; break;
            case GL_OUT_OF_MEMORY: std::cerr << "GL_OUT_OF_MEMORY" << std::endl; break;
            default: std::cerr << "Unknown error" << std::endl; break;
        }
    }
}

// Shader compilation and linking functions
GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shader;
}

GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource) {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

float SCALE = 0.1f; // Adjust 0.5 to the desired scale

float modelMatrix[16] = {
    SCALE, 0.0f,  0.0f,  0.0f,
    0.0f,  SCALE, 0.0f,  0.0f,
    0.0f,  0.0f,  SCALE, 0.0f,
    0.0f,  0.0f,  0.0f,  1.0f
};

// Identity matrix for debugging
float identityMatrix[16] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};

// Original matrices (you can switch back to these later)
float projectionMatrix[16] = {
    1.81066f, 0.0f,     0.0f,      0.0f,
    0.0f,     2.41421f, 0.0f,      0.0f,
    0.0f,     0.0f,     -1.0202f, -1.0f,
    0.0f,     0.0f,     -0.20202f, 0.0f
};

float viewMatrixcp[16] = {
    0.7071f,  0.4082f, -0.5774f,  0.0f,
    0.0f,     0.8165f,  0.5774f,  0.0f,
    0.7071f, -0.4082f,  0.5774f, -4.2426f,
    0.0f,     0.0f,     0.0f,     1.0f
};

float viewMatrix2[16] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};


int main(int argc, char **argv) {
    std::string file_input;

    if (argc == 2)
        file_input = std::string(argv[1]);
    else {
        std::cerr << "Wrong number of arguments" << std::endl;
        return -1;
    }

    // 1. Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Set OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // 2. Create GLFW window
    GLFWwindow* window = glfwCreateWindow(1200, 1000, "OBJ Viewer", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    #ifndef __APPLE__
        // 3. Initialize GLEW
        if (glewInit() != GLEW_OK) {
            std::cerr << "Failed to initialize GLEW" << std::endl;
            return -1;
        }
    #endif

    // Set up viewport and OpenGL settings
    glViewport(0, 0, 1200, 1000);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE); // Disable face culling for debugging
    checkGLError("OpenGL setup");

    // DEBUG: Choose which shaders to use
    // For debugging, start with simple shaders:
    std::cout << "=== Creating Shaders ===" << std::endl;
    GLuint shaderProgram = createShaderProgram(simpleVertexShader, simpleFragmentShader);
    // Later, switch to complex shaders:
    // GLuint shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    
    if (shaderProgram == 0) {
        std::cerr << "Failed to create shader program!" << std::endl;
        glfwTerminate();
        return -1;
    }
    std::cout << "Shader program created successfully: " << shaderProgram << std::endl;
    
    glUseProgram(shaderProgram);
    checkGLError("shader program use");

    // Get uniform locations
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    
    // These uniforms are only needed for complex shaders
    GLint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
    GLint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
    GLint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
    GLint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");

    // DEBUG: Print uniform locations
    std::cout << "=== Uniform Locations ===" << std::endl;
    std::cout << "modelLoc: " << modelLoc << std::endl;
    std::cout << "viewLoc: " << viewLoc << std::endl;
    std::cout << "projLoc: " << projLoc << std::endl;
    std::cout << "objectColorLoc: " << objectColorLoc << std::endl;
    std::cout << "lightColorLoc: " << lightColorLoc << std::endl;
    std::cout << "lightPosLoc: " << lightPosLoc << std::endl;
    std::cout << "viewPosLoc: " << viewPosLoc << std::endl;

    // Set uniform values - start with identity matrices for debugging
    std::cout << "=== Setting Uniforms ===" << std::endl;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, viewMatrix2);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, identityMatrix);
    
    // These are only set if using complex shaders (check if locations are valid)
    if (objectColorLoc != -1) glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
    if (lightColorLoc != -1) glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
    if (lightPosLoc != -1) glUniform3f(lightPosLoc, 1.2f, 1.0f, 2.0f);
    if (viewPosLoc != -1) glUniform3f(viewPosLoc, 0.0f, 0.0f, 3.0f);
    
    checkGLError("setting uniforms");

    // Create Parser and ModelRenderer objects

    Parser parser(file_input);
    ModelRenderer renderer;

    try {
        std::cout << "OBJ file parsed successfully!" << std::endl;
        parser.inspect();

        renderer.setup(parser);
        std::cout << "ModelRenderer: OpenGL buffers set up using parsed data." << std::endl;

    } catch (const Parser::FileNotFoundError& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        glfwTerminate();
        return -1;
    } catch (const Parser::ParseError& e) {
        std::cerr << "Parse Error: " << e.what() << std::endl;
        glfwTerminate();
        return -1;
    } catch (const std::exception& e) {
        std::cerr << "An unexpected error occurred: " << e.what() << std::endl;
        glfwTerminate();
        return -1;
    }

    std::cout << "=== Starting Render Loop ===" << std::endl;
    
    // Enable wireframe mode for debugging (optional)
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // 6. Render loop
    int frameCount = 0;
    while (!glfwWindowShouldClose(window)) {
        // Clear screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        checkGLError("clear buffers");

        glUseProgram(shaderProgram);
        checkGLError("use program in loop");

        // Draw the object
        renderer.draw();
        checkGLError("draw call");

        // DEBUG: Print frame info for first few frames
        if (frameCount < 5) {
            std::cout << "Frame " << frameCount << " rendered" << std::endl;
            frameCount++;
        }

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 7. Clean up
    std::cout << "=== Cleaning Up ===" << std::endl;
    glDeleteProgram(shaderProgram);
    glfwTerminate();

    return 0;
}
