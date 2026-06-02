#pragma once

#include <algorithm>
#include <vector>
#include <functional>
#include <iostream>
#include "./constants.h"
#include "./reel.h"
#include "rng.h"


using Reels_V = std::vector<Reel*>;

//
// Now define sets
//
class Set {
    public:
        Set(const int id,
            const std::string name,
            const Reels_V reels
            )
            : id(id), name(name), reels(reels)
        {}

        const int id;
        const std::string name;
        inline Reels_V& getReels() { return reels; }
    private:
        Reels_V reels;
};

//
// Define set of reelsets
//
class SetOfReelSets {
    public:
        SetOfReelSets(
            std::vector<Set*> sets,
            const std::vector<int> weights,
            RngLib& rng
            )
            :sets(sets), weights(weights), rng(rng)
        {};
        std::vector<Set*> sets;
        const std::vector<int> weights;
        RngLib& rng;

        const Set* getRandomSet () const;
        const Set* getRandomSet(const double prob) const;
        const Set* getRandomSet(const std::vector<int>& weights) const;
};

enum ReelSetsEnums {
    REELSET_RG = 0,
    REELSET_BB,
    REELSET_SB,
    REELSET_FS,
    REELSET_FS_WINNER,
    REELSET_INVALID,
};

// Just declaring it here to quieten the compiler
class Round;

constexpr inline int FORCED_REEL_COUNT = 3;
constexpr inline int FORCED_REEL_STOPS_SET_COUNT = 5;

inline const std::vector<int> Reel_Set_Default_Weights = {
    1, 1, 1, 1,
    1, 1, 1, 1,
    1, 1, 1
};

inline const std::vector<int> Reel_Set_Weight_SetA = {
    1, 1, 1, 1,
    1, 1, 1, 1,
    1, 1, 7
};

inline const std::vector<int> Reel_Set_Weight_SetB = {
    1, 1, 1, 1,
    1, 1, 1, 1,
    1, 1, 6 };

inline const std::array<std::array<int, FORCED_REEL_COUNT>, REEL_COUNT> FORCED_REEL_STOPS_WITHOUT_MORE_COINS = {{
    { 3, 15, 5 },
    { 18, 7, 12 },
    { 4, 19, 23 },
    { 14, 8, 14 },
    { 5, 8, 14 },
}};

inline const std::array<std::array<int, FORCED_REEL_COUNT>, REEL_COUNT> FORCED_REEL_STOPS_WITH_MORE_COINS = {{
    { 4, 16, 5 },
    { 19, 8, 13 },
    { 5, 20, 24 },
    { 15, 9, 15 },
    { 6, 9 , 15 },
}};

class ReelSet {
    public:
        ReelSet (RngLib& rng);
        virtual void select (enum ReelSetsEnums set, const bool baseSpin, const bool forced);
        virtual void select (enum ReelSetsEnums set, const bool baseSpin, const bool forced, const double prob);
        virtual void select (enum ReelSetsEnums set, const bool baseSpin, const bool forced, const std::vector<int>& weights);

        virtual std::array<int, REEL_COUNT> spin (bool baseSpin, int mode, bool reSpin, bool forced = false, bool moreCoins = false, int fsStopIdx = -1);
        virtual inline void setRoundRef(Round* round) { this->round = round; }
        std::vector<int> getStops ();

        inline Set* getSelected() const { return selected; }
        inline const SetOfReelSets* getRGSet() const { return RG; }
        inline const SetOfReelSets* getBBSet() const { return BB; }
        inline const SetOfReelSets* getSBSet() const { return SB; }
        inline const SetOfReelSets* getFSSet() const { return FS; }

        void selectReelSetByName (enum ReelSetsEnums set, std::string& reelSetName);
        void setStops (std::array<int, REEL_COUNT>& stopsS);

    protected:
        RngLib& rng;
        Round* round;
        Set* selected;
        const SetOfReelSets *RG, *BB, *SB, *FS, *FS_winner;
        const SetOfReelSets* getSetOfSets (ReelSetsEnums x);
        const SetOfReelSets* getSetOfSets(ReelSetsEnums x, int reelSetIdx);

    private:
        void printState (const char * message);
};

