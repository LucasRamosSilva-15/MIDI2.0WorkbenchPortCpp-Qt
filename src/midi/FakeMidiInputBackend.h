#pragma once

#include "IMidiInputBackend.h"

class FakeMidiInputBackend : public IMidiInputBackend {
public:
    FakeMidiInputBackend() : m_open(false) {}

    QString backendName() const override { return "FakeMidi"; }
    
    QStringList listInputPorts() override {
        return QStringList() << "Fake Port 1" << "Fake Port 2";
    }
    
    bool openInputPort(int portIndex) override {
        if (portIndex >= 0 && portIndex < 2) {
            m_open = true;
            return true;
        }
        return false;
    }
    
    void closeInputPort() override {
        m_open = false;
    }
    
    bool isOpen() const override {
        return m_open;
    }

    std::vector<MidiRawEvent> pollEvents() override {
        // Num cenário real de teste, injetaríamos eventos aqui.
        return {};
    }

private:
    bool m_open;
};
