#include "glblend_state.h"
#include "api.h"
#include <GL/gl.h>

using namespace FJ::GLstate::blendmode;
using FJ::Sphere::BlendMode;

FJ::Sphere::BlendMode FJ::GLstate::blendmode::lastMode = FJ::Sphere::bmNone;

void FJ::GLstate::blendmode::resetBlendMode(void){
    FJ::GLstate::blendmode::lastMode = FJ::Sphere::bmNone;
}

void FJ::GLstate::blendmode::setBlendMode(FJ::Sphere::BlendMode mode){
    if ((FJ::GLstate::blendmode::lastMode == mode)||(mode == FJ::Sphere::BlendMode::bmNone))
        return;

    /////
    // Set glBlendEquation

    switch (mode){
        case FJ::Sphere::BlendMode::bmNone:
        return;
        case FJ::Sphere::BlendMode::bmSubtract:
            glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
            break;
        case FJ::Sphere::BlendMode::bmMultiply:
        case FJ::Sphere::BlendMode::bmAdd:
        case FJ::Sphere::BlendMode::bmBlend:
        default:
            glBlendEquation(GL_FUNC_ADD);
    }

    /////
    // Set glBlendFunc

        switch (mode){
        case FJ::Sphere::BlendMode::bmAdd:
        case FJ::Sphere::BlendMode::bmSubtract:
            glBlendFunc(GL_ONE, GL_ONE);
            break;
        case FJ::Sphere::BlendMode::bmMultiply:
            glBlendFunc(GL_DST_COLOR, GL_ZERO);
            break;
        case FJ::Sphere::BlendMode::bmBlend:
        default:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}
