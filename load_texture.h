#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include <stb_image.h>

inline GLuint LoadTexture(const std::string filename, const GLenum wrap, const GLenum filter)
{
    GLuint result;
    glGenTextures(1, &result);
    glBindTexture(GL_TEXTURE_2D, result);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

    int width, height, channels;
    unsigned char * data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
    if (!data || width <= 0 || height <= 0)
    {
        std::cout << "Invalid texture: " << filename << std::endl;
        return 0;
    }
    GLuint type;
    switch (channels)
    {
        case 1: type = GL_RED; break;
        case 2: type = GL_RG; break;
        case 3: type = GL_RGB; break;
        case 4: type = GL_RGBA; break;
        default:
            std::cout << "Invalid channel count: " << channels << std::endl;
            return 0;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    return result;
}