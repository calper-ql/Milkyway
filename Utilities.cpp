//
// Created by calper on 4/19/19.
//

#include "Utilities.h"

int initGL() {
    if (glfwInit() == 0) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        exit(1);
    }

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    return 0;
}

int initGL3W() {
    if (gl3wInit()) {
        std::cout << "Failed to initialize GL3W" << std::endl;
        exit(1);
    }
    return 0;
}

void rt_buffer_to_gl(optix::Buffer buffer, GLuint &gl_tex_id) {

    // Query buffer information
    RTsize buffer_width_rts, buffer_height_rts;
    buffer->getSize(buffer_width_rts, buffer_height_rts);
    uint32_t width = static_cast<int>(buffer_width_rts);
    uint32_t height = static_cast<int>(buffer_height_rts);
    RTformat buffer_format = buffer->getFormat();

    if (!gl_tex_id) {
        glGenTextures(1, &gl_tex_id);

    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gl_tex_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);

    // send PBO or host-mapped image data to texture
    const unsigned pboId = buffer->getGLBOId();
    GLvoid* imageData = 0;
    if (pboId)
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboId);
    else
        imageData = buffer->map(0, RT_BUFFER_MAP_READ);

    RTsize elmt_size = buffer->getElementSize();
    if (elmt_size % 8 == 0) glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
    else if (elmt_size % 4 == 0) glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    else if (elmt_size % 2 == 0) glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
    else                          glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    GLenum pixel_format = GL_RGBA;

    if (buffer_format == RT_FORMAT_UNSIGNED_BYTE4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, pixel_format, GL_UNSIGNED_BYTE, imageData);
    else if (buffer_format == RT_FORMAT_FLOAT4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, pixel_format, GL_FLOAT, imageData);
    else if (buffer_format == RT_FORMAT_FLOAT3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, pixel_format, GL_FLOAT, imageData);

    if (pboId)
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    else
        buffer->unmap();

}


