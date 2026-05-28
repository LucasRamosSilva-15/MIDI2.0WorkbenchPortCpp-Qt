#pragma once

#include "InputSourceType.h"
#include <vector>
#include <cstdint>

struct MidiRawEvent {
    InputSourceType sourceType;
    double timestamp;         // Em milissegundos
    std::vector<uint8_t> midi1Bytes; 
    std::vector<uint32_t> umpWords;
};
