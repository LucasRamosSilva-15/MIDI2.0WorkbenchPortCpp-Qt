#include "Midi1ToUmpPreviewConverter.h"

Midi1ToUmpPreviewResult Midi1ToUmpPreviewConverter::convert(const std::vector<uint8_t>& bytes, uint8_t group) {
    Midi1ToUmpPreviewResult result;
    
    if (bytes.empty()) {
        result.supported = false;
        result.reason = "Empty message";
        return result;
    }
    
    uint8_t statusByte = bytes[0];
    
    if (statusByte >= 0xF0) {
        result.supported = false;
        result.reason = "Unsupported for UMP preview (System message)";
        return result;
    }
    
    if (statusByte < 0x80) {
        result.supported = false;
        result.reason = "Invalid status byte";
        return result;
    }
    
    uint8_t type = statusByte & 0xF0;
    uint8_t data1 = 0;
    uint8_t data2 = 0;
    
    if (type == 0xC0 || type == 0xD0) {
        if (bytes.size() < 2) {
            result.supported = false;
            result.reason = "Incomplete message (expected 2 bytes)";
            return result;
        }
        data1 = bytes[1];
    } else {
        if (bytes.size() < 3) {
            result.supported = false;
            result.reason = "Incomplete message (expected 3 bytes)";
            return result;
        }
        data1 = bytes[1];
        data2 = bytes[2];
    }
    
    result.supported = true;
    
    // Format: 0x20000000 | (group << 24) | (statusByte << 16) | (data1 << 8) | data2
    result.umpWord = 0x20000000 | ((group & 0x0F) << 24) | (statusByte << 16) | (data1 << 8) | data2;
    result.umpHex = QString("%1").arg(result.umpWord, 8, 16, QChar('0')).toUpper();
    
    QString typeStr;
    switch (type) {
        case 0x80: typeStr = "Note Off"; break;
        case 0x90: typeStr = "Note On"; break;
        case 0xA0: typeStr = "Poly Aftertouch"; break;
        case 0xB0: typeStr = "Control Change"; break;
        case 0xC0: typeStr = "Program Change"; break;
        case 0xD0: typeStr = "Channel Aftertouch"; break;
        case 0xE0: typeStr = "Pitch Bend"; break;
        default: typeStr = "Unknown Channel Voice"; break;
    }
    
    int channel = (statusByte & 0x0F) + 1;
    result.description = QString("UMP MIDI 1.0 Channel Voice | Group %1 | %2 Ch %3").arg(group).arg(typeStr).arg(channel);
    
    return result;
}
