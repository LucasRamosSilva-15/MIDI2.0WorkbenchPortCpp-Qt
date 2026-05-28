#include "RtMidiInputBackend.h"

#ifdef USE_RTMIDI
#include <RtMidi.h>
#endif

RtMidiInputBackend::RtMidiInputBackend() : m_midiIn(nullptr), m_isOpen(false) {
#ifdef USE_RTMIDI
    try {
        m_midiIn = new RtMidiIn();
    } catch (...) {
        m_midiIn = nullptr;
    }
#endif
}

#ifdef USE_RTMIDI
void RtMidiInputBackend::midiCallback(double timeStamp, std::vector<unsigned char> *message, void *userData) {
    if (!message || message->empty() || !userData) return;
    
    RtMidiInputBackend* backend = static_cast<RtMidiInputBackend*>(userData);
    
    MidiRawEvent ev;
    ev.sourceType = InputSourceType::LiveMidi1Bytes;
    ev.timestamp = timeStamp; 
    ev.midi1Bytes = *message;
    
    std::lock_guard<std::mutex> lock(backend->m_queueMutex);
    backend->m_eventQueue.push_back(ev);
}
#endif

RtMidiInputBackend::~RtMidiInputBackend() {
    closeInputPort();
#ifdef USE_RTMIDI
    if (m_midiIn) {
        delete static_cast<RtMidiIn*>(m_midiIn);
        m_midiIn = nullptr;
    }
#endif
}

QString RtMidiInputBackend::backendName() const {
#ifdef USE_RTMIDI
    return "RtMidi (Real Hardware)";
#else
    return "RtMidi (Compilado sem suporte)";
#endif
}

QStringList RtMidiInputBackend::listInputPorts() {
    QStringList ports;
#ifdef USE_RTMIDI
    if (!m_midiIn) return ports;
    RtMidiIn* midi = static_cast<RtMidiIn*>(m_midiIn);
    unsigned int nPorts = midi->getPortCount();
    for (unsigned int i = 0; i < nPorts; i++) {
        try {
            ports << QString::fromStdString(midi->getPortName(i));
        } catch (...) {
            ports << QString("Port %1").arg(i);
        }
    }
#endif
    return ports;
}

bool RtMidiInputBackend::openInputPort(int portIndex) {
#ifdef USE_RTMIDI
    if (!m_midiIn) return false;
    RtMidiIn* midi = static_cast<RtMidiIn*>(m_midiIn);
    try {
        midi->openPort(portIndex);
        midi->ignoreTypes(false, false, false);
        midi->setCallback(&RtMidiInputBackend::midiCallback, this);
        m_isOpen = true;
        return true;
    } catch (...) {
        return false;
    }
#else
    (void)portIndex;
    return false;
#endif
}

void RtMidiInputBackend::closeInputPort() {
#ifdef USE_RTMIDI
    if (m_midiIn && m_isOpen) {
        RtMidiIn* midi = static_cast<RtMidiIn*>(m_midiIn);
        midi->cancelCallback();
        midi->closePort();
        m_isOpen = false;
        
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_eventQueue.clear();
    }
#endif
}

bool RtMidiInputBackend::isOpen() const {
    return m_isOpen;
}

std::vector<MidiRawEvent> RtMidiInputBackend::pollEvents() {
    std::vector<MidiRawEvent> events;
    std::lock_guard<std::mutex> lock(m_queueMutex);
    std::swap(events, m_eventQueue);
    return events;
}
