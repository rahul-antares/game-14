#include "./byteorder.h"

double htond(double value) {
    uint64_t temp;
    memcpy(&temp, &value, sizeof(temp));

#if __BYTE_ORDER == __LITTLE_ENDIAN
    temp = ((uint64_t)htonl(temp & 0xFFFFFFFF) << 32) | htonl(temp >> 32);
#endif

    memcpy(&value, &temp, sizeof(value));
    return value;
}

double ntohd(double value) {
    return htond(value);
}

uint64_t htonll(uint64_t value) {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    return ((uint64_t)htonl(value & 0xFFFFFFFF) << 32) | htonl(value >> 32);
#else
    return value;  // No conversion needed for big-endian
#endif
}

uint64_t ntohll(uint64_t value) {
    return htonll(value);  // Same function for conversion back
}

