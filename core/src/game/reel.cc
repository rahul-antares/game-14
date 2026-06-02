#include "./reel.h"
#include "../utils/log.h"

int Reel::spin () {
    stop = rng.getScaled(reel.size());
    return stop;
}
