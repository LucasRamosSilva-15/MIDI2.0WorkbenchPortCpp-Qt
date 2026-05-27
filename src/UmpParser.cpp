#include "UmpParser.h"
#include <QString>

int UmpParser::getWordCountForMessageType(int mt) {
    switch (mt) {
        // 32-bit Messages (1 word)
        case 0x0: // Utility
        case 0x1: // System Real Time and System Common
        case 0x2: // MIDI 1.0 Channel Voice
        case 0x6: // Reserved 32-bit
        case 0x7: // Reserved 32-bit
            return 1;
            
        // 64-bit Messages (2 words)
        case 0x3: // Data Messages (SysEx7)
        case 0x4: // MIDI 2.0 Channel Voice
        case 0x8: // Reserved 64-bit
        case 0x9: // Reserved 64-bit
        case 0xA: // Reserved 64-bit
            return 2;
            
        // 96-bit Messages (3 words)
        case 0xB: // Reserved 96-bit
        case 0xC: // Reserved 96-bit
            return 3;
            
        // 128-bit Messages (4 words)
        case 0x5: // Data Messages (SysEx8 / Mixed Data Set)
        case 0xD: // Flex Data
        case 0xE: // Reserved 128-bit
        case 0xF: // UMP Stream / Endpoint
        default:
            return 4; 
    }
}

ParsedUmp UmpParser::parseMessage(const std::vector<uint32_t>& words) {
    ParsedUmp parsed;
    if (words.empty()) return parsed;

    parsed.words = words;
    parsed.sizeBits = words.size() * 32;
    
    uint32_t word0 = words[0];
    
    parsed.messageType = (word0 >> 28) & 0xF;
    
    // MT 0xF não possui um Group Field de 4-bits da mesma forma. Aqueles bits são Format/Status do Stream.
    if (parsed.messageType == 0xF) {
        parsed.group = -1;
    } else {
        parsed.group = (word0 >> 24) & 0xF;
    }
    
    // Status e Channel aplicam-se majoritariamente para Voice Messages (MT 0x2 e MT 0x4).
    if (parsed.messageType == 0x2 || parsed.messageType == 0x4) {
        parsed.status = (word0 >> 20) & 0xF;
        parsed.channel = (word0 >> 16) & 0xF;
        
        if (parsed.messageType == 0x2) { // MIDI 1.0
            uint8_t byte1 = (word0 >> 8) & 0xFF;
            uint8_t byte2 = word0 & 0xFF;
            
            switch(parsed.status) {
                case 0x8: parsed.description = QString("Note Off (Note: %1, Vel: %2)").arg(byte1).arg(byte2); break;
                case 0x9: parsed.description = QString("Note On (Note: %1, Vel: %2)").arg(byte1).arg(byte2); break;
                case 0xA: parsed.description = QString("Poly Aftertouch (Note: %1, Pressure: %2)").arg(byte1).arg(byte2); break;
                case 0xB: parsed.description = QString("Control Change (CC: %1, Val: %2)").arg(byte1).arg(byte2); break;
                case 0xC: parsed.description = QString("Program Change (Prog: %1)").arg(byte1); break;
                case 0xD: parsed.description = QString("Channel Aftertouch (Pressure: %1)").arg(byte1); break;
                case 0xE: {
                    uint16_t pbValue = (byte2 << 7) | (byte1 & 0x7F);
                    parsed.description = QString("Pitch Bend (Val: %1)").arg(pbValue); 
                    break;
                }
                default: parsed.description = "Unknown Voice Message"; break;
            }
        } else if (parsed.messageType == 0x4 && words.size() >= 2) { // MIDI 2.0
            // MIDI 2.0 Voice Messages (MT 0x4) são interpretadas usando offsets de bits 
            // rigorosos distribuídos em word0 (32 bits superiores) e word1 (32 bits inferiores).
            uint32_t word1 = words[1];
            uint8_t index = (word0 >> 8) & 0xFF; // Word 0: Bits 15-8
            
            switch(parsed.status) {
                case 0x8: {
                    // Word 1: Bits 31-16 (Velocity), Bits 15-0 (Attribute Data)
                    uint16_t vel = (word1 >> 16) & 0xFFFF;
                    parsed.description = QString("Note Off (Note: %1, Vel 16b: %2, Atributos: não detalhados)").arg(index).arg(vel);
                    break;
                }
                case 0x9: {
                    // Word 1: Bits 31-16 (Velocity), Bits 15-0 (Attribute Data)
                    uint16_t vel = (word1 >> 16) & 0xFFFF;
                    parsed.description = QString("Note On (Note: %1, Vel 16b: %2, Atributos: não detalhados)").arg(index).arg(vel);
                    break;
                }
                case 0xA: {
                    // Word 0 Bits 15-8 = Note, Word 1 = 32-bit Data
                    parsed.description = QString("Poly Aftertouch (Note: %1, Val 32b: %2)").arg(index).arg(word1); 
                    break;
                }
                case 0xB: {
                    // Word 0 Bits 15-8 = CC Index, Word 1 = 32-bit Data
                    parsed.description = QString("Control Change (CC: %1, Val 32b: %2)").arg(index).arg(word1); 
                    break;
                }
                case 0xC: {
                    // Program Change: Envolve Flags em word0 e Banks MSB/LSB em word1.
                    // Para evitar falsos positivos se a especificação mudar ou for interpretada mal, 
                    // marcamos como parcial e não extraímos dados numéricos cegos.
                    parsed.description = "Program Change (parcial)";
                    break;
                }
                case 0xD: {
                    // Word 1 = 32-bit Data
                    parsed.description = QString("Channel Aftertouch (Val 32b: %1)").arg(word1); 
                    break;
                }
                case 0xE: {
                    // Word 1 = 32-bit Data
                    parsed.description = QString("Pitch Bend (Val 32b: %1)").arg(word1); 
                    break;
                }
                case 0x0: case 0x1: case 0x6: case 0xF: {
                    // 0x0=PNRC, 0x1=PNAC, 0x6=Per-Note PB, 0xF=Per-Note Management
                    parsed.description = "Per-Note Message (parcial/não detalhado)";
                    break;
                }
                case 0x2: case 0x3: case 0x4: case 0x5: {
                    // 0x2=RC, 0x3=AC, 0x4=Rel RC, 0x5=Rel AC
                    parsed.description = "Registered/Assignable Controller (parcial/não detalhado)";
                    break;
                }
                case 0x7: {
                    parsed.description = "Reserved Voice Message";
                    break;
                }
                default: 
                    parsed.description = "Unknown Voice Message"; 
                    break;
            }
        } else {
            parsed.description = "Voice Message (Dados Incompletos)";
        }
    } else {
        // Para os outros MTs (Utility, System, SysEx, Flex, Stream, etc), deixamos Status e Channel como -1.
        if (parsed.messageType == 0x0) {
            parsed.description = "Utility Message";
        } else if (parsed.messageType == 0x1) {
            parsed.description = "System Real Time / System Common";
        } else if (parsed.messageType == 0x3 && words.size() >= 2) {
            QString payload = QString("%1").arg(words[1], 8, 16, QChar('0')).toUpper();
            parsed.description = QString("Data Message (SysEx7, Payload bruto: %1)").arg(payload);
        } else if (parsed.messageType == 0x5 && words.size() >= 4) {
            QString payload = QString("%1 %2 %3")
                                .arg(words[1], 8, 16, QChar('0'))
                                .arg(words[2], 8, 16, QChar('0'))
                                .arg(words[3], 8, 16, QChar('0')).toUpper();
            parsed.description = QString("Data Message (SysEx8/MDS, Payload bruto: %1)").arg(payload);
        } else if (parsed.messageType == 0xD && words.size() >= 4) {
            QString payload = QString("%1 %2 %3")
                                .arg(words[1], 8, 16, QChar('0'))
                                .arg(words[2], 8, 16, QChar('0'))
                                .arg(words[3], 8, 16, QChar('0')).toUpper();
            parsed.description = QString("Flex Data (parcial/não detalhado, Payload bruto: %1)").arg(payload);
        } else if (parsed.messageType == 0xF && words.size() >= 4) {
            // MT 0xF (UMP Stream): O cabeçalho base é comum a todos os Stream Messages.
            // Bits 27-26: Form (2 bits)
            // Bits 25-16: Status (10 bits)
            uint8_t form = (word0 >> 26) & 0x3;
            uint16_t status = (word0 >> 16) & 0x3FF;
            
            QString formStr;
            switch(form) {
                case 0: formStr = "Complete"; break;
                case 1: formStr = "Start"; break;
                case 2: formStr = "Continue"; break;
                case 3: formStr = "End"; break;
            }

            QString statusName;
            switch(status) {
                case 0x00: statusName = "Endpoint Discovery"; break;
                case 0x01: statusName = "Endpoint Info Notification"; break;
                case 0x02: statusName = "Device Identity Notification"; break;
                case 0x03: statusName = "Endpoint Name Notification"; break;
                case 0x04: statusName = "Product Instance Id Notification"; break;
                case 0x05: statusName = "Stream Configuration Request"; break;
                case 0x06: statusName = "Stream Configuration Notification"; break;
                case 0x10: statusName = "Function Block Discovery"; break;
                case 0x11: statusName = "Function Block Info Notification"; break;
                case 0x12: statusName = "Function Block Name Notification"; break;
                default: statusName = "não detalhado"; break;
            }

            QString extraInfo = "";
            // Extração estrita para o Status 0x001 (Endpoint Info Notification).
            // Estes campos como UMP Version, numFb, M1/M2 Support, etc., aplicam-se APENAS
            // a esta notificação específica e NÃO são campos genéricos de toda mensagem UMP Stream.
            if (status == 0x01 && words.size() >= 2) {
                uint32_t word1 = words[1];
                uint8_t major = (word0 >> 8) & 0xFF;
                uint8_t minor = word0 & 0xFF;
                uint8_t staticFb = (word1 >> 31) & 0x1;
                uint8_t numFb = (word1 >> 24) & 0x7F;
                uint8_t midi2 = (word1 >> 9) & 0x1;
                uint8_t midi1 = (word1 >> 8) & 0x1;
                uint8_t jrRx = (word1 >> 1) & 0x1;
                uint8_t jrTx = word1 & 0x1;

                extraInfo = QString(" [UMP v%1.%2, FuncBlocks: %3 (Static: %4), M2: %5, M1: %6, JRR: %7, JRT: %8]")
                                .arg(major).arg(minor)
                                .arg(numFb).arg(staticFb)
                                .arg(midi2).arg(midi1)
                                .arg(jrRx).arg(jrTx);
            }

            QString payload = QString("%1 %2 %3")
                                .arg(words[1], 8, 16, QChar('0'))
                                .arg(words[2], 8, 16, QChar('0'))
                                .arg(words[3], 8, 16, QChar('0')).toUpper();
            
            parsed.description = QString("UMP Stream: %1%2 (Form: %3, Status bruto: 0x%4, Payload bruto: %5, parcial/não detalhado)")
                                    .arg(statusName)
                                    .arg(extraInfo)
                                    .arg(formStr)
                                    .arg(QString("%1").arg(status, 3, 16, QChar('0')).toUpper())
                                    .arg(payload);
        } else {
            // Tipos Reservados: 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xE
            parsed.description = "Reserved / Future Use (Não interpretado)";
        }
    }
    
    return parsed;
}

QString UmpParser::getMessageTypeString(int mt) {
    switch(mt) {
        case 0x0: return "Utility (0x0)";
        case 0x1: return "System (0x1)";
        case 0x2: return "MIDI 1.0 (0x2)";
        case 0x3: return "SysEx7 (0x3)";
        case 0x4: return "MIDI 2.0 (0x4)";
        case 0x5: return "SysEx8/MDS (0x5)";
        case 0xD: return "Flex Data (0xD)";
        case 0xF: return "Stream/Endpoint (0xF)";
        default: return QString("Reserved (0x%1)").arg(mt, 1, 16).toUpper();
    }
}
