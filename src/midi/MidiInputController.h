#pragma once

#include "IMidiInputBackend.h"
#include <memory>
#include <vector>
#include <QStringList>

class MidiInputController {
public:
    MidiInputController();
    ~MidiInputController();

    void setBackend(std::unique_ptr<IMidiInputBackend> backend);
    
    QString getActiveBackendName() const;
    QStringList getAvailablePorts() const;
    
    bool openPort(int index);
    void closePort();
    bool isPortOpen() const;

    std::vector<MidiRawEvent> pollNewEvents();

private:
    std::unique_ptr<IMidiInputBackend> m_backend;
};
