#pragma once

#include "IUmpInputBackend.h"
#include <memory>
#include <vector>
#include <QStringList>

enum class UmpBackendType {
    FakeUmp,
    WindowsMidiServices
};

class MidiInputController {
public:
    MidiInputController();
    ~MidiInputController();

    void switchBackend(UmpBackendType newType);
    UmpBackendType getActiveBackendType() const;
    
    QString getActiveBackendName() const;
    QStringList getAvailablePorts() const;
    
    bool openPort(int index);
    void closePort();
    bool isPortOpen() const;

    std::vector<UmpRawEvent> pollNewEvents();

private:
    std::unique_ptr<IUmpInputBackend> m_backend;
    UmpBackendType m_activeType;
};
