#include <iostream>
#include <vector>
#include <string>
#include <QString>
#include "../src/UmpParser.h"

int testsRun = 0;
int testsPassed = 0;

void assertTest(const std::string& testName, bool condition) {
    testsRun++;
    if (condition) {
        std::cout << "[PASS] " << testName << std::endl;
        testsPassed++;
    } else {
        std::cout << "[FAIL] " << testName << std::endl;
    }
}

int main() {
    std::cout << "Starting UMP Parser Tests (v0.8.0)\n" << std::endl;

    // 1. MIDI 1.0 Channel Voice (MT 0x2)
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
        assertTest("Invalid Character rejected", !result.success && result.errorMessage.contains("inválido"));
    }

    // 10. Entrada com word incompleta (truncada impar)
    {
        ValidationResult result = UmpParser::validateAndExtractWords("2090400");
        assertTest("Incomplete word rejected", !result.success && result.errorMessage.contains("sobra"));
    }

    // 11. Entrada com pacote UMP incompleto (MT mentiroso)
    {
        ValidationResult result = UmpParser::validateAndExtractWords("40904000"); // MT 4 needs 64 bits (2 words)
        assertTest("Incomplete packet rejected", !result.success && result.errorMessage.contains("Pacote Incompleto"));
    }

    // 12. Entrada vazia
    {
        ValidationResult result = UmpParser::validateAndExtractWords("");
        assertTest("Empty input rejected safely", !result.success && result.errorMessage.contains("vazio"));
    }

    std::cout << "\nResults: " << testsPassed << " / " << testsRun << " passed." << std::endl;

    if (testsPassed == testsRun) {
        std::cout << "SUCCESS: All tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << "ERROR: Some tests failed!" << std::endl;
        return 1;
    }
}
