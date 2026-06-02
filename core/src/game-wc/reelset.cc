#include <stdexcept>
#include "constants.h"
#include "rng.h"
#include "../utils/log.h"
#include "game.h"

ReelSetWC::ReelSetWC (RngLib& rng) : ReelSet(rng) {
}

void ReelSetWC::select (enum ReelSetsEnums set) {
    return ReelSet::select(set, false, false);
}

std::array<int, REEL_COUNT> ReelSetWC::spin(bool basespin, int mode, bool reSpin, bool forced, bool moreCoins, int fsStopIdx) {
    RoundWC* r = dynamic_cast<RoundWC*>(round);

    if (!r)
        return ReelSet::spin(basespin, mode, reSpin, forced, moreCoins);

    Cheats& cheats = ((RoundWC*) round)->cheats;
    LOG("[** CHEAT **] ReelSetWC::spin\n");

    if (basespin && cheats.stops.size() > 0 && !reSpin) {
        return spinCheatStops(cheats.stops, basespin);
    }

    if (!basespin && !reSpin && cheats.fsStops.size() > fsStopIdx && fsStopIdx >= 0)
        return spinCheatStops(cheats.fsStops[fsStopIdx], basespin);

    // Else do the default spin
    return ReelSet::spin(basespin, mode, reSpin, forced, moreCoins);
}

std::array<int, REEL_COUNT> ReelSetWC::spinCheatStops (std::array<int, REEL_COUNT> cheatStops, int mode) {
    LOG("FROM REELSET-SPIN WITH CHEATS\n");

    Reels_V& reels = selected->getReels();
    std::array<int, REEL_COUNT> stops;

    for (unsigned int i = 0; i < REEL_COUNT; i++) {
        Reel* reel = reels.at(i);
        int stop = cheatStops[i];

        if (stop == -1) {
            stop   = reel->spin();
        } else {
            LOG("reel - size for cheat[%d] = %d", i, reel->size());
            stop = stop % reel->size();
        }

        stops[i] = stop;
    }

    return stops;
}

std::vector<int> ReelSetWC::makeCombo(int minSum) {
    const std::vector<int> multis = ((RoundWC*)round)->multis;
    const int maxTries = 12000000; // Approximately 1/( inverse of 15 to the power 6)
    int sum = 0;
    int tries = 0;
    std::vector<int> list;

    do {
        sum = 0;
        list.clear();

        for (int i = 0; i < REEL_COUNT; i++) {
            const int r = rng.getScaled(multis.size());
            const int value = multis[r];
            sum += value;
            list.push_back (value);

            if (sum >= minSum)
                break;
        }
        tries++;
    } while (sum < minSum && tries < maxTries);

    if (tries >= maxTries && sum < minSum)
        throw std::logic_error("too many retries");

    return list;
}

std::array<int, REEL_COUNT> ReelSetWC::spinGetMPMinSum (int minMultiSum) {
    std::array<int, REEL_COUNT> stops;
    std::vector<int> reelIds;
    std::array<bool, REEL_COUNT> reelsToChoose = { false };

    std::vector<int> multiList = makeCombo(minMultiSum);
    ((RoundWC *)round)->cheatMultiList = multiList;

    LOG("[** CHEAT **] MP values to achieve minMultiSum %d =", minMultiSum);
    for (int v : multiList)
        LOG(" %d", v);
    LOG("\n");

    for (int i = 0; i < REEL_COUNT; i++)
        reelIds.push_back(i);

    for (int remaining = multiList.size() -1; remaining >= 0; remaining--) {
        int rand = rng.getScaled(reelIds.size());
        reelsToChoose[(reelIds[rand])] = true;
        reelIds.erase(reelIds.begin() + rand);
    }

    Reels_V reels = selected->getReels();
    /*for (int i = 0; i < REEL_COUNT; i++) {*/
    /*    Reel* reel = reels.at(i);*/
    /**/
    /*    if (!reelsToChoose[i]) {*/
    /*        stops[i] = reel->spin();*/
    /*        continue;*/
    /*    }*/
    /**/
    /*    // Find all stop positions with MP*/
    /*    std::vector<int> stopsWithMP;*/
    /*    for (unsigned int i = 0; i < reel->getReel().size(); i++) {*/
    /*        Symbol sym = reel->symbolAt(i);*/
    /**/
    /*        if (sym == MP)*/
    /*            stopsWithMP.push_back(i);*/
    /*    }*/
    /**/
    /*    int whichStop = rng.getScaled(stopsWithMP.size());*/
    /*    int randomRow = rng.getScaled(GRID_HEIGHT);*/
    /*    int forceStop = std::abs ((stopsWithMP[whichStop] - randomRow) % (int) reels.at(i)->getReel().size());*/
    /**/
    /*    reel->setStop(forceStop);*/
    /*    stops[i] = reel->stopPos();*/
    /*}*/

    return stops;
}

std::array<int, REEL_COUNT> ReelSetWC::spinGetMP () {
    LOG("[** CHEAT **] multiplier is true\n");

    // Select a random reel, find all it's MP positions,
    // and stop at some random MP position
    unsigned int whichReel;
    std::array<int, REEL_COUNT> stops;
    Reels_V& reels = selected->getReels();
    std::vector<int> stopsWithMP;
    int whichStop = rng.getScaled(stopsWithMP.size());
    int randomRow = rng.getScaled(GRID_HEIGHT);
    int forceStop = std::abs ((stopsWithMP[whichStop] - randomRow) % (int) reels.at(whichReel)->getReel().size());

    reels.at(whichReel)->setStop(forceStop);

    // Spin all other reels except the "whichReel"
    for (unsigned int i = 0; i < reels.size(); i++) {
        Reel* reel = reels.at(i);
        int stop   = (whichReel == i? reel->stopPos() : reel->spin());
        stops[i]   = stop;
    }

    return stops;
}
