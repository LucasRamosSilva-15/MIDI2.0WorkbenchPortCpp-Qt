#pragma once

#include "IMidiInputBackend.h"

class RtMidiInputBackend : public IMidiInputBackend {
public:
    RtMidiInputBackend();
    ~RtMidiInputBackend() override;

    QString backendName() const override;
    QStringList listInputPorts() override;
    
    bool openInputPort(int portIndex) override;
    void closeInputPort() override;
    bool isOpen() const override;

    std::vector<MidiRawEvent> pollEvents() override;

private:
    void* m_midiIn; // Usando void* para não espalhar headers do RtMidi
    bool m_isOpen;
};
