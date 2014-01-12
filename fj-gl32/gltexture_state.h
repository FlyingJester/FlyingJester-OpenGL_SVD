#ifndef FJ_GL32_GLTEXTURE_STATE_HEAD
#define FJ_GL32_GLTEXTURE_STATE_HEAD
#include "api.h"

extern FJ::SetupType lastSetup;

namespace FJ{
    namespace GLstate{
        namespace texture{

            /////
            //Call whenever changing the GL Texture state
            void resetSetup(void);

            /////
            // May be functionally identical to resetSetup().
            // Maintained as a hook for future use.
            void textureSetup(void);

            void primitiveSetup(void);

        }

        extern unsigned int emptyTexture;

    }
}
#endif
