#include <iostream>
#include <iomanip>
#include <cctype>
#include "./colors.h"

void xxd(const void* data, size_t length, size_t bytesPerLine = 16);
void printHex(const char* msg, unsigned char* buf, size_t size);
