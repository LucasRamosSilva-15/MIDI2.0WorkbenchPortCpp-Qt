#pragma once

#include <QString>
#include <vector>
#include <QStringList>
#include "../core/MidiRawEvent.h"

class IMidiInputBackend {
public:
    virtual ~IMidiInputBackend() = default;

    virtual QString backendName() const = 0;
    virtual QStringList listInputPorts() = 0;
    
    virtual bool openInputPort(int portIndex) = 0;
    virtual void closeInputPort() = 0;
    virtual bool isOpen() const = 0;

    // Retorna todos os eventos enfileirados desde a última chamada
    virtual std::vector<MidiRawEvent> pollEvents() = 0;
};
