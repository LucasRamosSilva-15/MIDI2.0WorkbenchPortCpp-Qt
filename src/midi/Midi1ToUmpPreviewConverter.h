#pragma once

#include <vector>
#include <cstdint>
#include <QString>

struct Midi1ToUmpPreviewResult {
    bool supported = false;
    uint32_t umpWord = 0;
    QString umpHex;
    QString description;
    QString reason;
};

class Midi1ToUmpPreviewConverter {
public:
    static Midi1ToUmpPreviewResult convert(const std::vector<uint8_t>& bytes, uint8_t group = 0);
};
