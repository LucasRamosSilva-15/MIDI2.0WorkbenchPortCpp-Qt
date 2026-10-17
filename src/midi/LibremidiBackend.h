#pragma once

#include "IUmpInputBackend.h"
#include <QString>
#include <QStringList>
#include <vector>
#include <mutex>
#include <memory>

#include <libremidi/libremidi.hpp>

class LibremidiBackend : public IUmpInputBackend {
public:
    LibremidiBackend();
    ~LibremidiBackend() override;

    QString backendName() const override;
    QStringList listInputPorts() override;

    bool openInputPort(int portIndex) override;
    void closeInputPort() override;
    bool isOpen() const override;

    std::vector<UmpRawEvent> pollUmpEvents() override;

    static QString getPortDisplayName(const libremidi::input_port& port, int fallbackIndex = 0);
    static uint32_t convertMidi1BytesToUmpWord(const uint8_t* data, size_t size, uint8_t group = 0);
    static uint32_t convertMidi1BytesToUmpWord(const std::vector<uint8_t>& bytes, uint8_t group = 0) {
        return convertMidi1BytesToUmpWord(bytes.data(), bytes.size(), group);
    }

private:
    QStringList listInputPortsLocked();

    mutable std::mutex m_mutex;
    bool m_isOpen{false};
    QString m_openedPortName;
    std::vector<libremidi::input_port> m_cachedPorts;
    std::unique_ptr<libremidi::midi_in> m_midiIn;
    std::vector<UmpRawEvent> m_eventQueue;
};
