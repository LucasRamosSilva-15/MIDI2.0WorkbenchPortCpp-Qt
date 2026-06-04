#include "FakeUmpInputBackend.h"

FakeUmpInputBackend::FakeUmpInputBackend() : m_isOpen(false), m_pollCount(0) {
}

FakeUmpInputBackend::~FakeUmpInputBackend() {
    closeInputPort();
}

QString FakeUmpInputBackend::backendName() const {
    return "Fake UMP Backend";
}

QStringList FakeUmpInputBackend::listInputPorts() {
    return {"Fake UMP Port 1"};
}

bool FakeUmpInputBackend::openInputPort(int portIndex) {
    if (portIndex == 0) {
        m_isOpen = true;
        m_portName = "Fake UMP Port 1";
        m_pollCount = 0;
        return true;
    }
    return false; // Rejeitar indices inexistentes
}

void FakeUmpInputBackend::closeInputPort() {
    m_isOpen = false;
    m_portName.clear();
}

bool FakeUmpInputBackend::isOpen() const {
    return m_isOpen;
}

std::vector<UmpRawEvent> FakeUmpInputBackend::pollUmpEvents() {
    std::vector<UmpRawEvent> events;
    if (!m_isOpen) return events; // Retorna nulo se porta bloqueada

    // Simular recebimento pseudoaleatório de pacotes MT 0x2
    UmpRawEvent ev;
    ev.backendName = backendName();
    ev.portName = m_portName;
    ev.timestampMs = m_pollCount * 100.0;
    ev.sourceType = InputSourceType::LiveUmpWords;

    if (m_pollCount % 5 == 0) {
        ev.umpWords = {0x20903C7F}; // Note On, Ch 1, Note 60, Vel 127
    } else if (m_pollCount % 5 == 1) {
        ev.umpWords = {0x20803C40}; // Note Off, Ch 1, Note 60, Vel 64
    } else if (m_pollCount % 5 == 2) {
        ev.umpWords = {0x20B00764}; // Control Change, CC 7, Val 100
    } else if (m_pollCount % 5 == 3) {
        ev.umpWords = {0x20C00500}; // Program Change, Prog 5
    } else if (m_pollCount % 5 == 4) {
        ev.umpWords = {0x20E00040}; // Pitch Bend, Center
    }
    
    events.push_back(ev);
    m_pollCount++;
    return events;
}
