#include "api.h"

#include <unistd.h>

void FJ::Sleep(unsigned int ms){

    /////
    //Very close to correct.
    usleep(ms<<10);

}
