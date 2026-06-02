#include "./hexdump.h"

void xxd(const void* data, size_t length, size_t bytesPerLine) {
    const unsigned char* ptr = (const unsigned char*)data;

    for (size_t i = 0; i < length; i += bytesPerLine) {
        // Print the offset
        printf("%s%08x: ", ANSI_COLOR_RESET, (unsigned int)i);

        // Print hex values
        for (size_t j = 0; j < bytesPerLine; ++j) {
            if (j%2 == 0)
                printf(" ");

            if (i + j < length)
                printf("%s%02x", ANSI_COLOR_MAGENTA, ptr[i + j]);
            else
                printf("    ");
        }

        printf("  ");

        // Print ASCII representation
        for (size_t j = 0; j < bytesPerLine; ++j) {
            if (i + j < length) {
                char ch = ptr[i + j];
                printf("%s%c%s", ANSI_COLOR_YELLOW, (std::isprint(ch) ? ch : '.'), ANSI_COLOR_RESET);
            }
        }

        printf("\n");
    }
}

void printHex(const char* msg, unsigned char* buf, size_t size) {
    if (msg)
        printf("%s : ", msg);

    for (size_t i = 0; i < size; i++) {
        printf("%02X", buf[i]);
    }
    printf("\n");
}

