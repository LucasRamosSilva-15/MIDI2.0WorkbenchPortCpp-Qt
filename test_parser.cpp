#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <QString>
#include "UmpParser.h"

using namespace std;

void testUmp(const vector<uint32_t>& words) {
    int expected = UmpParser::getWordCountForMessageType((words[0] >> 28) & 0xF);
    ParsedUmp result = UmpParser::parseMessage(words);
    
    cout << "Entrada (Hex): ";
    for (uint32_t w : words) {
        cout << hex << uppercase << setfill('0') << setw(8) << w << " ";
    }
    cout << "\n";
    cout << "Esperado/Size: " << dec << expected << " palavras (" << result.sizeBits << " bits)\n";
    cout << "Type: " << result.messageType << " | Group: " << result.group << "\n";
    cout << "Descricao: " << result.description.toStdString() << "\n";
    cout << "----------------------------------------\n";
}

int main() {
    testUmp({0x20904000});
    testUmp({0x20804000});
    testUmp({0x23B0077F});
    testUmp({0x40904000, 0x40000000});
    testUmp({0x50000000, 0x00000000, 0x00000000, 0x00000000});
    testUmp({0xD0000000, 0x00000000, 0x00000000, 0x00000000});
    testUmp({0xF0000000, 0x00000000, 0x00000000, 0x00000000});
    
    return 0;
}
