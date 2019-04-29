//
// Created by calper on 4/19/19.
//

#include "TextureRenderer.h"

auto __texture_renderer_vertex_shader = "#version 330 core\n"
                                        "layout (location = 0) in vec3 aPos;\n"
                                        "layout (location = 1) in vec3 aColor;\n"
                                        "layout (location = 2) in vec2 aTexCoord;\n"
                                        "\n"
                                        "out vec3 ourColor;\n"
                                        "out vec2 TexCoord;\n"
                                        "\n"
                                        "void main()\n"
                                        "{\n"
                                        "\tgl_Position = vec4(aPos, 1.0);\n"
                                        "\tourColor = aColor;\n"
                                        "\tTexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
                                        "}";

auto __texture_renderer_fragment_shader = "#version 330 core\n"
                                          "out vec4 FragColor;\n"
                                          "\n"
                                          "in vec3 ourColor;\n"
                                          "in vec2 TexCoord;\n"
                                          "\n"
                                          "// texture sampler\n"
                                          "uniform sampler2D texture1;\n"
                                          "\n"
                                          "void main()\n"
                                          "{\n"
                                          "    vec4 col = texture(texture1, TexCoord);\n"
                                          "\tFragColor = col;\n"
                                          "}";



TextureRenderer::TextureRenderer() {
    inited = false;
}


bool TextureRenderer::init() {

    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader_id, 1, &__texture_renderer_vertex_shader, nullptr);
    glCompileShader(vertex_shader_id);
    int success;
    glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertex_shader_id, 512, NULL, infoLog);
        std::cout << infoLog << std::endl;
        glDeleteShader(vertex_shader_id);
        return false;
    }

    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader_id, 1, &__texture_renderer_fragment_shader, nullptr);
    glCompileShader(fragment_shader_id);
    glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragment_shader_id, 512, nullptr, infoLog);
        std::cout << infoLog << std::endl;
        glDeleteShader(fragment_shader_id);
        return false;
    }

    program = glCreateProgram();
    glAttachShader(program, vertex_shader_id);
    glAttachShader(program, fragment_shader_id);
    glLinkProgram(program);

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cout << infoLog << std::endl;
        return false;
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    inited = true;


    return true;
}

float TextureRenderer::render(GLuint texture) {
    if (inited) {
        glUseProgram(program);

        float vertices[] = {
                // Positions          // Colors           // Texture Coords
                1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // Top Right
                1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Bottom Right
                -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Bottom Left
                -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // Top Left
        };

        unsigned int indices[] = {
                0, 1, 3, // first triangle
                1, 2, 3  // second triangle
        };

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(0);
        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // texture coord attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        auto texLoc = glGetUniformLocation(program, "texture1");
        glUniform1i(texLoc, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    return 0;
}
