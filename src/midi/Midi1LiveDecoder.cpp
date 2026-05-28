#include "Midi1LiveDecoder.h"

QString Midi1LiveDecoder::decode(const std::vector<uint8_t>& bytes) {
    if (bytes.empty()) return "Invalido/Vazio";
    
    uint8_t statusByte = bytes[0];
    
    if (statusByte >= 0xF0) {
        return "System/Common/Real-Time (parcial/bruto)";
    }
    
    if (statusByte < 0x80) {
        return "Status Byte Invalido";
    }
    
    uint8_t type = statusByte & 0xF0;
    uint8_t channel = (statusByte & 0x0F) + 1; // 1 a 16
    
    QString typeStr;
    QString dataStr;
    
    switch (type) {
        case 0x80: // Note Off
            if (bytes.size() >= 3) {
                typeStr = "Note Off";
                dataStr = QString("Note: %1, Vel: %2").arg(bytes[1]).arg(bytes[2]);
            }
            break;
        case 0x90: // Note On
            if (bytes.size() >= 3) {
                if (bytes[2] == 0) {
                    typeStr = "Note On (vel 0 / Note Off)";
                    dataStr = QString("Note: %1").arg(bytes[1]);
                } else {
                    typeStr = "Note On";
                    dataStr = QString("Note: %1, Vel: %2").arg(bytes[1]).arg(bytes[2]);
                }
            }
            break;
        case 0xA0: // Poly Aftertouch
            if (bytes.size() >= 3) {
                typeStr = "Poly Aftertouch";
                dataStr = QString("Note: %1, Pressure: %2").arg(bytes[1]).arg(bytes[2]);
            }
            break;
        case 0xB0: // Control Change
            if (bytes.size() >= 3) {
                typeStr = "Control Change";
                dataStr = QString("CC: %1, Val: %2").arg(bytes[1]).arg(bytes[2]);
            }
            break;
        case 0xC0: // Program Change
            if (bytes.size() >= 2) {
                typeStr = "Program Change";
                dataStr = QString("Prog: %1").arg(bytes[1]);
            }
            break;
        case 0xD0: // Channel Aftertouch
            if (bytes.size() >= 2) {
                typeStr = "Channel Aftertouch";
                dataStr = QString("Pressure: %1").arg(bytes[1]);
            }
            break;
        case 0xE0: // Pitch Bend
            if (bytes.size() >= 3) {
                typeStr = "Pitch Bend";
                int value = (bytes[2] << 7) | bytes[1]; // MSB << 7 + LSB
                dataStr = QString("Val: %1").arg(value);
            }
            break;
        default:
            return "Desconhecido";
    }
    
    if (typeStr.isEmpty()) {
        return "Mensagem Incompleta";
    }
    
    return QString("%1 [Ch %2] %3").arg(typeStr).arg(channel).arg(dataStr);
}
