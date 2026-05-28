#pragma once

#include <vector>
#include <cstdint>
#include <QString>

class Midi1LiveDecoder {
public:
    static QString decode(const std::vector<uint8_t>& bytes);
};
