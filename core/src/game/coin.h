#pragma once

#if defined (VARIANT_96)
const double ProbBaseGameNormalSetA   = 0.24673;
const double ProbBaseGameEnhancedSetA = 0.97639;
const double ProbBaseGameWildSpinSetA = 0.68326;
const double ProbFreeSpinLVL1SetA     = 0.88895;
const double ProbFreeSpinLVL2SetA     = 0.8442;
const double ProbFreeSpinLVL3SetA     = 0.72893;

#elif defined (VARIANT_94)
const double ProbBaseGameNormalSetA   = 0.12684;
const double ProbBaseGameEnhancedSetA = 0.84278;
const double ProbBaseGameWildSpinSetA = 0.62116;
const double ProbFreeSpinLVL1SetA     = 0.59633;
const double ProbFreeSpinLVL2SetA     = 0.57862;
const double ProbFreeSpinLVL3SetA     = 0.50029;

#elif defined (VARIANT_92)
const double ProbBaseGameNormalSetA   = 0.00695;
const double ProbBaseGameEnhancedSetA = 0.70918;
const double ProbBaseGameWildSpinSetA = 0.55905;
const double ProbFreeSpinLVL1SetA     = 0.30371;
const double ProbFreeSpinLVL2SetA     = 0.31305;
const double ProbFreeSpinLVL3SetA     = 0.27165;

#else
#error "unknown coin config for unknown variant"
#endif
