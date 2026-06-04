#pragma once
#include <vector>
#include <cstdint>
#include <QString>
#include "InputSourceType.h"

struct UmpRawEvent {
    InputSourceType sourceType = InputSourceType::LiveUmpWords;
    double timestampMs = 0.0;
    std::vector<uint32_t> umpWords;
    QString backendName;
    QString portName;
};
