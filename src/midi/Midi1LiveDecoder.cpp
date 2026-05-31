#include "Midi1LiveDecoder.h"

QString Midi1LiveDecoder::decode(const std::vector<uint8_t>& bytes) {
    return decodeDetailed(bytes).description;
}

Midi1DecodedMessage Midi1LiveDecoder::decodeDetailed(const std::vector<uint8_t>& bytes, bool treatNoteOnZeroVelAsOff) {
    Midi1DecodedMessage msg;
    msg.channel = -1;

    if (bytes.empty()) {
        msg.description = "Invalido/Vazio";
        msg.messageType = "Invalido";
        return msg;
    }
    
    uint8_t statusByte = bytes[0];
    
    if (statusByte >= 0xF0) {
        msg.description = "System/Common/Real-Time (parcial/bruto)";
        msg.messageType = "System/Common/Real-Time";
        return msg;
    }
    
    if (statusByte < 0x80) {
        msg.description = "Status Byte Invalido";
        msg.messageType = "Invalido";
        return msg;
    }
    
    uint8_t type = statusByte & 0xF0;
    msg.channel = (statusByte & 0x0F) + 1; // 1 a 16
    
    QString typeStr;
    QString dataStr;
    
    switch (type) {
        case 0x80: // Note Off
            if (bytes.size() >= 3) {
                typeStr = "Note Off";
                msg.messageType = "Note Off";
                dataStr = QString("Note: %1, Vel: %2").arg(bytes[1]).arg(bytes[2]);
            }
            break;
        case 0x90: // Note On
            if (bytes.size() >= 3) {
                if (bytes[2] == 0) {
                    if (treatNoteOnZeroVelAsOff) {
                        typeStr = "Note Off";
                        msg.messageType = "Note Off";
                        dataStr = QString("Note: %1, Vel: 0").arg(bytes[1]);
                    } else {
                        typeStr = "Note On (vel 0 / Note Off)";
                        msg.messageType = "Note On";
                        dataStr = QString("Note: %1").arg(bytes[1]);
                    }
                } else {
                    typeStr = "Note On";
                    msg.messageType = "Note On";
                    dataStr = QString("Note: %1, Vel: %2").arg(bytes[1]).arg(bytes[2]);
                }
            }
            break;
        case 0xA0: // Poly Aftertouch
            if (bytes.size() >= 3) {
                typeStr = "Poly Aftertouch";
                msg.messageType = "Poly Aftertouch";
                dataStr = QString("Note: %1, Pressure: %2").arg(bytes[1]).arg(bytes[2]);
            }
            break;
        case 0xB0: // Control Change
            if (bytes.size() >= 3) {
                typeStr = "Control Change";
                msg.messageType = "Control Change";
                dataStr = QString("CC: %1, Val: %2").arg(bytes[1]).arg(bytes[2]);
            }
            break;
        case 0xC0: // Program Change
            if (bytes.size() >= 2) {
                typeStr = "Program Change";
                msg.messageType = "Program Change";
                dataStr = QString("Prog: %1").arg(bytes[1]);
            }
            break;
        case 0xD0: // Channel Aftertouch
            if (bytes.size() >= 2) {
                typeStr = "Channel Aftertouch";
                msg.messageType = "Channel Aftertouch";
                dataStr = QString("Pressure: %1").arg(bytes[1]);
            }
            break;
        case 0xE0: // Pitch Bend
            if (bytes.size() >= 3) {
                typeStr = "Pitch Bend";
                msg.messageType = "Pitch Bend";
                int value = (bytes[2] << 7) | bytes[1]; // MSB << 7 + LSB
                dataStr = QString("Val: %1").arg(value);
            }
            break;
        default:
            msg.description = "Desconhecido";
            msg.messageType = "Desconhecido";
            return msg;
    }
    
    if (typeStr.isEmpty()) {
        msg.description = "Mensagem Incompleta";
        msg.messageType = "Incompleto";
        return msg;
    }
    
    msg.description = QString("%1 [Ch %2] %3").arg(typeStr).arg(msg.channel).arg(dataStr);
    return msg;
}
