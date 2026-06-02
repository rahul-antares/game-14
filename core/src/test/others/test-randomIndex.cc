#include <random>
#include <iostream>
#include <string>
#include <algorithm>

unsigned int uniform_distribution(unsigned int max) {
    // Create a random number engine and seed it with a random device
    static std::random_device rd;
    static std::mt19937 gen(rd());

    // Define a uniform distribution for unsigned integers between 0 and max-1
    std::uniform_int_distribution<unsigned int> distribution(0, max - 1);

    // Generate and return a random number
    return distribution(gen);
}

int getRandNum(const int *weights, unsigned int length) {
    unsigned int totalWeight = 0;

    // Calculate total weight
    for (unsigned int i = 0; i < length; i++)
        totalWeight += weights[i];

    // If total weight is 0, throw
    if (totalWeight == 0)
        throw std::invalid_argument("totalWeight cannot be zero");

    // Generate a random number
    unsigned int randNum   = uniform_distribution(totalWeight);
    unsigned int cumWeight = 0;

    for (unsigned int i = 0; i < length; i++) {
        cumWeight += weights[i];

        if (randNum < cumWeight)
            return i;
    }

    return length -1;
}


int main (int argc, char* argv[]) {
    int w[] = { 25, 25, 50 };
    unsigned int len = sizeof w/ sizeof *w;
    int wAcc[] = { 0, 0, 0, 0 };
    unsigned long counters[] = { 0, 0, 0, 0 };

    printf ("\nwAcc = [");
    for (unsigned int i = 0; i < len; i++) {
        if (i == 0)
            wAcc[i] = w[i];
        else
            wAcc[i] = wAcc[i-1] + w[i];

        printf ("%d, ", wAcc[i]);
    }

    printf ("]\n");

    unsigned long loopCount = std::strtol(argv[1], nullptr, 10);
    for (unsigned long i = 0; i < loopCount; i++) {
        int output = getRandNum (w, len);

        counters[output]++;
        /*
        for (unsigned int j = 0; j < len; j++) {
            if (output < wAcc[j]) {
                counters[j]++;
                break;
            }
        }
        */
    }

    printf ("\n output = [");
    for (unsigned int i = 0; i < len; i++) {
        printf ("%.2f%%, ", (double) counters[i] * 100/loopCount);
    }
    printf ("]\n");
    return 0;
}
