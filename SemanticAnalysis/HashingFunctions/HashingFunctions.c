//
// Created by Owner on 19/03/2026.
//

#include "HashingFunctions.h"
unsigned int fnv1a_hash(const char s[], const int length) {
    //FNV-1a
    unsigned int val = 2166136261u;
    for (int i = 0; i < length; i++) {
        val ^= (unsigned char) s[i];
        val *= 16777619u;
    }
    return val;
}