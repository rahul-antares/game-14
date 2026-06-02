#include "../harness.h"

int main () {
    Harness harness;
    HarnessInput input;

    input.iterations = 100;
    input.playOptions.betAmount = 10;
    input.playOptions.featureMode = 0;
    input.playOptions.buyFeature = false;

    harness.run (input);
    HarnessOutput output = harness.getStats ();

    return 0;
}
