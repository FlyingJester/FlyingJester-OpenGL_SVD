#include "gltexture_state.h"

#include <GL/gl.h>

/////
//Calls to store the GL texturing state in software.

void FJ::GLstate::texture::resetSetup(void){
	lastSetup = FJ::eNone;
}

void FJ::GLstate::texture::textureSetup(void){

    unsigned int m[] = {
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF
    };

	glColorPointer(4, GL_UNSIGNED_BYTE, 0, m);

	lastSetup = FJ::eTexture;
}

void FJ::GLstate::texture::primitiveSetup(void){

	if(lastSetup == FJ::ePrimitive)
		return;

	lastSetup = FJ::ePrimitive;

    glBindTexture(GL_TEXTURE_2D, FJ::GLstate::emptyTexture);

}

/////
// Previous state

FJ::SetupType lastSetup = FJ::eNone;

/////
// The empty texture

GLuint FJ::GLstate::emptyTexture = 0;
