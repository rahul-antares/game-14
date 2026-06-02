#pragma once

#include <vector>
#include "./symbols.h"
#include "rng.h"

using Reel_V = std::vector<Symbol>;

class Reel {
    public:
        Reel (int index, const Reel_V& symbols, RngLib& rng) :
            id(index), reel(symbols), rng(rng)
    {};

        inline unsigned int size () { return reel.size(); }
        inline int getId () { return id; }
        inline int setStop (int s) { stop = s < 0 ? reel.size() - std::abs(s)%reel.size() : s%reel.size(); return stop; }
        inline int moveBy (int m) { return setStop(stop + m); }
        inline int stopPos () { return stop; }
        inline Symbol symbolAt (int pos) { return reel[pos % reel.size()]; }
        inline const Reel_V getReel() { return reel; };
        int spin ();

    private:
        const int id;
        const Reel_V& reel;
        RngLib& rng;
        int stop = -1;
};


//
// ------------------------- Regular Reel Sets -------------------------
//
// Set 1
const std::vector<std::vector<Symbol>> RGSet1 =  {
    { CS, CS, CS, CS, CS, CS, L9, H4, L10, CS, L9, H3, L6, SC, L8, L7, H1, L10, H2, L9, H3, H4, CS, CS, H2, H4, L7, H3, H4, H2, CS, CS, L6, L8, H4, L9, CS, L7, L6, L9, H4, L7, H3, L8, L10, L6, H2, L7, H4, L10, SC, L6, L10, H1, L8, L7, L6, L8, },
    {
        CS, CS, CS, CS, CS, CS, L7, L8, L9, L7, CS, L10, H4, L9, L8, CS, CS, H4, L6, H1, L8, H4, H2, CS, CS, H3, L6, L9, L10, H2, H1, H4, L8, L10, H3, L9, H4, L6, L7, H4,
        L9, H3, L10, SC, L6, L7, H2, L6, H3, L10, CS, L6, H2, H4, L8,
    },
    {
        CS, CS, CS, CS, CS, CS, L7, L9, CS, L6, L8, CS, L7, L6, L10, CS, CS, H4, H2, L8, H3, H4, CS, CS, L7, H1, L10, H2, H4, H3, L9, H4, L6, L9, L8, L7, H2, L10, L7, H2,
        L8, H4, L6, L9, H4, L10, H3, L6, SC, L8, L6, H1, H4, L8, L10, L7, L6, L9, H3,
    },
    {
        CS, CS, CS, CS, CS, CS, L9, L6, H3, CS, L8, H4, L7, L6, L9, SC, L10, H4, L9, CS, CS, L8, H4, L7, H2, H1, CS, CS, L8, L6, SC, L10, H4, H3, L6, CS, L7, H4, L9, H3,
        L7, H2, L10, H4, L7, L8, H2, L9, H4, L6, H2, L10, H1, H3, L6, L7, L9, L10,
    },
    {
        CS, CS, CS, CS, CS, CS, H4, H2, L6, CS, H4, L8, H3, CS, CS, L7, H2, H4, L7, L9, L10, L8, H4, L6, H3, L7, L9, CS, CS, L8, H4, H3, L8, SC, L10, H4, H3, L9, H1, L6,
        H2, L10, H4, L9, H1, L6, L7, L8, L9, SC, L10, CS, L6, L7, H2, L10, L7,
    },
};

const std::vector<std::vector<Symbol>> RGSet2 =  {
    {
        CS, CS, CS, CS, CS, CS, L9, H4, L10, CS, L9, H3, L6, SC, L8, L7, H1, L10, H2, L9, H3, H4, CS, CS, H2, H4, L7, H3, H4, H2, CS, CS, L6, SC, L8, H4, CS, L9, L7, SC,
        L6, L9, H4, L7, H3, L8, L10, L6, H2, L7, H4, L10, SC, L6, L10, H1, L8, L7, SC, L6, L8,
    },
    {
        CS, CS, CS, CS, CS, CS, L7, L8, SC, L9, L7, CS, L10, H4, L9, SC, L8, CS, CS, H4, L6, H1, L8, H4, H2, CS, CS, H3, L6, SC, L9, L10, H2, H1, H4, L8, SC, L10, H3, L9,
        H4, L6, CS, L7, H4, L9, H3, L10, L6, SC, L7, H2, L6, H3, L10, L6, H2, H4, L8,
    },
    {
        CS, CS, CS, CS, CS, CS, L7, L9, CS, L6, SC, L8, CS, L7, L6, L10, CS, CS, H4, H2, L8, H3, H4, CS, CS, L7, H1, L10, H2, H4, H3, L9, H4, L6, L9, L8, L7, H2, L10, L7,
        H2, L8, H4, L6, L9, H4, L10, H3, L6, SC, L8, L6, H1, H4, L8, L10, L7, L6, L9, H3,
    },
    {
        CS, CS, CS, CS, CS, CS, L9, L6, H3, CS, L8, H4, L7, L6, L9, L10, H4, L9, CS, CS, L8, H4, L7, H2, H1, CS, CS, L8, L6, L10, H4, H3, L6, L7, H4, L9, H3, L7, H2, L10,
        H4, L7, L8, H2, L9, H4, L6, H2, L10, H1, H3, L6, CS, L7, L9, L10,
    },
    {
        CS, CS, CS, CS, CS, CS, H4, H2, L6, CS, H4, L8, H3, CS, CS, L7, H2, L7, H4, L9, L10, L8, H4, L6, H3, L7, L9, CS, CS, L8, H4, H3, L8, L10, H4, H3, L9, H1, L6, H2,
        L10, H4, L9, H1, L6, L7, L8, CS, L9, L10, L6, L7, H2, L10, L7,
    },
};

const std::vector<std::vector<Symbol>> RGSet3 =  {
    {
        CS, CS, CS, CS, CS, CS, L9, H4, L10, CS, L9, H3, L6, SC, L8, L7, H1, L10, H2, L9, H3, H4, CS, CS, H2, H4, L7, H3, H4, H2, CS, CS, L6, SC, L8, H4, CS, L9, L7, SC,
        L6, L9, H4, L7, H3, L8, L10, L6, H2, L7, H4, L10, SC, L6, L10, H1, L8, L7, SC, L6, L8,
    },
    {
        CS, CS, CS, CS, CS, CS, L7, L8, SC, L9, L7, CS, L10, H4, L9, SC, L8, CS, CS, H4, L6, H1, L8, H4, H2, CS, CS, H3, L6, SC, L9, L10, H2, H1, H4, L8, SC, L10, H3, L9,
        H4, L6, CS, L7, H4, L9, H3, L10, L6, SC, L7, H2, L6, H3, L10, L6, H2, H4, L8,
    },
    {
        CS, CS, CS, CS, CS, CS, L7, L9, CS, L6, L8, CS, L7, L6, L10, CS, CS, H4, H2, L8, H3, H4, CS, CS, L7, H1, L10, H2, H4, H3, L9, H4, L6, L9, L8, L7, H2, L10, L7, H2,
        L8, H4, L6, L9, H4, L10, L6, H3, L8, L6, H1, H4, L8, L10, L7, L6, L9, H3,
    },
    {
        CS, CS, CS, CS, CS, CS, L9, L6, H3, CS, L8, H4, L7, L6, SC, L9, L10, H4, L9, CS, CS, L8, H4, L7, H2, H1, CS, CS, L8, L6, L10, H4, H3, L6, SC, L7, H4, L9, H3, L7,
        H2, L10, H4, L7, L8, H2, L9, H4, L6, H2, L10, H1, H3, L6, CS, L7, L9, L10,
    },
    {
        CS, CS, CS, CS, CS, CS, H4, H2, L6, CS, H4, L8, H3, CS, CS, L7, H2, L9, H4, L7, L10, L8, H4, L6, H3, L7, L9, CS, CS, L8, H4, H3, L8, CS, L10, H4, H3, L9, H1, L6,
        H2, L10, H4, L9, H1, L6, L7, L8, L9, L10, L6, L7, H2, L10, L7,
    },
};

const std::vector<std::vector<Symbol>> RGSet4 =  {
    {
        CS, CS, CS, CS, CS, CS, L9, H4, L10, CS, L9, H3, L6, SC, L8, L7, H1, L10, H2, L9, H3, H4, CS, CS, H2, H4, L7, H3, H4, H2, CS, CS, L6, SC, L8, H4, CS, L9, L7, SC,
        L6, L9, H4, L7, H3, L8, L10, L6, H2, L7, H4, L10, SC, L6, L10, H1, L8, L7, SC, L6, L8,
    },
    {
        CS, CS, CS, CS, CS, CS, L7, L8, SC, L9, L7, CS, L10, H4, L9, SC, L8, CS, CS, H4, L6, H1, L8, H4, H2, CS, CS, H3, L6, SC, L9, L10, H2, H1, H4, L8, SC, L10, H3, L9,
        H4, L6, L7, H4, L9, H3, L10, L6, SC, L7, H2, L6, H3, L10, CS, L6, H2, H4, L8,
    },
    {
        CS, CS, CS, CS, CS, CS, L7, L9, CS, L6, L8, CS, L7, L6, L10, CS, CS, H4, H2, L8, H3, H4, CS, CS, L7, H1, L10, H2, H4, H3, L9, H4, L6, L9, L8, L7, H2, L10, L7, H2,
        L8, H4, L6, L9, H4, L10, L6, H3, L8, L6, H1, H4, L8, L10, L7, L6, L9, H3,
    },
    {
        CS, CS, CS, CS, CS, CS, L9, L6, H3, CS, L8, H4, L7, L6, L9, L10, H4, L9, CS, CS, L8, H4, L7, H2, H1, CS, CS, L8, L6, L10, H4, H3, L6, CS, L7, H4, L9, H3, L7, H2,
        L10, H4, L7, L8, H2, L9, H4, L6, H2, L10, H1, H3, L6, L7, L9, L10,
    },
    {
        CS, CS, CS, CS, CS, CS, H4, H2, L6, CS, H4, L8, H3, CS, CS, L7, H2, L9, H4, L7, L10, L8, H4, L6, H3, L7, L9, CS, CS, L8, H4, H3, L8, CS, L10, H4, H3, L9, H1, L6,
        H2, L10, H4, L9, H1, L6, SC, L7, L8, L9, SC, L10, L6, L7, H2, L10, L7,
    },
};

const std::vector<std::vector<Symbol>> RGSet5 =  {
    {
        CS, CS, CS, CS, CS, CS, L9, H4, L10, CS, L9, H3, L6, SC, L8, L7, H1, L10, H2, L9, H3, H4, CS, CS, H2, H4, L7, H3, H4, H2, CS, CS, L6, SC, L8, H4, SC, L9, L7, SC,
        L6, L9, H4, L7, H3, L8, SC, L10, L6, H2, L7, H4, L10, SC, L6, L10, H1, L8, CS, L7, SC, L6, L8,
    },
    {
        CS, CS, CS, CS, CS, CS, L7, L8, L9, L7, CS, L10, H4, L9, L8, CS, CS, H4, L6, H1, L8, H4, H2, CS, CS, H3, L6, L9, L10, H2, H1, H4, L8, L10, H3, L9, H4, L6, CS, L7,
        H4, L9, H3, L10, L6, L7, H2, L6, H3, L10, L6, H2, H4, L8,
    },
    {
        CS, CS, CS, CS, CS, CS, L7, L9, CS, L6, SC, L8, CS, L7, L6, L10, CS, CS, H4, H2, L8, H3, H4, CS, CS, L7, H1, L10, H2, H4, H3, SC, L9, H4, L6, SC, L9, L8, SC, L7,
        H2, L10, SC, L7, H2, L8, H4, L6, L9, H4, L10, H3, L6, SC, L8, L6, H1, H4, L8, L10, L7, L6, L9, H3,
    },
    {
        CS, CS, CS, CS, CS, CS, L9, L6, H3, CS, L8, H4, L7, L6, L9, L10, H4, L9, CS, CS, L8, H4, L7, H2, H1, CS, CS, L8, L6, L10, H4, H3, L6, CS, L7, H4, L9, H3, L7, H2,
        L10, H4, L7, L8, H2, L9, H4, L6, H2, L10, H1, H3, L6, SC, L7, L9, L10,
    },
    {
        CS, CS, CS, CS, CS, CS, H4, H2, L6, CS, H4, L8, H3, CS, CS, L7, H2, L9, H4, L7, L10, L8, H4, L6, H3, L7, L9, CS, CS, L8, H4, H3, L8, L10, H4, H3, L9, H1, L6, H2,
        L10, H4, L9, H1, L6, L7, L8, CS, L9, L10, L6, L7, H2, L10, L7,
    },
};

const std::vector<std::vector<Symbol>> RGSet6 =  {
    {
        CS, CS, CS, CS, CS, CS, L9, H4, L10, CS, L9, H3, L6, SC, L8, L7, H1, L10, H2, L9, H3, H4, CS, CS, H2, H4, L7, H3, H4, H2, CS, CS, L6, SC, L8, H4, L9, L7, SC, L6,
        L9, H4, L7, H3, L8, SC, L10, L6, H2, L7, H4, L10, SC, L6, L10, H1, L8, CS, L7, SC, L6, L8,
    },
    {
        CS, CS, CS, CS, CS, CS, L7, L8, L9, L7, CS, L10, H4, L9, L8, CS, CS, H4, L6, H1, L8, H4, H2, CS, CS, H3, L6, L9, L10, H2, H1, H4, L8, L10, H3, L9, H4, L6, L7, H4,
        L9, H3, L10, CS, L6, L7, H2, L6, H3, L10, L6, H2, H4, L8,
    },
    {
        CS, CS, CS, CS, CS, CS, L7, L9, CS, L6, SC, L8, CS, L7, L6, L10, CS, CS, H4, H2, L8, H3, H4, CS, CS, L7, H1, L10, H2, H4, H3, SC, L9, H4, L6, SC, L9, L8, SC, L7,
        H2, L10, SC, L7, H2, L8, H4, L6, L9, H4, L10, H3, L6, SC, L8, L6, H1, H4, L8, L10, L7, L6, L9, H3,
    },
    {
        CS, CS, CS, CS, CS, CS, L9, L6, H3, CS, L8, H4, L7, L6, L9, L10, H4, L9, CS, CS, L8, H4, L7, H2, H1, CS, CS, L8, L6, L10, H4, H3, L6, CS, L7, H4, L9, H3, L7, H2,
        L10, H4, L7, L8, H2, L9, H4, L6, H2, L10, H1, H3, L6, L7, L9, L10,
    },
    {
        CS, CS, CS, CS, CS, CS, H4, H2, L6, CS, H4, L8, H3, CS, CS, L7, H2, L9, H4, L7, SC, L10, L8, H4, L6, H3, L7, L9, CS, CS, L8, H4, H3, L8, L10, H4, H3, L9, H1, L6,
        H2, L10, H4, L9, H1, L6, L7, L8, SC, L9, L10, CS, L6, L7, H2, L10, L7,
    },
};
const std::vector<std::vector<Symbol>> RGSet7 =  {
    {
        CS, CS, CS, CS, CS, CS, L9, H4, L10, CS, L9, H3, L6, SC, L8, L7, H1, L10, H2, L9, H3, H4, CS, CS, H2, H4, L7, H3, H4, H2, CS, CS, L6, SC, L8, H4, CS, L9, L7, SC,
        L6, L9, H4, L7, H3, L8, SC, L10, L6, H2, L7, H4, L10, SC, L6, L10, H1, L8, L7, SC, L6, L8,
    },
    {
        CS, CS, CS, CS, CS, CS, L7, L8, L9, L7, CS, L10, H4, L9, L8, CS, CS, H4, L6, H1, L8, H4, H2, CS, CS, H3, L6, L9, L10, H2, H1, H4, L8, L10, H3, L9, H4, L6, L7, H4,
        L9, H3, L10, L6, L7, H2, L6, H3, L10, CS, L6, H2, H4, L8,
    },
    {
        CS, CS, CS, CS, CS, CS, L7, L9, CS, L6, L8, CS, L7, L6, L10, CS, CS, H4, H2, L8, H3, H4, CS, CS, L7, H1, L10, H2, H4, H3, L9, H4, L6, L9, L8, L7, H2, L10, L7, H2,
        L8, H4, L6, L9, H4, L10, L6, H3, L8, L6, H1, H4, L8, L10, L7, L6, L9, H3,
    },
    {
        CS, CS, CS, CS, CS, CS, L9, SC, L6, H3, CS, L8, H4, L7, SC, L6, L9, L10, H4, L9, CS, CS, L8, H4, L7, CS, H2, H1, CS, CS, L8, SC, L6, L10, H4, H3, L6, SC, L7, H4,
        L9, H3, L7, H2, L10, H4, L7, SC, L8, H2, L9, H4, L6, H2, L10, H1, H3, L6, SC, L7, L9, L10,
    },
    {
        CS, CS, CS, CS, CS, CS, H4, H2, L6, CS, H4, L8, H3, CS, CS, L7, H2, L9, H4, L7, CS, L10, L8, H4, L6, H3, L7, L9, CS, CS, L8, H4, H3, L8, L10, H4, H3, L9, H1, L6,
        H2, L10, H4, L9, H1, L6, L7, L8, SC, L9, L10, L6, L7, H2, L10, L7,
    },
};
const std::vector<std::vector<Symbol>> RGSet8 =  {
    {
        CS, CS, CS, CS, CS, CS, L9, H4, L10, CS, L9, H3, L6, CS, L8, L7, H1, L10, H2, L9, H3, H4, CS, CS, H2, H4, L7, H3, H4, H2, CS, CS, L6, L8, H4, L9, L7, L6, L9, H4,
        L7, H3, L8, L10, L6, H2, L7, L10, H4, L6, L10, H1, L8, L7, L6, L8,
    },
    {
        CS, CS, CS, CS, CS, CS, L7, L8, SC, L9, L7, CS, L10, H4, L9, SC, L8, CS, CS, H4, L6, H1, L8, H4, H2, CS, CS, H3, L6, SC, L9, L10, H2, H1, H4, L8, SC, L10, H3, L9,
        H4, L6, SC, L7, H4, L9, H3, L10, L6, SC, L7, H2, L6, H3, L10, CS, L6, H2, H4, L8,
    },
    {
        CS, CS, CS, CS, CS, CS, L7, L9, CS, L6, SC, L8, CS, L7, L6, L10, CS, CS, H4, H2, L8, H3, H4, CS, CS, L7, H1, L10, H2, H4, H3, SC, L9, H4, L6, SC, L9, L8, SC, L7,
        H2, L10, SC, L7, H2, L8, H4, L6, L9, H4, L10, H3, L6, SC, L8, L6, H1, H4, L8, L10, L7, L6, L9, H3,
    },
    {
        CS, CS, CS, CS, CS, CS, L9, L6, H3, CS, L8, H4, L7, L6, SC, L9, L10, H4, L9, CS, CS, L8, H4, L7, H2, H1, CS, CS, L8, L6, L10, H4, H3, L6, CS, L7, H4, L9, H3, L7,
        H2, L10, H4, L7, L8, H2, L9, H4, L6, H2, L10, H1, H3, L6, L7, SC, L9, L10,
    },
    {
        CS, CS, CS, CS, CS, CS, H4, H2, L6, CS, H4, L8, H3, CS, CS, L7, H2, L9, H4, L7, L10, L8, H4, L6, H3, L7, L9, CS, CS, L8, H4, H3, L8, CS, L10, H4, H3, L9, H1, L6,
        H2, L10, H4, L9, H1, L6, L7, L8, L9, L10, L6, L7, H2, L10, L7,
    },
};

const std::vector<std::vector<Symbol>> RGSet9 =  {
    {
        CS, CS, CS, CS, CS, CS, L9, H4, L10, CS, L9, H3, L6, L8, L7, H1, L10, H2, L9, H3, H4, CS, CS, H2, H4, L7, H3, H4, H2, CS, CS, L6, L8, H4, L9, CS, L7, L6, L9, H4,
        L7, H3, L8, L10, L6, H2, L7, L10, H4, L6, L10, H1, L8, L7, L6, L8,
    },
    {
        CS, CS, CS, CS, CS, CS, L7, L8, SC, L9, L7, CS, L10, H4, L9, SC, L8, CS, CS, H4, L6, H1, L8, H4, H2, CS, CS, H3, L6, SC, L9, L10, H2, H1, H4, L8, SC, L10, H3, L9,
        H4, L6, CS, L7, H4, L9, H3, L10, L6, SC, L7, H2, L6, H3, L10, L6, H2, H4, L8,
    },
    {
        CS, CS, CS, CS, CS, CS, L7, L9, CS, L6, SC, L8, CS, L7, L6, L10, CS, CS, H4, H2, L8, H3, H4, CS, CS, L7, H1, L10, H2, H4, H3, SC, L9, H4, L6, SC, L9, L8, SC, L7,
        H2, L10, L7, H2, L8, H4, L6, L9, H4, L10, H3, L6, SC, L8, L6, H1, H4, L8, L10, L7, L6, L9, H3,
    },
    {
        CS, CS, CS, CS, CS, CS, L9, L6, H3, CS, L8, H4, L7, L6, L9, L10, H4, L9, CS, CS, L8, H4, L7, H2, H1, CS, CS, L8, L6, L10, H4, H3, L6, CS, L7, H4, L9, H3, L7, H2,
        L10, H4, L7, L8, H2, L9, H4, L6, H2, L10, H1, H3, L6, L7, L9, L10,
    },
    {
        CS, CS, CS, CS, CS, CS, H4, H2, L6, CS, H4, L8, H3, CS, CS, L7, H2, L9, H4, L7, SC, L10, L8, H4, L6, H3, L7, L9, CS, CS, L8, H4, H3, L8, L10, H4, H3, L9, H1, L6,
        H2, L10, H4, L9, H1, L6, L7, SC, L8, L9, CS, L10, L6, L7, H2, L10, L7,
    },
};

const std::vector<std::vector<Symbol>> RGSet10 =  {
    {
        CS, CS, CS, CS, CS, CS, L9, H4, L10, CS, L9, H3, L6, L8, L7, H1, L10, H2, L9, H3, H4, CS, CS, H2, H4, L7, H3, H4, H2, CS, CS, L6, L8, H4, L9, CS, L7, L6, L9, H4,
        L7, H3, L8, L10, L6, H2, L7, L10, H4, L6, L10, H1, L8, L7, L6, L8,
    },
    {
        CS, CS, CS, CS, CS, CS, L7, L8, SC, L9, L7, CS, L10, H4, L9, SC, L8, CS, CS, H4, L6, H1, L8, H4, H2, CS, CS, H3, L6, SC, L9, CS, L10, H2, H1, H4, L8, SC, L10, H3,
        L9, H4, L6, SC, L7, H4, L9, H3, L10, L6, SC, L7, H2, L6, H3, L10, L6, H2, H4, L8,
    },
    {
        CS, CS, CS, CS, CS, CS, L7, L9, CS, L6, L8, CS, L7, L6, L10, CS, CS, H4, H2, L8, H3, H4, CS, CS, L7, H1, L10, H2, H4, H3, L9, H4, L6, L9, L8, L7, H2, L10, L7, H2,
        L8, H4, L6, L9, H4, L10, L6, H3, L8, L6, H1, H4, L8, L10, L7, L6, L9, H3,
    },
    {
        CS, CS, CS, CS, CS, CS, L9, SC, L6, H3, CS, L8, H4, L7, SC, L6, L9, L10, H4, L9, CS, CS, L8, H4, L7, H2, H1, CS, CS, L8, SC, L6, CS, L10, H4, H3, L6, SC, L7, H4,
        L9, H3, L7, H2, L10, H4, L7, SC, L8, H2, L9, H4, L6, H2, L10, H1, H3, L6, SC, L7, L9, L10,
    },
    {
        CS, CS, CS, CS, CS, CS, H4, H2, L6, CS, H4, L8, H3, CS, CS, L7, H2, L9, H4, L7, L10, L8, H4, L6, H3, L7, L9, CS, CS, L8, H4, H3, L8, CS, L10, H4, H3, L9, H1, L6,
        H2, L10, H4, L9, H1, L6, L7, L8, SC, L9, L10, L6, L7, H2, L10, L7,
    },
};

const std::vector<std::vector<Symbol>> RGSet11 =  {
    {
        CS, CS, CS, L10, H2, L9, H1, L8, H4, L7, H3, L6, H1, L7, H2, H3, L6, H4,
    },
    {
        CS, CS, CS, L10, H1, L9, H2, L8, H3, L7, H4, L6, H1, L7, L8, H3, H2, H4, L6,
    },
    {
        CS, CS, CS, L9, H2, L10, H1, L7, H4, L8, H3, L6, H1, L8, L7, H2, H3, L6, H4,
    },
    {
        CS, CS, CS, L9, H1, L10, H2, L7, H4, L8, H3, L6, H1, L7, L8, H3, H2, H4, L6,
    },
    {
        CS, CS, CS, H3, L10, L6, H2, H4, L8, H3, L7, H4, H1, L9, H2, L7, H3, H4, L8, H1, L6, H3,
    },
};

//
// ------------------------- Bonus Boost Reel Sets -------------------------
//

// Set 1
const std::vector<std::vector<Symbol>> BBSet1 = {
    { CS, CS, CS, CS, CS, CS, L9, H4, L10, CS, L9, H3, L6, SC, L8, L7, H1, L10, H2, L9, H3, H4, CS, CS, H2, H4, L7, H3, H4, H2, CS, CS, L6, L8, H4, L9, CS, L7, L6, L9, H4, L7, H3, L8, L10, L6, H2, L7, H4, L10, SC, L6, L10, H1, L8, L7, L6, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L8, L9, L7, CS, L10, H4, L9, L8, CS, CS, H4, L6, H1, L8, H4, H2, CS, CS, H3, L6, L9, L10, H2, H1, H4, L8, L10, H3, L9, H4, L6, L7, H4, L9, H3, L10, SC, L6, L7, H2, L6, H3, L10, CS, L6, H2, H4, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L9, CS, L6, L8, CS, L7, L6, L10, CS, CS, H4, H2, L8, H3, H4, CS, CS, L7, H1, L10, H2, H4, H3, L9, H4, L6, L9, L8, L7, H2, L10, L7, H2, L8, H4, L6, L9, H4, L10, H3, L6, SC, L8, L6, H1, H4, L8, L10, L7, L6, L9, H3, },

    { CS, CS, CS, CS, CS, CS, L9, L6, H3, CS, L8, H4, L7, L6, L9, SC, L10, H4, L9, CS, CS, L8, H4, L7, H2, H1, CS, CS, L8, L6, SC, L10, H4, H3, L6, CS, L7, H4, L9, H3, L7, H2, L10, H4, L7, L8, H2, L9, H4, L6, H2, L10, H1, H3, L6, L7, L9, L10, },

    { CS, CS, CS, CS, CS, CS, H4, H2, L6, CS, H4, L8, H3, CS, CS, L7, H2, H4, L7, L9, L10, L8, H4, L6, H3, L7, L9, CS, CS, L8, H4, H3, L8, SC, L10, H4, H3, L9, H1, L6, H2, L10, H4, L9, H1, L6, L7, L8, L9, SC, L10, CS, L6, L7, H2, L10, L7, },
};

// Set 2
const std::vector<std::vector<Symbol>> BBSet2 = {
    { CS, CS, CS, CS, CS, CS, L9, H4, L10, CS, L9, H3, L6, SC, L8, L7, H1, L10, H2, L9, H3, H4, SC, CS, CS, H2, H4, L7, H3, H4, H2, CS, CS, L6, SC, L8, H4, CS, L9, L7, SC, L6, L9, H4, L7, H3, L8, L10, L6, H2, L7, H4, L10, SC, L6, L10, H1, L8, L7, SC, L6, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L8, L9, L7, CS, L10, H4, L9, SC, L8, CS, CS, H4, L6, SC, H1, L8, H4, H2, CS, CS, H3, L6, SC, L9, L10, H2, H1, H4, L8, SC, L10, H3, L9, H4, L6, CS, L7, H4, L9, H3, L10, L6, SC, L7, H2, L6, H3, L10, L6, H2, H4, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L9, CS, L6, SC, L8, CS, L7, L6, L10, CS, CS, H4, H2, SC, L8, H3, H4, CS, CS, L7, H1, L10, H2, H4, H3, L9, H4, L6, L9, L8, L7, H2, L10, L7, H2, L8, H4, L6, L9, H4, L10, H3, L6, SC, L8, L6, H1, H4, L8, L10, L7, L6, L9, H3, },

    { CS, CS, CS, CS, CS, CS, L9, L6, H3, CS, L8, H4, L7, L6, L9, L10, H4, L9, CS, CS, L8, H4, L7, H2, H1, CS, CS, L8, L6, L10, H4, H3, L6, L7, H4, L9, H3, L7, H2, L10, H4, L7, L8, H2, L9, H4, L6, H2, L10, H1, H3, L6, CS, L7, L9, L10, },

    { CS, CS, CS, CS, CS, CS, H4, H2, L6, CS, H4, L8, H3, CS, CS, L7, H2, L7, H4, L9, L10, L8, H4, L6, H3, L7, L9, CS, CS, L8, H4, H3, L8, L10, H4, H3, L9, H1, L6, H2, L10, H4, L9, H1, L6, L7, L8, CS, L9, L10, L6, L7, H2, L10, L7, },
};

// Set 3
const std::vector<std::vector<Symbol>> BBSet3 = {
    { CS, CS, CS, CS, CS, CS, L9, H4, L10, CS, L9, H3, L6, L8, L7, H1, L10, H2, L9, H3, H4, CS, CS, H2, H4, L7, SC, H3, H4, H2, CS, CS, L6, SC, L8, H4, CS, L9, L7, SC, L6, L9, H4, L7, H3, L8, L10, L6, H2, L7, H4, L10, SC, L6, L10, H1, L8, L7, SC, L6, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L8, SC, L9, L7, CS, L10, H4, L9, SC, L8, CS, CS, H4, L6, H1, L8, H4, H2, CS, CS, H3, L6, SC, L9, L10, H2, SC, H1, H4, L8, SC, L10, H3, L9, H4, L6, CS, L7, H4, L9, H3, L10, L6, SC, L7, H2, L6, H3, L10, L6, H2, H4, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L9, CS, L6, L8, CS, L7, L6, L10, CS, CS, H4, H2, L8, H3, H4, CS, CS, L7, H1, L10, H2, H4, H3, L9, H4, L6, L9, L8, L7, H2, L10, L7, H2, L8, H4, L6, L9, H4, L10, L6, H3, L8, L6, H1, H4, L8, L10, L7, L6, L9, H3, },

    { CS, CS, CS, CS, CS, CS, L9, L6, H3, CS, L8, H4, L7, L6, SC, L9, L10, H4, L9, CS, CS, L8, SC, H4, L7, H2, H1, CS, CS, L8, L6, L10, H4, H3, L6, SC, L7, H4, L9, H3, L7, H2, L10, H4, L7, L8, H2, L9, H4, L6, H2, L10, H1, H3, L6, CS, L7, L9, L10, },

    { CS, CS, CS, CS, CS, CS, H4, H2, L6, CS, H4, L8, H3, CS, CS, L7, H2, L9, H4, L7, L10, L8, H4, L6, H3, L7, L9, CS, CS, L8, H4, H3, L8, CS, L10, H4, H3, L9, H1, L6, H2, L10, H4, L9, H1, L6, L7, L8, L9, L10, L6, L7, H2, L10, L7, },
};

// Set 4
const std::vector<std::vector<Symbol>> BBSet4 = {
    { CS, CS, CS, CS, CS, CS, L9, H4, L10, CS, L9, H3, L6, SC, L8, L7, H1, L10, H2, L9, H3, H4, SC, CS, CS, H2, H4, L7, H3, H4, H2, CS, CS, L6, SC, L8, H4, CS, L9, L7, SC, L6, L9, H4, L7, H3, L8, L10, L6, H2, L7, H4, L10, SC, L6, L10, H1, L8, L7, SC, L6, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L8, SC, L9, L7, CS, L10, H4, L9, L8, CS, CS, H4, L6, SC, H1, L8, H4, H2, CS, CS, H3, L6, SC, L9, L10, H2, H1, H4, L8, SC, L10, H3, L9, H4, L6, L7, H4, L9, H3, L10, L6, SC, L7, H2, L6, H3, L10, CS, L6, H2, H4, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L9, CS, L6, L8, CS, L7, L6, L10, CS, CS, H4, H2, L8, H3, H4, CS, CS, L7, H1, L10, H2, H4, H3, L9, H4, L6, L9, L8, L7, H2, L10, L7, H2, L8, H4, L6, L9, H4, L10, L6, H3, L8, L6, H1, H4, L8, L10, L7, L6, L9, H3, },

    { CS, CS, CS, CS, CS, CS, L9, L6, H3, CS, L8, H4, L7, L6, L9, L10, H4, L9, CS, CS, L8, H4, L7, H2, H1, CS, CS, L8, L6, L10, H4, H3, L6, CS, L7, H4, L9, H3, L7, H2, L10, H4, L7, L8, H2, L9, H4, L6, H2, L10, H1, H3, L6, L7, L9, L10, },

    { CS, CS, CS, CS, CS, CS, H4, H2, L6, CS, H4, L8, H3, CS, CS, L7, H2, L9, H4, L7, L10, SC, L8, H4, L6, H3, L7, L9, CS, CS, L8, H4, H3, L8, CS, L10, H4, H3, L9, H1, L6, H2, L10, H4, L9, H1, L6, SC, L7, L8, L9, SC, L10, L6, L7, H2, L10, L7, },
};

// Set 5
const std::vector<std::vector<Symbol>> BBSet5 = {
    { CS, CS, CS, CS, CS, CS, L9, H4, L10, CS, L9, H3, L6, L8, L7, H1, L10, H2, SC, L9, H3, H4, CS, CS, H2, H4, L7, H3, H4, H2, CS, CS, L6, SC, L8, H4, SC, L9, L7, SC, L6, L9, H4, L7, H3, L8, SC, L10, L6, H2, L7, H4, L10, SC, L6, L10, H1, L8, CS, L7, SC, L6, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L8, L9, L7, CS, L10, H4, L9, L8, CS, CS, H4, L6, H1, L8, H4, H2, CS, CS, H3, L6, L9, L10, H2, H1, H4, L8, L10, H3, L9, H4, L6, CS, L7, H4, L9, H3, L10, L6, L7, H2, L6, H3, L10, L6, H2, H4, L8, },
    { CS, CS, CS, CS, CS, CS, L7, L9, CS, L6, SC, L8, CS, L7, L6, L10, CS, CS, H4, H2, L8, H3, SC, H4, CS, CS, L7, H1, L10, H2, H4, H3, SC, L9, H4, L6, SC, L9, L8, SC, L7, H2, L10, SC, L7, H2, L8, H4, L6, L9, H4, L10, H3, L6, SC, L8, L6, H1, H4, L8, L10, L7, L6, L9, H3, },

    { CS, CS, CS, CS, CS, CS, L9, L6, H3, CS, L8, H4, L7, L6, L9, L10, H4, L9, CS, CS, L8, H4, L7, H2, H1, CS, CS, L8, SC, L6, L10, H4, H3, L6, CS, L7, H4, L9, H3, L7, H2, L10, H4, L7, L8, H2, L9, H4, L6, H2, L10, H1, H3, L6, SC, L7, L9, L10, },

    { CS, CS, CS, CS, CS, CS, H4, H2, L6, CS, H4, L8, H3, CS, CS, L7, H2, L9, H4, L7, L10, L8, H4, L6, H3, L7, L9, CS, CS, L8, H4, H3, L8, L10, H4, H3, L9, H1, L6, H2, L10, H4, L9, H1, L6, L7, L8, CS, L9, L10, L6, L7, H2, L10, L7, },
};

// Set 6
const std::vector<std::vector<Symbol>> BBSet6 = {
    { CS, CS, CS, CS, CS, CS, L9, H4, L10, CS, L9, H3, L6, SC, L8, L7, H1, L10, H2, L9, H3, H4, SC, CS, CS, H2, H4, L7, H3, H4, H2, CS, CS, L6, SC, L8, H4, L9, L7, SC, L6, L9, H4, L7, H3, L8, SC, L10, L6, H2, L7, H4, L10, SC, L6, L10, H1, L8, CS, L7, SC, L6, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L8, L9, L7, CS, L10, H4, L9, L8, CS, CS, H4, L6, H1, L8, H4, H2, CS, CS, H3, L6, L9, L10, H2, H1, H4, L8, L10, H3, L9, H4, L6, L7, H4, L9, H3, L10, CS, L6, L7, H2, L6, H3, L10, L6, H2, H4, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L9, CS, L6, L8, CS, L7, L6, L10, CS, CS, H4, H2, L8, H3, H4, CS, CS, L7, H1, L10, H2, H4, H3, SC, L9, H4, L6, SC, L9, L8, SC, L7, H2, L10, SC, L7, H2, L8, H4, SC, L6, L9, H4, L10, H3, L6, SC, L8, L6, H1, H4, L8, L10, L7, L6, L9, H3, },

    { CS, CS, CS, CS, CS, CS, L9, L6, H3, CS, L8, H4, L7, L6, L9, L10, H4, L9, CS, CS, L8, H4, L7, H2, H1, CS, CS, L8, L6, L10, H4, H3, L6, CS, L7, H4, L9, H3, L7, H2, L10, H4, L7, L8, H2, L9, H4, L6, H2, L10, H1, H3, L6, L7, L9, L10, },

    { CS, CS, CS, CS, CS, CS, H4, H2, L6, CS, H4, L8, H3, CS, CS, L7, H2, L9, H4, L7, SC, L10, L8, H4, L6, H3, L7, L9, CS, CS, L8, H4, H3, L8, SC, L10, H4, H3, L9, H1, L6, H2, L10, H4, L9, H1, L6, L7, L8, SC, L9, L10, CS, L6, L7, H2, L10, L7, },
};

// Set 7
const std::vector<std::vector<Symbol>> BBSet7 = {
    { CS, CS, CS, CS, CS, CS, L9, H4, L10, CS, L9, H3, L6, L8, L7, H1, L10, H2, L9, SC, H3, H4, CS, CS, H2, H4, L7, H3, H4, H2, CS, CS, L6, SC, L8, H4, CS, L9, L7, SC, L6, L9, H4, L7, H3, L8, SC, L10, L6, H2, L7, H4, L10, SC, L6, L10, H1, L8, L7, SC, L6, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L8, L9, L7, CS, L10, H4, L9, L8, CS, CS, H4, L6, H1, L8, H4, H2, CS, CS, H3, L6, L9, L10, H2, H1, H4, L8, L10, H3, L9, H4, L6, L7, H4, L9, H3, L10, L6, L7, H2, L6, H3, L10, CS, L6, H2, H4, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L9, CS, L6, L8, CS, L7, L6, L10, CS, CS, H4, H2, L8, H3, H4, CS, CS, L7, H1, L10, H2, H4, H3, L9, H4, L6, L9, L8, L7, H2, L10, L7, H2, L8, H4, L6, L9, H4, L10, L6, H3, L8, L6, H1, H4, L8, L10, L7, L6, L9, H3, },

    { CS, CS, CS, CS, CS, CS, L9, SC, L6, H3, CS, L8, H4, L7, SC, L6, L9, L10, H4, L9, CS, CS, L8, SC, H4, L7, CS, H2, H1, CS, CS, L8, SC, L6, L10, H4, H3, L6, SC, L7, H4, L9, H3, L7, H2, L10, H4, L7, SC, L8, H2, L9, H4, L6, H2, L10, H1, H3, L6, SC, L7, L9, L10, },

    { CS, CS, CS, CS, CS, CS, H4, H2, L6, CS, H4, L8, H3, CS, CS, L7, H2, L9, H4, L7, CS, L10, SC, L8, H4, L6, H3, L7, L9, CS, CS, L8, H4, H3, L8, L10, H4, H3, L9, H1, L6, H2, L10, H4, L9, H1, L6, L7, L8, SC, L9, L10, L6, L7, H2, L10, L7, },
};

// Set 8
const std::vector<std::vector<Symbol>> BBSet8 = {
    { CS, CS, CS, CS, CS, CS, L9, H4, L10, CS, L9, H3, L6, CS, L8, L7, H1, L10, H2, L9, H3, H4, CS, CS, H2, H4, L7, H3, H4, H2, CS, CS, L6, L8, H4, L9, L7, L6, L9, H4, L7, H3, L8, L10, L6, H2, L7, L10, H4, L6, L10, H1, L8, L7, L6, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L8, SC, L9, L7, CS, L10, H4, L9, SC, L8, CS, CS, H4, L6, H1, L8, H4, H2, CS, CS, H3, L6, SC, L9, L10, H2, H1, H4, L8, SC, L10, H3, L9, H4, L6, SC, L7, H4, L9, H3, L10, L6, SC, L7, H2, L6, H3, SC, L10, CS, L6, H2, H4, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L9, CS, L6, L8, CS, L7, L6, L10, CS, CS, H4, H2, L8, H3, H4, CS, CS, L7, H1, L10, H2, H4, H3, SC, L9, H4, L6, SC, L9, L8, SC, L7, H2, L10, SC, L7, H2, L8, H4, L6, L9, H4, L10, H3, L6, SC, L8, L6, H1, H4, L8, SC, L10, L7, L6, L9, H3, },

    { CS, CS, CS, CS, CS, CS, L9, L6, H3, CS, L8, H4, L7, L6, SC, L9, L10, H4, L9, CS, CS, L8, H4, L7, H2, H1, CS, CS, L8, L6, L10, H4, H3, L6, CS, L7, H4, L9, H3, L7, H2, L10, H4, L7, L8, SC, H2, L9, H4, L6, H2, L10, H1, H3, L6, L7, SC, L9, L10, },

    { CS, CS, CS, CS, CS, CS, H4, H2, L6, CS, H4, L8, H3, CS, CS, L7, H2, L9, H4, L7, L10, L8, H4, L6, H3, L7, L9, CS, CS, L8, H4, H3, L8, CS, L10, H4, H3, L9, H1, L6, H2, L10, H4, L9, H1, L6, L7, L8, L9, L10, L6, L7, H2, L10, L7, },
};

// Set 9
const std::vector<std::vector<Symbol>> BBSet9 = {
    { CS, CS, CS, CS, CS, CS, L9, H4, L10, CS, L9, H3, L6, L8, L7, H1, L10, H2, L9, H3, H4, CS, CS, H2, H4, L7, H3, H4, H2, CS, CS, L6, L8, H4, L9, CS, L7, L6, L9, H4, L7, H3, L8, L10, L6, H2, L7, L10, H4, L6, L10, H1, L8, L7, L6, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L8, L9, L7, CS, L10, H4, L9, SC, L8, CS, CS, H4, SC, L6, H1, L8, H4, H2, CS, CS, H3, L6, SC, L9, L10, H2, H1, H4, L8, SC, L10, H3, L9, H4, L6, CS, L7, H4, L9, H3, L10, L6, SC, L7, H2, L6, H3, L10, L6, H2, H4, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L9, CS, L6, SC, L8, CS, L7, L6, L10, CS, CS, H4, H2, L8, SC, H3, H4, CS, CS, L7, H1, L10, H2, H4, H3, SC, L9, H4, L6, SC, L9, L8, SC, L7, H2, L10, L7, H2, L8, H4, L6, L9, H4, L10, H3, L6, SC, L8, L6, H1, H4, L8, L10, L7, L6, L9, H3, },

    { CS, CS, CS, CS, CS, CS, L9, L6, H3, CS, L8, H4, L7, L6, L9, L10, H4, L9, CS, CS, L8, H4, L7, H2, H1, CS, CS, L8, L6, L10, H4, H3, L6, CS, L7, H4, L9, H3, L7, H2, L10, H4, L7, L8, H2, L9, H4, L6, H2, L10, H1, H3, L6, L7, L9, L10, },

    { CS, CS, CS, CS, CS, CS, H4, H2, L6, CS, H4, L8, H3, CS, CS, L7, H2, L9, H4, L7, SC, L10, L8, H4, L6, H3, L7, SC, L9, CS, CS, L8, H4, H3, L8, L10, H4, H3, L9, H1, L6, H2, L10, H4, L9, H1, L6, L7, SC, L8, L9, CS, L10, L6, L7, H2, L10, L7, },
};

// Set 10
const std::vector<std::vector<Symbol>> BBSet10 = {
    { CS, CS, CS, CS, CS, CS, L9, H4, L10, CS, L9, H3, L6, L8, L7, H1, L10, H2, L9, H3, H4, CS, CS, H2, H4, L7, H3, H4, H2, CS, CS, L6, L8, H4, L9, CS, L7, L6, L9, H4, L7, H3, L8, L10, L6, H2, L7, L10, H4, L6, L10, H1, L8, L7, L6, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L8, SC, L9, L7, CS, L10, H4, L9, SC, L8, CS, CS, H4, L6, H1, SC, L8, H4, H2, CS, CS, H3, L6, SC, L9, CS, L10, H2, H1, H4, L8, SC, L10, H3, L9, H4, L6, SC, L7, H4, L9, H3, L10, L6, SC, L7, H2, L6, H3, L10, L6, H2, H4, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L9, CS, L6, L8, CS, L7, L6, L10, CS, CS, H4, H2, L8, H3, H4, CS, CS, L7, H1, L10, H2, H4, H3, L9, H4, L6, L9, L8, L7, H2, L10, L7, H2, L8, H4, L6, L9, H4, L10, L6, H3, L8, L6, H1, H4, L8, L10, L7, L6, L9, H3, },

    { CS, CS, CS, CS, CS, CS, L9, L6, H3, CS, L8, H4, L7, SC, L6, L9, L10, H4, L9, CS, CS, L8, SC, H4, L7, H2, H1, CS, CS, L8, SC, L6, CS, L10, H4, H3, L6, SC, L7, H4, L9, H3, L7, H2, L10, H4, L7, SC, L8, H2, L9, H4, L6, H2, L10, H1, H3, L6, SC, L7, L9, L10, },

    { CS, CS, CS, CS, CS, CS, H4, H2, L6, CS, H4, L8, H3, CS, CS, L7, H2, L9, H4, SC, L7, L10, L8, H4, L6, H3, L7, L9, CS, CS, L8, H4, H3, L8, CS, L10, H4, H3, L9, H1, L6, H2, L10, H4, L9, H1, L6, L7, L8, SC, L9, L10, L6, L7, H2, L10, L7, },
};

// Set 11
const std::vector<std::vector<Symbol>> BBSet11 = {
    { CS, CS, CS, L10, H2, L9, H1, L8, H4, L7, H3, L6, H1, L7, H2, H3, L6, H4, },
    { CS, CS, CS, L10, H1, L9, H2, L8, H3, L7, H4, L6, H1, L7, L8, H3, H2, H4, L6, },
    { CS, CS, CS, L9, H2, L10, H1, L7, H4, L8, H3, L6, H1, L8, L7, H2, H3, L6, H4, },
    { CS, CS, CS, L9, H1, L10, H2, L7, H4, L8, H3, L6, H1, L7, L8, H3, H2, H4, L6, },
    { CS, CS, CS, H3, L10, L6, H2, H4, L8, H3, L7, H4, H1, L9, H2, L7, H3, H4, L8, H1, L6, H3, },
};



//
// ------------------------- Super Boost Reel Sets -------------------------
//

// Set 1
const std::vector<std::vector<Symbol>> SBSet1 = {
    { CS, CS, CS, CS, CS, CS, L9, H4, L10, CS, L9, H3, L6, SC, L8, L7, H1, L10, H2, L9, H3, H4, CS, CS, H2, H4, L7, H3, H4, H2, CS, CS, L6, L8, H4, L9, CS, L7, L6, L9, H4, L7, H3, L8, L10, L6, H2, L7, H4, L10, SC, L6, L10, H1, L8, L7, L6, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L8, L9, L7, CS, L10, H4, L9, L8, CS, CS, H4, L6, H1, L8, H4, H2, CS, CS, H3, L6, L9, L10, H2, H1, H4, L8, L10, H3, L9, H4, L6, L7, H4, L9, H3, L10, SC, L6, L7, H2, L6, H3, L10, CS, L6, H2, H4, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L9, CS, L6, L8, CS, L7, L6, L10, CS, CS, H4, H2, L8, H3, H4, CS, CS, L7, H1, L10, H2, H4, H3, L9, H4, L6, L9, L8, L7, H2, L10, L7, H2, L8, H4, L6, L9, H4, L10, H3, L6, SC, L8, L6, H1, H4, L8, L10, L7, L6, L9, H3, },

    { CS, CS, CS, CS, CS, CS, L9, L6, H3, CS, L8, H4, L7, L6, L9, SC, L10, H4, L9, CS, CS, L8, H4, L7, H2, H1, CS, CS, L8, L6, SC, L10, H4, H3, L6, CS, L7, H4, L9, H3, L7, H2, L10, H4, L7, L8, H2, L9, H4, L6, H2, L10, H1, H3, L6, L7, L9, L10, },

    { CS, CS, CS, CS, CS, CS, H4, H2, L6, CS, H4, L8, H3, CS, CS, L7, H2, H4, L7, L9, L10, L8, H4, L6, H3, L7, L9, CS, CS, L8, H4, H3, L8, SC, L10, H4, H3, L9, H1, L6, H2, L10, H4, L9, H1, L6, L7, L8, L9, SC, L10, CS, L6, L7, H2, L10, L7, },
};

// Set 2
const std::vector<std::vector<Symbol>> SBSet2 = {
    { CS, CS, CS, CS, CS, CS, L9, H4, L10, CS, L9, H3, L6, SC, L8, L7, H1, L10, H2, L9, H3, H4, SC, CS, CS, H2, H4, L7, H3, H4, H2, CS, CS, L6, SC, L8, H4, CS, L9, L7, SC, L6, L9, H4, L7, H3, SC, L8, L10, L6, H2, L7, H4, L10, SC, L6, L10, H1, L8, L7, SC, L6, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L8, L9, L7, CS, L10, H4, L9, SC, L8, CS, CS, H4, L6, SC, H1, L8, H4, H2, CS, CS, H3, L6, SC, L9, L10, H2, H1, H4, L8, SC, L10, H3, L9, H4, L6, CS, L7, SC, H4, L9, H3, L10, L6, SC, L7, H2, L6, H3, L10, L6, H2, H4, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L9, CS, L6, SC, L8, CS, L7, L6, L10, CS, CS, H4, H2, SC, L8, H3, H4, CS, CS, L7, H1, L10, H2, H4, H3, L9, H4, L6, L9, L8, L7, H2, L10, L7, H2, L8, SC, H4, L6, L9, H4, L10, H3, L6, SC, L8, L6, H1, H4, L8, L10, L7, L6, L9, H3, },

    { CS, CS, CS, CS, CS, CS, L9, L6, H3, CS, L8, H4, L7, L6, L9, L10, H4, L9, CS, CS, L8, H4, L7, H2, H1, CS, CS, L8, L6, L10, H4, H3, L6, L7, H4, L9, H3, L7, H2, L10, H4, L7, L8, H2, L9, H4, L6, H2, L10, H1, H3, L6, CS, L7, L9, L10, },

    { CS, CS, CS, CS, CS, CS, H4, H2, L6, CS, H4, L8, H3, CS, CS, L7, H2, L7, H4, L9, L10, L8, H4, L6, H3, L7, L9, CS, CS, L8, H4, H3, L8, L10, H4, H3, L9, H1, L6, H2, L10, H4, L9, H1, L6, L7, L8, CS, L9, L10, L6, L7, H2, L10, L7, },
};

// Set 3
const std::vector<std::vector<Symbol>> SBSet3 = {
    { CS, CS, CS, CS, CS, CS, L9, H4, L10, CS, L9, H3, L6, L8, L7, H1, L10, H2, L9, H3, H4, CS, CS, H2, H4, L7, SC, H3, H4, H2, CS, CS, L6, SC, L8, H4, CS, L9, L7, SC, L6, L9, H4, L7, H3, SC, L8, L10, L6, H2, L7, H4, L10, SC, L6, L10, H1, L8, L7, SC, L6, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L8, SC, L9, L7, CS, L10, H4, L9, SC, L8, CS, CS, H4, L6, H1, L8, H4, H2, CS, CS, H3, L6, SC, L9, L10, H2, SC, H1, H4, L8, SC, L10, H3, L9, H4, L6, CS, L7, SC, H4, L9, H3, L10, L6, SC, L7, H2, L6, H3, L10, L6, H2, H4, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L9, CS, L6, L8, CS, L7, L6, L10, CS, CS, H4, H2, L8, H3, H4, CS, CS, L7, H1, L10, H2, H4, H3, L9, H4, L6, L9, L8, L7, H2, L10, L7, H2, L8, H4, L6, L9, H4, L10, L6, H3, L8, L6, H1, H4, L8, L10, L7, L6, L9, H3, },

    { CS, CS, CS, CS, CS, CS, L9, L6, H3, CS, L8, H4, L7, L6, SC, L9, L10, H4, L9, CS, CS, L8, SC, H4, L7, H2, H1, CS, CS, L8, L6, L10, H4, H3, L6, SC, L7, H4, L9, H3, L7, H2, L10, SC, H4, L7, L8, H2, L9, H4, L6, H2, L10, H1, H3, L6, CS, L7, L9, L10, },

    { CS, CS, CS, CS, CS, CS, H4, H2, L6, CS, H4, L8, H3, CS, CS, L7, H2, L9, H4, L7, L10, L8, H4, L6, H3, L7, L9, CS, CS, L8, H4, H3, L8, CS, L10, H4, H3, L9, H1, L6, H2, L10, H4, L9, H1, L6, L7, L8, L9, L10, L6, L7, H2, L10, L7, },
};

// Set 4
const std::vector<std::vector<Symbol>> SBSet4 = {
    { CS, CS, CS, CS, CS, CS, L9, H4, L10, CS, L9, H3, L6, L8, L7, H1, L10, H2, L9, H3, H4, SC, CS, CS, H2, H4, L7, SC, H3, H4, H2, CS, CS, L6, SC, L8, H4, CS, L9, L7, SC, L6, L9, H4, L7, H3, SC, L8, L10, L6, H2, L7, H4, L10, SC, L6, L10, H1, L8, L7, SC, L6, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L8, L9, L7, CS, L10, H4, L9, SC, L8, CS, CS, H4, L6, SC, H1, L8, H4, H2, CS, CS, H3, L6, SC, L9, L10, H2, H1, H4, L8, SC, L10, H3, L9, H4, L6, L7, SC, H4, L9, H3, L10, L6, SC, L7, H2, L6, H3, L10, SC, CS, L6, H2, H4, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L9, CS, L6, L8, CS, L7, L6, L10, CS, CS, H4, H2, L8, H3, H4, CS, CS, L7, H1, L10, H2, H4, H3, L9, H4, L6, L9, L8, L7, H2, L10, L7, H2, L8, H4, L6, L9, H4, L10, L6, H3, L8, L6, H1, H4, L8, L10, L7, L6, L9, H3, },

    { CS, CS, CS, CS, CS, CS, L9, L6, H3, CS, L8, H4, L7, L6, L9, L10, H4, L9, CS, CS, L8, H4, L7, H2, H1, CS, CS, L8, L6, L10, H4, H3, L6, CS, L7, H4, L9, H3, L7, H2, L10, H4, L7, L8, H2, L9, H4, L6, H2, L10, H1, H3, L6, L7, L9, L10, },

    { CS, CS, CS, CS, CS, CS, H4, H2, L6, CS, H4, L8, H3, CS, CS, L7, H2, L9, H4, L7, L10, SC, L8, H4, L6, H3, L7, L9, CS, CS, L8, H4, H3, L8, CS, L10, H4, H3, L9, H1, SC, L6, H2, L10, H4, L9, H1, L6, SC, L7, L8, L9, SC, L10, L6, L7, H2, L10, L7, },
};

// Set 5
const std::vector<std::vector<Symbol>> SBSet5 = {
    { CS, CS, CS, CS, CS, CS, L9, H4, L10, CS, L9, H3, L6, L8, L7, H1, L10, H2, SC, L9, H3, H4, CS, CS, H2, SC, H4, L7, H3, H4, H2, CS, CS, L6, SC, L8, H4, SC, L9, L7, SC, L6, L9, H4, L7, H3, L8, SC, L10, L6, H2, L7, H4, L10, SC, L6, L10, H1, L8, CS, L7, SC, L6, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L8, L9, L7, CS, L10, H4, L9, L8, CS, CS, H4, L6, H1, L8, H4, H2, CS, CS, H3, L6, L9, L10, H2, H1, H4, L8, L10, H3, L9, H4, L6, CS, L7, H4, L9, H3, L10, L6, L7, H2, L6, H3, L10, L6, H2, H4, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L9, CS, L6, SC, L8, CS, L7, L6, L10, SC, CS, CS, H4, H2, L8, H3, SC, H4, CS, CS, L7, H1, L10, H2, H4, H3, SC, L9, H4, L6, SC, L9, L8, SC, L7, H2, L10, SC, L7, H2, L8, H4, L6, L9, H4, L10, H3, L6, SC, L8, L6, H1, H4, L8, L10, L7, L6, L9, H3, },

    { CS, CS, CS, CS, CS, CS, L9, L6, H3, CS, L8, H4, L7, L6, SC, L9, L10, H4, L9, CS, CS, L8, H4, L7, H2, H1, CS, CS, L8, SC, L6, L10, H4, H3, L6, CS, L7, H4, L9, H3, L7, H2, L10, H4, L7, L8, H2, L9, H4, L6, H2, L10, H1, H3, L6, SC, L7, L9, L10, },

    { CS, CS, CS, CS, CS, CS, H4, H2, L6, CS, H4, L8, H3, CS, CS, L7, H2, L9, H4, L7, L10, L8, H4, L6, H3, L7, L9, CS, CS, L8, H4, H3, L8, L10, H4, H3, L9, H1, L6, H2, L10, H4, L9, H1, L6, L7, L8, CS, L9, L10, L6, L7, H2, L10, L7, },
};

// Set 6
const std::vector<std::vector<Symbol>> SBSet6 = {
    { CS, CS, CS, CS, CS, CS, L9, H4, L10, CS, L9, H3, L6, SC, L8, L7, H1, L10, H2, L9, H3, H4, SC, CS, CS, H2, H4, L7, SC, H3, H4, H2, CS, CS, L6, SC, L8, H4, L9, L7, SC, L6, L9, H4, L7, H3, L8, SC, L10, L6, H2, L7, H4, L10, SC, L6, L10, H1, L8, CS, L7, SC, L6, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L8, L9, L7, CS, L10, H4, L9, L8, CS, CS, H4, L6, H1, L8, H4, H2, CS, CS, H3, L6, L9, L10, H2, H1, H4, L8, L10, H3, L9, H4, L6, L7, H4, L9, H3, L10, CS, L6, L7, H2, L6, H3, L10, L6, H2, H4, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L9, CS, L6, L8, CS, L7, L6, L10, CS, CS, H4, H2, L8, H3, SC, H4, CS, CS, L7, H1, L10, H2, H4, H3, SC, L9, H4, L6, SC, L9, L8, SC, L7, H2, L10, SC, L7, H2, L8, H4, SC, L6, L9, H4, L10, H3, L6, SC, L8, L6, H1, H4, L8, L10, L7, L6, L9, H3, },

    { CS, CS, CS, CS, CS, CS, L9, L6, H3, CS, L8, H4, L7, L6, L9, L10, H4, L9, CS, CS, L8, H4, L7, H2, H1, CS, CS, L8, L6, L10, H4, H3, L6, CS, L7, H4, L9, H3, L7, H2, L10, H4, L7, L8, H2, L9, H4, L6, H2, L10, H1, H3, L6, L7, L9, L10, },

    { CS, CS, CS, CS, CS, CS, H4, H2, L6, CS, H4, L8, H3, CS, CS, L7, H2, L9, H4, L7, SC, L10, L8, H4, L6, H3, L7, L9, CS, CS, L8, H4, H3, L8, SC, L10, H4, H3, L9, H1, L6, H2, SC, L10, H4, L9, H1, L6, L7, L8, SC, L9, L10, CS, L6, L7, H2, L10, L7, },
};

// Set 7
const std::vector<std::vector<Symbol>> SBSet7 = {
    { CS, CS, CS, CS, CS, CS, L9, H4, L10, CS, L9, H3, L6, L8, L7, H1, L10, H2, L9, SC, H3, H4, CS, CS, H2, H4, L7, SC, H3, H4, H2, CS, CS, L6, SC, L8, H4, CS, L9, L7, SC, L6, L9, H4, L7, H3, L8, SC, L10, L6, H2, L7, H4, L10, SC, L6, L10, H1, L8, L7, SC, L6, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L8, L9, L7, CS, L10, H4, L9, L8, CS, CS, H4, L6, H1, L8, H4, H2, CS, CS, H3, L6, L9, L10, H2, H1, H4, L8, L10, H3, L9, H4, L6, L7, H4, L9, H3, L10, L6, L7, H2, L6, H3, L10, CS, L6, H2, H4, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L9, CS, L6, L8, CS, L7, L6, L10, CS, CS, H4, H2, L8, H3, H4, CS, CS, L7, H1, L10, H2, H4, H3, L9, H4, L6, L9, L8, L7, H2, L10, L7, H2, L8, H4, L6, L9, H4, L10, L6, H3, L8, L6, H1, H4, L8, L10, L7, L6, L9, H3, },

    { CS, CS, CS, CS, CS, CS, L9, L6, H3, CS, L8, H4, L7, SC, L6, L9, L10, H4, L9, CS, CS, L8, SC, H4, L7, CS, H2, H1, CS, CS, L8, SC, L6, L10, H4, H3, L6, SC, L7, H4, L9, H3, L7, H2, L10, H4, L7, SC, L8, H2, L9, H4, L6, SC, H2, L10, H1, H3, L6, SC, L7, L9, L10, },

    { CS, CS, CS, CS, CS, CS, H4, H2, L6, CS, H4, L8, H3, CS, CS, L7, H2, L9, H4, L7, CS, L10, SC, L8, H4, L6, H3, L7, L9, CS, CS, L8, H4, H3, L8, L10, H4, H3, SC, L9, H1, L6, H2, L10, H4, L9, H1, L6, L7, L8, SC, L9, L10, L6, L7, H2, SC, L10, L7, },
};

// Set 8
const std::vector<std::vector<Symbol>> SBSet8 = {
    { CS, CS, CS, CS, CS, CS, L9, H4, L10, CS, L9, H3, L6, CS, L8, L7, H1, L10, H2, L9, H3, H4, CS, CS, H2, H4, L7, H3, H4, H2, CS, CS, L6, L8, H4, L9, L7, L6, L9, H4, L7, H3, L8, L10, L6, H2, L7, L10, H4, L6, L10, H1, L8, L7, L6, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L8, SC, L9, L7, CS, L10, H4, L9, SC, L8, CS, CS, H4, L6, H1, SC, L8, H4, H2, CS, CS, H3, L6, SC, L9, L10, H2, H1, H4, L8, SC, L10, H3, L9, H4, L6, SC, L7, H4, L9, H3, L10, L6, SC, L7, H2, L6, H3, SC, L10, CS, L6, H2, H4, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L9, CS, L6, L8, CS, L7, L6, L10, CS, CS, H4, H2, L8, H3, SC, H4, CS, CS, L7, H1, L10, H2, H4, H3, SC, L9, H4, L6, SC, L9, L8, SC, L7, H2, L10, SC, L7, H2, L8, H4, L6, L9, H4, L10, H3, L6, SC, L8, L6, H1, H4, L8, SC, L10, L7, L6, L9, H3, },

    { CS, CS, CS, CS, CS, CS, L9, L6, H3, CS, L8, H4, L7, L6, SC, L9, L10, H4, L9, CS, CS, L8, SC, H4, L7, H2, H1, CS, CS, L8, L6, L10, H4, H3, L6, CS, L7, H4, L9, H3, L7, H2, L10, H4, L7, L8, SC, H2, L9, H4, L6, H2, L10, H1, H3, L6, L7, SC, L9, L10, },

    { CS, CS, CS, CS, CS, CS, H4, H2, L6, CS, H4, L8, H3, CS, CS, L7, H2, L9, H4, L7, L10, L8, H4, L6, H3, L7, L9, CS, CS, L8, H4, H3, L8, CS, L10, H4, H3, L9, H1, L6, H2, L10, H4, L9, H1, L6, L7, L8, L9, L10, L6, L7, H2, L10, L7, },
};

// Set 9
const std::vector<std::vector<Symbol>> SBSet9 = {
    { CS, CS, CS, CS, CS, CS, L9, H4, L10, CS, L9, H3, L6, L8, L7, H1, L10, H2, L9, H3, H4, CS, CS, H2, H4, L7, H3, H4, H2, CS, CS, L6, L8, H4, L9, CS, L7, L6, L9, H4, L7, H3, L8, L10, L6, H2, L7, L10, H4, L6, L10, H1, L8, L7, L6, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L8, L9, L7, CS, L10, H4, L9, SC, L8, CS, CS, H4, SC, L6, H1, L8, H4, H2, CS, CS, H3, L6, SC, L9, L10, H2, H1, H4, L8, SC, L10, H3, L9, H4, L6, CS, SC, L7, H4, L9, SC, H3, L10, L6, SC, L7, H2, L6, H3, L10, L6, H2, H4, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L9, CS, L6, L8, CS, L7, L6, L10, CS, CS, H4, H2, L8, SC, H3, H4, CS, CS, L7, H1, L10, H2, H4, H3, SC, L9, H4, L6, SC, L9, L8, SC, L7, H2, L10, L7, H2, L8, SC, H4, L6, L9, H4, L10, H3, L6, SC, L8, L6, H1, H4, L8, SC, L10, L7, L6, L9, H3, },

    { CS, CS, CS, CS, CS, CS, L9, L6, H3, CS, L8, H4, L7, L6, L9, L10, H4, L9, CS, CS, L8, H4, L7, H2, H1, CS, CS, L8, L6, L10, H4, H3, L6, CS, L7, H4, L9, H3, L7, H2, L10, H4, L7, L8, H2, L9, H4, L6, H2, L10, H1, H3, L6, L7, L9, L10, },

    { CS, CS, CS, CS, CS, CS, H4, H2, L6, CS, H4, L8, H3, CS, CS, L7, H2, L9, H4, L7, SC, L10, L8, H4, L6, H3, L7, SC, L9, CS, CS, L8, H4, H3, L8, L10, H4, H3, SC, L9, H1, L6, H2, L10, H4, L9, H1, L6, L7, SC, L8, L9, CS, L10, L6, L7, H2, L10, L7, },
};

// Set 10
const std::vector<std::vector<Symbol>> SBSet10 = {
    { CS, CS, CS, CS, CS, CS, L9, H4, L10, CS, L9, H3, L6, L8, L7, H1, L10, H2, L9, H3, H4, CS, CS, H2, H4, L7, H3, H4, H2, CS, CS, L6, L8, H4, L9, CS, L7, L6, L9, H4, L7, H3, L8, L10, L6, H2, L7, L10, H4, L6, L10, H1, L8, L7, L6, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L8, L9, L7, CS, L10, H4, L9, SC, L8, CS, CS, H4, L6, H1, SC, L8, H4, H2, CS, CS, H3, L6, SC, L9, CS, L10, H2, H1, H4, L8, SC, L10, H3, L9, H4, L6, SC, L7, H4, L9, H3, L10, L6, SC, L7, H2, L6, H3, SC, L10, L6, H2, H4, L8, },

    { CS, CS, CS, CS, CS, CS, L7, L9, CS, L6, L8, CS, L7, L6, L10, CS, CS, H4, H2, L8, H3, H4, CS, CS, L7, H1, L10, H2, H4, H3, L9, H4, L6, L9, L8, L7, H2, L10, L7, H2, L8, H4, L6, L9, H4, L10, L6, H3, L8, L6, H1, H4, L8, L10, L7, L6, L9, H3, },

    { CS, CS, CS, CS, CS, CS, L9, L6, H3, CS, L8, H4, L7, SC, L6, L9, L10, H4, L9, CS, CS, L8, SC, H4, L7, H2, H1, CS, CS, L8, SC, L6, CS, L10, H4, H3, L6, SC, L7, H4, L9, H3, SC, L7, H2, L10, H4, L7, SC, L8, H2, L9, H4, L6, H2, L10, H1, H3, L6, SC, L7, L9, L10, },

    { CS, CS, CS, CS, CS, CS, H4, H2, L6, SC, CS, H4, L8, H3, CS, CS, L7, H2, L9, H4, SC, L7, L10, L8, H4, L6, H3, L7, L9, CS, CS, L8, H4, H3, L8, CS, L10, SC, H4, H3, L9, H1, L6, H2, L10, H4, L9, H1, L6, L7, L8, SC, L9, L10, L6, L7, H2, L10, L7, },
};

// Set 11
const std::vector<std::vector<Symbol>> SBSet11 = {
    { CS, CS, CS, L10, H2, L9, H1, L8, H4, L7, H3, L6, H1, L7, H2, H3, L6, H4, },
    { CS, CS, CS, L10, H1, L9, H2, L8, H3, L7, H4, L6, H1, L7, L8, H3, H2, H4, L6, },
    { CS, CS, CS, L9, H2, L10, H1, L7, H4, L8, H3, L6, H1, L8, L7, H2, H3, L6, H4, },
    { CS, CS, CS, L9, H1, L10, H2, L7, H4, L8, H3, L6, H1, L7, L8, H3, H2, H4, L6, },
    { CS, CS, CS, H3, L10, L6, H2, H4, L8, H3, L7, H4, H1, L9, H2, L7, H3, H4, L8, H1, L6, H3, },
};

//
// ------------------------- FS Reel Sets -------------------------
//

// Set 1
const std::vector<std::vector<Symbol>> FSSet1 = {
    { CR, CR, H4, L6, L8, H4, H3, L10, CR, CR, L8, H1, L10, L6, L8, L9, L6, L8, L10, H2, L7, L8, L10, L6, H2, L10, L8, L6, L9, L10, L6, L7, L10, L6, L8, },
    { CR, CR, CR, H3, L7, CR, H4, H1, L7, L9, L10, H2, L7, CR, CR, L9, L6, L7, L9, L8, H1, L7, CR, CR, L9, H3, L7, L9, L6, L10, L9, L7, L8, L9, },
    { CR, CR, CR, H4, H2, H3, CR, L6, H2, H4, H3, CR, L7, H1, H4, L9, L10, H3, H2, H4, L10, L6, H2, L8, L7, H3, L8, L9, },
    { CR, CR, H4, H2, L6, CR, L10, H4, L8, H1, H3, H4, L7, H1, H3, H2, CR, L9, H3, },
    { CR, CR, H2, H3, CR, H4, L6, L10, CR, H4, H3, H1, H2, CR, L9, L7, L8, L6, L10, L9, L7, L8, },
};

// Set 2
const std::vector<std::vector<Symbol>> FSSet2 = {
    { CR, CR, CR, H4, L6, L8, H4, H3, L10, CR, CR, L8, H1, L10, L6, L8, L9, L6, L8, L10, H2, L7, L8, L10, L6, H2, L10, L8, L6, L9, L10, L6, L7, L10, L6, L8, },
    { CR, CR, CR, H3, L7, CR, CR, H4, H1, L7, L9, L10, H2, L7, CR, CR, L9, L6, L7, L9, L8, H1, L7, CR, CR, L9, H3, L7, L9, L6, L10, L9, L7, L8, L9, },
    { CR, CR, CR, H4, H2, H3, CR, L6, H2, H4, H3, CR, CR, L7, H1, H4, L9, L10, H3, H2, H4, L10, L6, H2, L8, L7, H3, L8, L9, },
    { CR, CR, CR, H4, H2, L6, CR, L10, H4, L8, H1, H3, H4, L7, H1, H3, H2, CR, CR, L9, H3, },
    { CR, CR, CR, H2, H3, CR, H4, L6, L10, CR, CR, H4, H3, H1, H2, CR, L9, L7, L8, L6, L10, L9, L7, L8, },
};

const std::vector<std::vector<std::vector<Symbol>>> RG_Sets_ALL = {
    RGSet1, RGSet2, RGSet3, RGSet4,
    RGSet5, RGSet6, RGSet7, RGSet8,
    RGSet9, RGSet10, RGSet11,
};

const std::vector<std::vector<std::vector<Symbol>>> BB_Sets_ALL = {
    BBSet1, BBSet2, BBSet3, BBSet4,
    BBSet5, BBSet6, BBSet7, BBSet8,
    BBSet9, BBSet10, BBSet11
};

const std::vector<std::vector<std::vector<Symbol>>> SB_Sets_ALL = {
    SBSet1, SBSet2, SBSet3, SBSet4,
    SBSet5, SBSet6, SBSet7, SBSet8,
    SBSet9, SBSet10, SBSet11,
};
