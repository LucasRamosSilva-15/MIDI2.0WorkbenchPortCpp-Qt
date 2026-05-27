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

struct ValidationResult {
    bool success;
    QString errorMessage;
    std::vector<std::vector<uint32_t>> extractedMessages;
};

class UmpParser {
public:
    static int getWordCountForMessageType(int messageType);
    static QString getMessageTypeString(int messageType);
    static ParsedUmp parseMessage(const std::vector<uint32_t>& words);
    static ValidationResult validateAndExtractWords(const QString& hexInput);
};
