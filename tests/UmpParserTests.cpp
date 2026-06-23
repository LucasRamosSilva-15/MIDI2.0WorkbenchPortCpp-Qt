#include <iostream>
#include <vector>
#include <string>
#include <QString>
#include "../src/UmpParser.h"
#include "../src/midi/Midi1LiveDecoder.h"
#include "../src/midi/Midi1ToUmpPreviewConverter.h"
#include "../src/midi/FakeUmpInputBackend.h"
#include "../src/midi/WindowsMidiServicesBackend.h"
#include "../src/midi/WindowsMidiServicesSdkProbe.h"

#include "TestUtils.h"
#include "WindowsMidiServicesBackendTests.h"

int testsRun = 0;
int testsPassed = 0;



int main() {
    std::cout << "Starting UMP Parser Tests (v0.8.0)\n" << std::endl;

    // 1. System Common / Real-Time (MT 0x1)
    {
        ValidationResult result = UmpParser::validateAndExtractWords("10F80000");
        assertTest("MT 0x1 valid Timing Clock", result.success && result.extractedMessages.size() == 1);
        if (result.success) {
            ParsedUmp parsed = UmpParser::parseMessage(result.extractedMessages[0]);
            assertTest("MT 0x1 MT check", parsed.messageType == 0x1);
            assertTest("MT 0x1 Size check", parsed.sizeBits == 32);
        }

        ValidationResult resultStart = UmpParser::validateAndExtractWords("10FA0000");
        assertTest("MT 0x1 valid Start", resultStart.success);
        
        ValidationResult resultStop = UmpParser::validateAndExtractWords("10FC0000");
        assertTest("MT 0x1 valid Stop", resultStop.success);
    }

    // 2. MIDI 1.0 Channel Voice (MT 0x2)
    {
        ValidationResult result = UmpParser::validateAndExtractWords("20904000");
        assertTest("MIDI 1.0 CV valid", result.success && result.extractedMessages.size() == 1);
        if (result.success) {
            ParsedUmp parsed = UmpParser::parseMessage(result.extractedMessages[0]);
            assertTest("MIDI 1.0 CV MT", parsed.messageType == 0x2);
            assertTest("MIDI 1.0 CV Size", parsed.sizeBits == 32);
        }
    }

    // 2. MIDI 2.0 Channel Voice (MT 0x4)
    {
        ValidationResult result = UmpParser::validateAndExtractWords("40904000 40000000");
        assertTest("MIDI 2.0 CV valid", result.success && result.extractedMessages.size() == 1);
        if (result.success) {
            ParsedUmp parsed = UmpParser::parseMessage(result.extractedMessages[0]);
            assertTest("MIDI 2.0 CV MT", parsed.messageType == 0x4);
            assertTest("MIDI 2.0 CV Size", parsed.sizeBits == 64);
        }
    }

    // 3. SysEx7 (MT 0x3)
    {
        ValidationResult result = UmpParser::validateAndExtractWords("30040000 00000000");
        assertTest("SysEx7 valid", result.success && result.extractedMessages.size() == 1);
        if (result.success) {
            ParsedUmp parsed = UmpParser::parseMessage(result.extractedMessages[0]);
            assertTest("SysEx7 MT", parsed.messageType == 0x3);
            assertTest("SysEx7 Form", parsed.description.contains("Form: Complete"));
        }
    }

    // 4. SysEx8 (MT 0x5)
    {
        ValidationResult result = UmpParser::validateAndExtractWords("501D0000 00000000 00000000 00000000");
        assertTest("SysEx8 valid", result.success && result.extractedMessages.size() == 1);
        if (result.success) {
            ParsedUmp parsed = UmpParser::parseMessage(result.extractedMessages[0]);
            assertTest("SysEx8 MT", parsed.messageType == 0x5);
            assertTest("SysEx8 StreamID", parsed.description.contains("StreamID: 0x00"));
        }
    }

    // 5. MDS Header (MT 0x5, Form 8)
    {
        ValidationResult result = UmpParser::validateAndExtractWords("508E0000 00000000 00000000 00000000");
        assertTest("MDS Header valid", result.success && result.extractedMessages.size() == 1);
        if (result.success) {
            ParsedUmp parsed = UmpParser::parseMessage(result.extractedMessages[0]);
            assertTest("MDS Header Name", parsed.description.contains("MDS Header"));
        }
    }

    // 6. Flex Data (MT 0xD)
    {
        ValidationResult result = UmpParser::validateAndExtractWords("D0000101 41000000 00000000 00000000");
        assertTest("Flex Data valid", result.success && result.extractedMessages.size() == 1);
        if (result.success) {
            ParsedUmp parsed = UmpParser::parseMessage(result.extractedMessages[0]);
            assertTest("Flex Data MT", parsed.messageType == 0xD);
        }
    }

    // 7. UMP Stream Endpoint Info (MT 0xF)
    {
        ValidationResult result = UmpParser::validateAndExtractWords("F0000000 00000000 00000000 00000000");
        assertTest("UMP Stream valid", result.success && result.extractedMessages.size() == 1);
        if (result.success) {
            ParsedUmp parsed = UmpParser::parseMessage(result.extractedMessages[0]);
            assertTest("UMP Stream MT", parsed.messageType == 0xF);
        }
    }

    // 8. Product Instance ID (MT 0xF Status 0x004)
    {
        ValidationResult result = UmpParser::validateAndExtractWords("F0040000 41424300 00000000 00000000");
        assertTest("Product Instance ID valid", result.success && result.extractedMessages.size() == 1);
        if (result.success) {
            ParsedUmp parsed = UmpParser::parseMessage(result.extractedMessages[0]);
            assertTest("Product Instance ID Desc", parsed.description.contains("Product Instance Id"));
        }
    }

    // 9. Entrada com caractere inválido
    {
        ValidationResult result = UmpParser::validateAndExtractWords("20904X00");
        assertTest("Invalid Character rejected", !result.success && result.errorType == UmpValidationError::InvalidCharacter);
    }

    // 10. Entrada com word incompleta (truncada impar)
    {
        ValidationResult result = UmpParser::validateAndExtractWords("2090400"); // 7 chars, faltou 1 nibble
        assertTest("Incomplete word rejected", !result.success && result.errorType == UmpValidationError::IncompleteWord);
    }

    // 11. Entrada com pacote UMP incompleto (MT mentiroso)
    {
        ValidationResult result = UmpParser::validateAndExtractWords("40904000"); // MT 0x4 exige 2 palavras, mas fornecemos apenas 1
        assertTest("Incomplete packet rejected", !result.success && result.errorType == UmpValidationError::IncompletePacket);
    }

    // 12. Entrada vazia
    {
        ValidationResult result = UmpParser::validateAndExtractWords("");
        assertTest("Empty input rejected safely", !result.success && result.errorType == UmpValidationError::EmptyAfterFormatting);
    }

    // 13. Linha de comentario no comeco e meio
    {
        ValidationResult result = UmpParser::validateAndExtractWords("# Comment\n20904000\n# Another");
        assertTest("Comment lines ignored", result.success && result.extractedMessages.size() == 1);
    }

    // 14. Comentario inline
    {
        ValidationResult result = UmpParser::validateAndExtractWords("20904000 # Inline comment with X");
        assertTest("Inline comment ignored", result.success && result.extractedMessages.size() == 1);
    }

    // 15. Caractere invalido real fora de comentario continua sendo pego
    {
        ValidationResult result = UmpParser::validateAndExtractWords("2090400Z # Comentario com Z");
        assertTest("Invalid character outside comment fails", !result.success && result.errorType == UmpValidationError::InvalidCharacter);
    }

    std::cout << "\nStarting Midi1LiveDecoder Tests\n" << std::endl;

    // Decoder 1. Note Off
    {
        QString res = Midi1LiveDecoder::decode({0x80, 0x3C, 0x40}); // Ch 1, Note 60, Vel 64
        assertTest("Decoder Note Off", res == "Note Off [Ch 1] Note: 60, Vel: 64");
    }

    // Decoder 2. Note On
    {
        QString res = Midi1LiveDecoder::decode({0x91, 0x3D, 0x7F}); // Ch 2, Note 61, Vel 127
        assertTest("Decoder Note On", res == "Note On [Ch 2] Note: 61, Vel: 127");
    }

    // Decoder 3. Note On with vel 0 -> Note Off
    {
        QString res = Midi1LiveDecoder::decode({0x92, 0x3E, 0x00}); // Ch 3, Note 62, Vel 0
        assertTest("Decoder Note On Vel 0", res == "Note On (vel 0 / Note Off) [Ch 3] Note: 62");
    }

    // Decoder 4. Control Change
    {
        QString res = Midi1LiveDecoder::decode({0xB0, 0x07, 0x7F}); // Ch 1, Vol(7), Val 127
        assertTest("Decoder CC", res == "Control Change [Ch 1] CC: 7, Val: 127");
    }

    // Decoder 5. System/RealTime message
    {
        QString res = Midi1LiveDecoder::decode({0xF8}); // Timing Clock
        assertTest("Decoder System Real-Time", res == "System/Common/Real-Time (parcial/bruto)");
    }

    std::cout << "\nStarting Midi1ToUmpPreviewConverter Tests\n" << std::endl;

    // Converter 1. Note On 90 3C 7F -> 20903C7F
    {
        Midi1ToUmpPreviewResult res = Midi1ToUmpPreviewConverter::convert({0x90, 0x3C, 0x7F});
        assertTest("Converter Note On", res.supported && res.umpHex == "20903C7F");
    }

    // Converter 2. Note Off 80 3C 40 -> 20803C40
    {
        Midi1ToUmpPreviewResult res = Midi1ToUmpPreviewConverter::convert({0x80, 0x3C, 0x40});
        assertTest("Converter Note Off", res.supported && res.umpHex == "20803C40");
    }

    // Converter 3. Control Change B0 07 64 -> 20B00764
    {
        Midi1ToUmpPreviewResult res = Midi1ToUmpPreviewConverter::convert({0xB0, 0x07, 0x64});
        assertTest("Converter CC", res.supported && res.umpHex == "20B00764");
    }

    // Converter 4. Program Change C0 05 -> 20C00500
    {
        Midi1ToUmpPreviewResult res = Midi1ToUmpPreviewConverter::convert({0xC0, 0x05});
        assertTest("Converter Program Change", res.supported && res.umpHex == "20C00500");
    }

    // Converter 5. Pitch Bend E0 00 40 -> 20E00040
    {
        Midi1ToUmpPreviewResult res = Midi1ToUmpPreviewConverter::convert({0xE0, 0x00, 0x40});
        assertTest("Converter Pitch Bend", res.supported && res.umpHex == "20E00040");
    }

    // Converter 6. System Real-Time F8 -> unsupported
    {
        Midi1ToUmpPreviewResult res = Midi1ToUmpPreviewConverter::convert({0xF8});
        assertTest("Converter System RT Unsupported", !res.supported);
    }


    std::cout << "\nStarting FakeUmpInputBackend Tests\n" << std::endl;

    {
        FakeUmpInputBackend fakeBackend;
        
        // 1. Name Check
        assertTest("FakeUmpBackend Name", fakeBackend.backendName() == "Fake UMP Backend");
        
        // 2. Ports listing
        QStringList ports = fakeBackend.listInputPorts();
        assertTest("FakeUmpBackend Lists Port", ports.size() == 1 && ports[0] == "Fake UMP Port 1");
        
        // 3. Open invalid port
        assertTest("FakeUmpBackend Open Invalid Port", !fakeBackend.openInputPort(999));
        
        // 4. Default state
        assertTest("FakeUmpBackend Initially Closed", !fakeBackend.isOpen());
        assertTest("FakeUmpBackend Empty when Closed", fakeBackend.pollUmpEvents().empty());
        
        // 5. Open valid port
        assertTest("FakeUmpBackend Open Valid Port", fakeBackend.openInputPort(0));
        assertTest("FakeUmpBackend is Open", fakeBackend.isOpen());
        
        // 6. Polling UMP words
        std::vector<UmpRawEvent> events = fakeBackend.pollUmpEvents();
        assertTest("FakeUmpBackend Poll Event Array Size", events.size() == 1);
        if (!events.empty()) {
            assertTest("FakeUmpBackend Poll Word MT 0x2", events[0].umpWords.size() == 1 && events[0].umpWords[0] == 0x20903C7F);
        }
        
        // 7. Closing port
        fakeBackend.closeInputPort();
        assertTest("FakeUmpBackend Closes Successfully", !fakeBackend.isOpen());
        assertTest("FakeUmpBackend Empty after Close", fakeBackend.pollUmpEvents().empty());
    }

    runWindowsMidiServicesBackendTests();

    std::cout << "\nResults: " << testsPassed << " / " << testsRun << " passed." << std::endl;

    if (testsPassed == testsRun) {
        std::cout << "SUCCESS: All tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << "ERROR: Some tests failed!" << std::endl;
        return 1;
    }
}
