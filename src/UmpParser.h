#pragma once
#include <QString>
#include <vector>
#include <cstdint>

struct ParsedUmp {
    std::vector<uint32_t> words;
    int sizeBits = 0;
    int messageType = 0;
    int group = 0;
    int status = -1;
    int channel = -1;
    QString description;
};

class UmpParser {
public:
    static int getWordCountForMessageType(int mt);
    static ParsedUmp parseMessage(const std::vector<uint32_t>& words);
    static QString getMessageTypeString(int mt);
};
