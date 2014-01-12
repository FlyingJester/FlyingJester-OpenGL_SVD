#ifndef FJ_GL32_GLBLEND_STATE_HEAD
#define FJ_GL32_GLBLEND_STATE_HEAD
#include "api.h"

namespace FJ{
    namespace GLstate{
        namespace blendmode{

            extern FJ::Sphere::BlendMode lastMode;

            /////
            // Call whenever invalidating the blendmode state.

            void resetBlendMode(void);

            /////
            // Call to change the blendmode state.
            //   It is generally inadvisable to call with bmNone.
            //   That's what resetBlendMode is for.

            void setBlendMode(FJ::Sphere::BlendMode mode);

        }

    }
}
#endif
