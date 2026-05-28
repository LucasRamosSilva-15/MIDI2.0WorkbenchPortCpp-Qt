#pragma once

#include "IMidiInputBackend.h"
#include <mutex>
#include <vector>

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
#ifdef USE_RTMIDI
    static void midiCallback(double timeStamp, std::vector<unsigned char> *message, void *userData);
#endif

    void* m_midiIn; // Usando void* para não espalhar headers do RtMidi
    bool m_isOpen;

    std::mutex m_queueMutex;
    std::vector<MidiRawEvent> m_eventQueue;
};
