#pragma once

#include <vector>
#include <cstdint>
#include <QString>

struct Midi1DecodedMessage {
    QString description;
    QString messageType;
    int channel; // 1-16, or -1 for System/Common/Real-Time
    
    // Detailed fields (-1 if not applicable)
    int note = -1;
    int velocity = -1;
    int controller = -1;
    int value = -1;
    int program = -1;
    int pitchBend = -1;
};

class Midi1LiveDecoder {
public:
    static QString decode(const std::vector<uint8_t>& bytes);
    static Midi1DecodedMessage decodeDetailed(const std::vector<uint8_t>& bytes, bool treatNoteOnZeroVelAsOff = false);
};
