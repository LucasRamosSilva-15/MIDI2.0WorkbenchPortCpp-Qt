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
            uint8_t format = (word0 >> 20) & 0xF;
            uint8_t numBytes = (word0 >> 16) & 0xF;
            if (numBytes > 6) numBytes = 6;

            QString formStr;
            switch(format) {
                case 0: formStr = "Complete"; break;
                case 1: formStr = "Start"; break;
                case 2: formStr = "Continue"; break;
                case 3: formStr = "End"; break;
                default: formStr = QString("Unknown(0x%1)").arg(format, 1, 16); break;
            }

            uint8_t bytes[6];
            bytes[0] = (word0 >> 8) & 0xFF;
            bytes[1] = word0 & 0xFF;
            bytes[2] = (words[1] >> 24) & 0xFF;
            bytes[3] = (words[1] >> 16) & 0xFF;
            bytes[4] = (words[1] >> 8) & 0xFF;
            bytes[5] = words[1] & 0xFF;

            QString payload = "";
            for (int i=0; i<numBytes; ++i) {
                if (i > 0) payload += " ";
                payload += QString("%1").arg(bytes[i], 2, 16, QChar('0')).toUpper();
            }

            parsed.description = QString("Data Message (SysEx7) [Group: %1, Form: %2, Bytes: %3] (Payload bruto: %4, parcial/não detalhado)")
                                    .arg(parsed.group).arg(formStr).arg(numBytes).arg(payload);
        } else if (parsed.messageType == 0x5 && words.size() >= 4) {
            uint8_t format = (word0 >> 20) & 0xF;
            uint8_t numBytes = (word0 >> 16) & 0xF;
            
            QString payload = "";
            if (format <= 3) {
                // SysEx8
                if (numBytes > 13) numBytes = 13;
                uint8_t streamId = (word0 >> 8) & 0xFF;
                
                uint8_t bytes[13];
                bytes[0] = word0 & 0xFF;
                bytes[1] = (words[1] >> 24) & 0xFF;
                bytes[2] = (words[1] >> 16) & 0xFF;
                bytes[3] = (words[1] >> 8) & 0xFF;
                bytes[4] = words[1] & 0xFF;
                bytes[5] = (words[2] >> 24) & 0xFF;
                bytes[6] = (words[2] >> 16) & 0xFF;
                bytes[7] = (words[2] >> 8) & 0xFF;
                bytes[8] = words[2] & 0xFF;
                bytes[9] = (words[3] >> 24) & 0xFF;
                bytes[10] = (words[3] >> 16) & 0xFF;
                bytes[11] = (words[3] >> 8) & 0xFF;
                bytes[12] = words[3] & 0xFF;
                
                for (int i=0; i<numBytes; ++i) {
                    if (i > 0) payload += " ";
                    payload += QString("%1").arg(bytes[i], 2, 16, QChar('0')).toUpper();
                }
                
                QString formStr;
                switch(format) {
                    case 0: formStr = "Complete"; break;
                    case 1: formStr = "Start"; break;
                    case 2: formStr = "Continue"; break;
                    case 3: formStr = "End"; break;
                }
                parsed.description = QString("Data Message (SysEx8) [Group: %1, Form: %2, StreamID: 0x%3, Bytes: %4] (Payload bruto: %5, parcial/não detalhado)")
                                        .arg(parsed.group).arg(formStr).arg(streamId, 2, 16, QChar('0')).arg(numBytes).arg(payload);
            } else if (format == 8 || format == 9) {
                // MDS
                if (numBytes > 14) numBytes = 14;
                uint8_t bytes[14];
                bytes[0] = (word0 >> 8) & 0xFF;
                bytes[1] = word0 & 0xFF;
                bytes[2] = (words[1] >> 24) & 0xFF;
                bytes[3] = (words[1] >> 16) & 0xFF;
                bytes[4] = (words[1] >> 8) & 0xFF;
                bytes[5] = words[1] & 0xFF;
                bytes[6] = (words[2] >> 24) & 0xFF;
                bytes[7] = (words[2] >> 16) & 0xFF;
                bytes[8] = (words[2] >> 8) & 0xFF;
                bytes[9] = words[2] & 0xFF;
                bytes[10] = (words[3] >> 24) & 0xFF;
                bytes[11] = (words[3] >> 16) & 0xFF;
                bytes[12] = (words[3] >> 8) & 0xFF;
                bytes[13] = words[3] & 0xFF;
                
                for (int i=0; i<numBytes; ++i) {
                    if (i > 0) payload += " ";
                    payload += QString("%1").arg(bytes[i], 2, 16, QChar('0')).toUpper();
                }
                
                QString formStr = (format == 8) ? "MDS Header" : "MDS Payload";
                parsed.description = QString("Data Message (MDS) [Group: %1, Form: %2, Bytes: %3] (Payload bruto: %4, parcial/não detalhado)")
                                        .arg(parsed.group).arg(formStr).arg(numBytes).arg(payload);
            } else {
                QString formStr = QString("Unknown(0x%1)").arg(format, 1, 16);
                payload = QString("%1 %2 %3")
                                .arg(words[1], 8, 16, QChar('0'))
                                .arg(words[2], 8, 16, QChar('0'))
                                .arg(words[3], 8, 16, QChar('0')).toUpper();
                parsed.description = QString("Data Message (SysEx8/MDS) [Group: %1, Form: %2, Bytes: %3] (Payload bruto word1-3: %4, parcial/não detalhado)")
                                        .arg(parsed.group).arg(formStr).arg(numBytes).arg(payload);
            }
        } else if (parsed.messageType == 0xD && words.size() >= 4) {
            uint8_t format = (word0 >> 22) & 0x3;
            uint8_t address = (word0 >> 20) & 0x3;
            uint8_t channel = (word0 >> 16) & 0xF;
            uint8_t statusBank = (word0 >> 8) & 0xFF;
            uint8_t status = word0 & 0xFF;

            QString formStr = format == 0 ? "Complete" : (format == 1 ? "Start" : (format == 2 ? "Continue" : "End"));
            QString addrStr = address == 0 ? "Channel" : (address == 1 ? "Group" : "Reserved");

            QString chStr = "";
            if (address == 0) {
                chStr = QString(", Ch: %1").arg(channel);
            }

            QString payload = QString("%1 %2 %3")
                                .arg(words[1], 8, 16, QChar('0'))
                                .arg(words[2], 8, 16, QChar('0'))
                                .arg(words[3], 8, 16, QChar('0')).toUpper();

            parsed.description = QString("Flex Data [Group: %1, Addr: %2%3, Bank: 0x%4, Status: 0x%5] (Form: %6, Payload bruto: %7, parcial/não detalhado)")
                              .arg(parsed.group).arg(addrStr).arg(chStr)
                              .arg(statusBank, 2, 16, QChar('0')).arg(status, 2, 16, QChar('0')).arg(formStr).arg(payload);
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
            } else if (status == 0x02 && words.size() >= 4) {
                // Device Identity Notification (Status 0x002)
                // Os campos (Manufacturer, Family, Model e Revision) utilizam estritamente 7 bits.
                // Eles são extraídos e exibidos puramente como valores brutos (hexadecimal),
                // sem qualquer tentativa de resolver nomes de fabricantes através de tabelas externas.
                uint32_t word1 = words[1];
                uint32_t word2 = words[2];
                uint32_t word3 = words[3];

                uint8_t mfr1 = (word1 >> 16) & 0x7F;
                uint8_t mfr2 = (word1 >> 8) & 0x7F;
                uint8_t mfr3 = word1 & 0x7F;

                uint8_t famLsb = (word2 >> 24) & 0x7F;
                uint8_t famMsb = (word2 >> 16) & 0x7F;
                uint8_t modLsb = (word2 >> 8) & 0x7F;
                uint8_t modMsb = word2 & 0x7F;

                uint8_t rev1 = (word3 >> 24) & 0x7F;
                uint8_t rev2 = (word3 >> 16) & 0x7F;
                uint8_t rev3 = (word3 >> 8) & 0x7F;
                uint8_t rev4 = word3 & 0x7F;

                extraInfo = QString(" [Mfr: %1-%2-%3, Fam: %4-%5, Mod: %6-%7, Rev: %8-%9-%10-%11]")
                                .arg(mfr1, 2, 16, QChar('0')).arg(mfr2, 2, 16, QChar('0')).arg(mfr3, 2, 16, QChar('0'))
                                .arg(famLsb, 2, 16, QChar('0')).arg(famMsb, 2, 16, QChar('0'))
                                .arg(modLsb, 2, 16, QChar('0')).arg(modMsb, 2, 16, QChar('0'))
                                .arg(rev1, 2, 16, QChar('0')).arg(rev2, 2, 16, QChar('0'))
                                .arg(rev3, 2, 16, QChar('0')).arg(rev4, 2, 16, QChar('0')).toUpper();
            } else if ((status == 0x03 || status == 0x04) && words.size() >= 4) {
                // Endpoint Name Notification (0x003) e Product Instance ID Notification (0x004)
                uint8_t form = (word0 >> 26) & 0x3;
                
                QString nameStr = "";
                // Os 2 bytes inferiores da Word 0 (bits 15-0) já podem conter ASCII no nome/ID
                for (int shift = 8; shift >= 0; shift -= 8) {
                    char c = (word0 >> shift) & 0xFF;
                    if (c >= 32 && c <= 126) nameStr += QChar(c);
                }
                
                // Words 1, 2 e 3 comportam os 12 bytes restantes (4 bytes cada)
                for (int i = 1; i <= 3; ++i) {
                    uint32_t w = words[i];
                    for (int shift = 24; shift >= 0; shift -= 8) {
                        char c = (w >> shift) & 0xFF;
                        if (c >= 32 && c <= 126) nameStr += QChar(c);
                    }
                }

                QString formWarning = "";
                if (form != 0) {
                    formWarning = " (Fragmentado)";
                }

                extraInfo = QString(" [ASCII filtrado%1: '%2']")
                                .arg(formWarning).arg(nameStr.trimmed());
            } else if ((status == 0x05 || status == 0x06) && words.size() >= 1) {
                // Stream Configuration Request (0x005) e Notification (0x006)
                uint8_t protocol = (word0 >> 8) & 0xFF;
                uint8_t jrRx = (word0 >> 1) & 0x1;
                uint8_t jrTx = word0 & 0x1;

                QString protocolStr;
                if (protocol == 1) protocolStr = "1 (MIDI 1.0)";
                else if (protocol == 2) protocolStr = "2 (MIDI 2.0)";
                else protocolStr = QString("%1 (desconhecido/reservado)").arg(protocol);

                extraInfo = QString(" [%1, JR_RX: %2, JR_TX: %3]")
                                .arg("Protocol: " + protocolStr).arg(jrRx).arg(jrTx);
            } else if (status == 0x10 && words.size() >= 1) {
                // Function Block Discovery (0x010)
                // Os parâmetros (fbNum, reqName, reqInfo) residem nos 16 bits inferiores da word0
                uint8_t fbNum = (word0 >> 8) & 0xFF;
                uint8_t reqName = (word0 >> 1) & 0x1;
                uint8_t reqInfo = word0 & 0x1;

                extraInfo = QString(" [FB#: %1, ReqName: %2, ReqInfo: %3]")
                                .arg(fbNum).arg(reqName).arg(reqInfo);
            } else if (status == 0x11 && words.size() >= 2) {
                // Function Block Info Notification (0x011)
                // word0 (16 bits inferiores) carrega as flags de estado (Active), UI Hint e Direction
                // word1 (32 bits integrais) carrega o First Group, Length, MIDI-CI e SysEx Streams
                uint32_t word1 = words[1];
                
                uint8_t active = (word0 >> 15) & 0x1;
                uint8_t fbNum = (word0 >> 8) & 0x7F;
                uint8_t uiHint = (word0 >> 4) & 0x3;
                uint8_t isMidi1 = (word0 >> 2) & 0x3;
                uint8_t direction = word0 & 0x3;

                uint8_t firstGrp = (word1 >> 24) & 0xFF;
                uint8_t grpLen = (word1 >> 16) & 0xFF;
                uint8_t midiCi = (word1 >> 8) & 0x7F;
                uint8_t maxSysex = word1 & 0xFF;

                extraInfo = QString(" [FB#: %1, Active: %2, Dir: %3, UI: %4, M1: %5 | Grp: %6, Len: %7, CI: %8, SysEx: %9]")
                                .arg(fbNum).arg(active).arg(direction).arg(uiHint).arg(isMidi1)
                                .arg(firstGrp).arg(grpLen).arg(midiCi).arg(maxSysex);
            } else if (status == 0x12 && words.size() >= 4) {
                // Function Block Name Notification (0x012)
                uint8_t fbNum = (word0 >> 8) & 0x7F;
                uint8_t form = (word0 >> 26) & 0x3;
                
                QString nameStr = "";
                for (int i = 1; i <= 3; ++i) {
                    uint32_t w = words[i];
                    for (int shift = 24; shift >= 0; shift -= 8) {
                        char c = (w >> shift) & 0xFF;
                        if (c >= 32 && c <= 126) {
                            nameStr += QChar(c);
                        }
                    }
                }

                QString formWarning = "";
                if (form != 0) {
                    formWarning = " (Fragmentado)";
                }

                extraInfo = QString(" [FB#: %1, ASCII filtrado%2: '%3']")
                                .arg(fbNum).arg(formWarning).arg(nameStr.trimmed());
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

#include <QStringList>

ValidationResult UmpParser::validateAndExtractWords(const QString& hexInput) {
    ValidationResult result;
    result.success = false;
    
    QString cleanInput = "";
    QStringList lines = hexInput.split('\n');
    for (const QString& line : lines) {
        int hashIdx = line.indexOf('#');
        QString codePart = (hashIdx != -1) ? line.left(hashIdx) : line;
        cleanInput += codePart;
    }
    
    cleanInput = cleanInput.remove(" ").remove("\r").remove("\t");
    
    if (cleanInput.isEmpty()) {
        result.errorMessage = "Aviso: A entrada resultou em um buffer vazio após a remoção de formatação.";
        return result;
    }

    // Check invalid characters
    for (int i = 0; i < cleanInput.length(); ++i) {
        QChar c = cleanInput[i];
        if (!c.isDigit() && !(c >= 'A' && c <= 'F') && !(c >= 'a' && c <= 'f')) {
            result.errorMessage = QString("Erro de Validação: Encontrado caractere inválido '%1' (posição %2). Use apenas dígitos hexadecimais (0-9, A-F).").arg(c).arg(i);
            return result;
        }
    }

    // Check padding / odd lengths
    if (cleanInput.length() % 8 != 0) {
        int sobras = cleanInput.length() % 8;
        result.errorMessage = QString("Erro de integridade UMP: O tamanho da entrada (%1 caracteres hexadecimais) não forma words exatas. Há uma sobra de %2 caractere(s). Faltam nibbles ou a estrutura foi truncada.").arg(cleanInput.length()).arg(sobras);
        return result;
    }

    std::vector<uint32_t> allWords;
    for (int i = 0; i < cleanInput.length(); i += 8) {
        QString hexWordStr = cleanInput.mid(i, 8);
        bool ok;
        uint32_t word = hexWordStr.toUInt(&ok, 16);
        if (ok) {
            allWords.push_back(word);
        } else {
            result.errorMessage = QString("Erro Crítico Inesperado: A string '%1' falhou na conversão.").arg(hexWordStr);
            return result;
        }
    }

    int wordIndex = 0;
    while (wordIndex < allWords.size()) {
        uint32_t word0 = allWords[wordIndex];
        int mt = (word0 >> 28) & 0xF;
        int expectedWords = getWordCountForMessageType(mt);
        
        if (wordIndex + expectedWords > allWords.size()) {
            result.errorMessage = QString("Erro de empacotamento UMP: Pacote Incompleto. MT 0x%1 esperava %2 palavras.").arg(mt, 1, 16).arg(expectedWords);
            return result;
        }
        
        std::vector<uint32_t> msgWords;
        for (int i = 0; i < expectedWords; ++i) {
            msgWords.push_back(allWords[wordIndex + i]);
        }
        result.extractedMessages.push_back(msgWords);
        wordIndex += expectedWords;
    }

    result.success = true;
    return result;
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
