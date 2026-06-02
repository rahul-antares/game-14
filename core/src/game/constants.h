#pragma once

#ifndef GAME11_GAME_CONSTANTS_H 
#define GAME11_GAME_CONSTANTS_H 

const int  MAX_WIN_CAP               = 5000;
const int CREDITS_PER_BET            = 10;
const int  REEL_COUNT                = 5;
const int  GRID_HEIGHT               = 3;
const int NUM_PAYLINES               = 10;

const char GAME_CORE[]               = "bdg7360";
const char GAME_NAME[]               = "Outback Aussies";
const char GAME_VERSION[]            = "1.0";
const char GAME_STUDIO[]             = "BigDaddy";

const char PARSHEET_UPDATE_DETAILS[] = "PAR-SHEET V.1";
const char PARSHEET_VERSION[]        = "v.1";

const double IncProbFreespinsBB      = 79.5;
const double IncProbFreeSpinsSB      = 238.4;

#if defined (VARIANT_96)
const char VARIANT[]          = "96";
#elif defined(VARIANT_94)
const char VARIANT[]          = "94";
#elif defined(VARIANT_92)
const char VARIANT[]          = "92";
#else
#error "invalid variant"
#endif

#endif // GAME11_GAME_CONSTANTS_H
