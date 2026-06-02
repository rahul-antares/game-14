#include "./reelset.h"
#include "../utils/log.h"
#include "../utils/error.h"
#include "reel.h"
#include "rng.h"

ReelSet::ReelSet(RngLib& rng) : rng(rng) {
    auto makeSet = [&](int id, const std::string& name, const std::vector<Reel_V>& reelSet) {
        Reels_V reels;
        for (size_t i = 0; i < reelSet.size(); ++i) {
            reels.push_back(new Reel(i, reelSet[i], rng));
        }

        return new Set(id, name, reels);
    };

    std::vector<Set*> rg_sets;
    for (int i = 0; i < 11; ++i)
        rg_sets.push_back(makeSet(i, "RG_reelSet" + std::to_string(i+1), RG_Sets_ALL[i]));

    std::vector<Set*> bb_sets;
    for (int i = 0; i < 11; ++i)
        bb_sets.push_back (makeSet ( i, "BB_reelSet" + std::to_string(i+1), BB_Sets_ALL[i]));

    std::vector<Set*> sb_sets;
    for (int i = 0; i < 11; ++i)
        sb_sets.push_back (makeSet ( i, "SB_reelSet" + std::to_string(i+1), SB_Sets_ALL[i]));

    RG        = new SetOfReelSets(rg_sets, Reel_Set_Default_Weights, rng);
    BB        = new SetOfReelSets(bb_sets, Reel_Set_Default_Weights, rng);
    SB        = new SetOfReelSets(sb_sets, Reel_Set_Default_Weights, rng);

    FS        = new SetOfReelSets ({ makeSet (0, "FS_reelSet1", FSSet1 )}, { 100 }, rng );
    FS_winner = new SetOfReelSets ({ makeSet (0, "FS_reelSet2", FSSet2 )}, { 100 }, rng );
}

void ReelSet::setStops (std::array<int, REEL_COUNT>& stopsS) {
    Reels_V& reels = selected->getReels();

    for (unsigned int i = 0; i < reels.size(); i++) {
        Reel* reel = reels.at(i);
        reel->setStop(stopsS[i]);
    }
}

std::vector<int> ReelSet::getStops () {
    Reels_V& reels = selected->getReels();
    std::vector<int> stops(REEL_COUNT);

    for (unsigned int i = 0; i < reels.size(); i++) {
        Reel* reel = reels.at(i);
        stops[i] = reel->stopPos();
    }

    return stops;
}

const Set* SetOfReelSets::getRandomSet(const double prob) const {
    if (sets.size() != 2)
        throw_error("Total Reel Sets available = ", sets.size(), ", but have to choose from 2");

    const double randProb = rng.getDouble();
    int index             = (randProb < prob ? 0 : 1);

    return sets[index];
}

const Set* SetOfReelSets::getRandomSet() const {
    int index = rng.getWeightedIndex(weights.data(), weights.size());
    return sets[index];
}

const Set* SetOfReelSets::getRandomSet(const std::vector<int>& rsWeights) const {
    if (rsWeights.size() != this->sets.size())
        throw_error ("size mismatch between reels and the probs, weights-size = ", rsWeights.size(), ", #reels = ", this->sets.size());

    int index = rng.getWeightedIndex(rsWeights.data(), rsWeights.size());
    return sets[index];
}

void ReelSet::selectReelSetByName (enum ReelSetsEnums set, std::string& reelSetName) {
    const SetOfReelSets* sets = getSetOfSets (set);
    bool selectedByName       = false;

    for (auto &set: sets->sets) {
        if (set->name == reelSetName) {
            this->selected = (Set*) set;
            selectedByName = true;
            break;
        }
    }

    if (!selectedByName)
        throw_error("didn't select a reelset with name = ", reelSetName, ", this shouldn't happen!");

    LOG("[reelSet] selected reel set \"%s\"\n", this->selected->name.c_str());
}

void ReelSet::select (enum ReelSetsEnums set, const bool baseSpin, const bool forced, const double prob) {
    const SetOfReelSets* sets = getSetOfSets(set);
    const Set* selectedSet    = sets->getRandomSet(prob);
    this->selected            = (Set*) selectedSet;

    LOG("[reelSet] selected reel set \"%s\"\n", this->selected->name.c_str());
}

void ReelSet::select (enum ReelSetsEnums set, const bool baseSpin, const bool forced, const std::vector<int>& weights) {
    const SetOfReelSets* sets = getSetOfSets (set);
    const Set* selectedSet    = sets->getRandomSet(weights);
    this->selected            = (Set*) selectedSet;

    LOG("[reelSet] selected reel set \"%s\"\n", this->selected->name.c_str());
}

void ReelSet::select (enum ReelSetsEnums set, const bool baseSpin, const bool forced) {
    const SetOfReelSets* sets = getSetOfSets(set);
    const Set* selectedSet    = sets->getRandomSet();
    this->selected            = (Set*) selectedSet;

    LOG("[reelSet] selected reel set \"%s\"\n", this->selected->name.c_str());
}

std::array<int, REEL_COUNT> ReelSet::spin (bool baseSpin, int mode, bool reSpin, bool forced, bool moreCoins, int fsStopIdx) {
    Reels_V& reels = selected->getReels();
    std::array<int, REEL_COUNT> stops;

    const auto& forcedReelStopsIdx = rng.getScaled (FORCED_REEL_STOPS_SET_COUNT);
    const auto& forcedReelStops = (moreCoins ? FORCED_REEL_STOPS_WITH_MORE_COINS : FORCED_REEL_STOPS_WITHOUT_MORE_COINS)[forcedReelStopsIdx];

    for (unsigned int i = 0; i < REEL_COUNT; i++) {
        Reel* reel = reels.at(i);
        stops[i] = reel->spin();

        if (i < FORCED_REEL_COUNT && forced)
            stops[i] = forcedReelStops [i];
    }

    return stops;
}

const SetOfReelSets* ReelSet::getSetOfSets (ReelSetsEnums x) {
    switch (x) {
        case REELSET_RG        : return RG;
        case REELSET_BB        : return BB;
        case REELSET_SB        : return SB;
        case REELSET_FS        : return FS;
        case REELSET_FS_WINNER : return FS_winner;
        default:
            throw std::logic_error ("unknown reelset for selection");
    }
}


void ReelSet::printState (const char * message) {
#ifdef DEBUG_LOG_ENABLED
    LOG("reelSet state at: %s\n", message);

    for (Reel* reel: selected->getReels()) {
        LOG("[reel %d] stop=%d\n",
            reel->getId(),
            reel->stopPos()
            );
    }
#endif
}
