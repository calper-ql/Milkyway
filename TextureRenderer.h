//
// Created by calper on 4/19/19.
//

#ifndef MILKYWAYVR_TEXTURERENDERER_H
#define MILKYWAYVR_TEXTURERENDERER_H

#include "Utilities.h"

class TextureRenderer {
private:
    GLuint texture;
    GLuint VBO, VAO, EBO;
    GLuint program;

    bool inited;

public:
    TextureRenderer();

    bool init();

    float render(GLuint texture);
};


#endif //MILKYWAYVR_TEXTURERENDERER_H
