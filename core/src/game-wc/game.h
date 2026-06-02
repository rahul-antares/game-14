#pragma once

#include <vector>
#include "constants.h"
#include "data-defs.h"
#include "rng.h"
#include "../game/game.h"
#include "cheats.h"

const int NUM_SCATTERS[] = { 71, 25, 4 };

class GameWithCheats : public Game {
    public:
        GameWithCheats();

        using Game::play;
        PlayResponse play(PlayOptions options, Cheats cheats);
        PlayResponse actuallyPlay(PlayOptions options, Cheats& cheats);

        using Game::next;
        PlayResponse next(NextPlay next, PlayResponse prev, Cheats cheats);
        PlayResponse actuallyNext(NextPlay next, PlayResponse prev, Cheats cheats);

        ~GameWithCheats();
    private:
        Cheats cheats;

        // Cheat methods
        int getNumScatters ();
        bool gotDesiredOutcome (PlayResponse& response, Cheats& cheats, int counter);
        bool gotDesiredOutcomeNext (NextPlay& next, PlayResponse& response, Cheats& cheats, int counter);
        bool evaluateMinWin (PlayResponse& response, Cheats& cheats, int counter);
        bool evaluateMaxWin (PlayResponse& response, Cheats& cheats, int counter);
        bool evaluateWinSym (PlayResponse& response, Cheats& cheats, int counter);
        bool evaluateExcludeSym (PlayResponse& response, Cheats& cheats, int counter);
        bool evaluateStops (PlayResponse& response, Cheats& cheats, int counter);
        bool evaluatePaylineWin (PlayResponse& response, Cheats& cheats, int counter);
        bool evaluateReelset (PlayResponse& response, Cheats& cheats, int counter);

        bool evaluateFreeSpins (PlayResponse& response, Cheats& cheats, int counter);
        bool evaluateReSpin (PlayResponse& response, Cheats& cheats, int counter);
        bool evaluateTopRowReSpin (PlayResponse& response, Cheats& cheats, int counter);
        bool evaluateNumCoins (PlayResponse& response, Cheats& cheats, int counter);
        bool evaluateScatterHook (PlayResponse& response, Cheats& cheats, int counter);
        bool evaluateNumFeatures (PlayResponse& response, Cheats& cheats, int counter);
        bool evaluateFeatures (PlayResponse& response, Cheats& cheats, int counter);
        bool evaluateCoinValue (PlayResponse& response, Cheats& cheats, int counter);
        bool evaluateWinCap (PlayResponse& response, Cheats& cheats, int counter);
        bool evaluateFsDoubleWild (PlayResponse& response, Cheats& cheats, int counter);
        bool evaluateFsCoinValue (PlayResponse& response, Cheats& cheats, int counter);
        bool evaluateFsCoinValueCollected (PlayResponse& response, Cheats& cheats, int counter);
        bool evaluateFsCoinValueCollectedEveryLevel (PlayResponse& response, Cheats& cheats, int counter);
        bool evaluateFsCoinValueEveryLevel (PlayResponse& response, Cheats& cheats, int counter);

        bool evaluateMinCollectionLevel (PlayResponse& response, Cheats& cheats, int counter, const bool inNext);
        bool evaluateMaxCollectionLevel (PlayResponse& response, Cheats& cheats, int counter, const bool inNext);
        bool evaluateMaxFsWin (PlayResponse& response, Cheats& cheats, int counter, bool inNext);
        bool evaluateMinFsWin (PlayResponse& response, Cheats& cheats, int counter);
        bool evaluateNumScatters (PlayResponse& response, Cheats& cheats, int counter);
        bool evaluateReachLevel (NextPlay& next, PlayResponse& response, Cheats& cheats, int counter);
        bool evaluateReachLevel (PlayResponse& response, Cheats& cheats, int counter, const bool inInext);
        bool evaluateRespinsRetrigger (PlayResponse& response, Cheats& cheats, int counter);
        bool evaluateGambleSuccessLevel ([[ maybe_unused ]] NextPlay& next, PlayResponse& response, Cheats& cheats, [[ maybe_unused ]] int counter);
};

class ReelSetWC: public ReelSet {
    public:
        ReelSetWC(RngLib& rng);
        void select (enum ReelSetsEnums set);
        std::array<int, REEL_COUNT> spin (bool baseSpin, int mode, bool reSpin, bool forced = false, bool moreCoins = false, int fsStopIdx = -1);
        std::array<int, REEL_COUNT> spinCheatStops (std::array<int, REEL_COUNT> stops, int mode);

    private:
        std::array<int, REEL_COUNT> spinGetMP ();
        std::array<int, REEL_COUNT> spinGetMPMinSum (int minMultiSum);
        std::vector<int> makeCombo(int minSum);
};

class PayWindowWC : public PayWindow {
    public:
        PayWindowWC(
                RngLib& rng,
                ReelSetWC& reelSet
                )
            :
            PayWindow(rng, reelSet)
    {};

    int placeWS(const int mode, const bool baseGame, const bool reSpin, SymCoords_Vec& wildSyms, SymCoords_Vec& newWSPlaced, int fsLevel, bool forced, bool lastFour);
    int replaceR (const int mode, const bool baseGame, SymCoords_Vec& bonusSyms, SymCoords_Vec& rSymReplacements);
};

class RoundWC : public Round {
    public:
        RoundWC(
                RngLib& rng,
                PlayResponse& playResponse,
                PayWindowWC& payWindow,
                Cheats& cheats
                )
            :
            Round(rng, playResponse, payWindow),
            cheats(cheats) {};

        Cheats& cheats;
        std::vector<int> multis;
        std::vector<int> cheatMultiList;

    private:
};

