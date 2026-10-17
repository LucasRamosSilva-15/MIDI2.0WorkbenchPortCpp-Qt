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

private:
    QStringList listInputPortsLocked();
    static QString getPortDisplayName(const libremidi::input_port& port, int fallbackIndex = 0);

    mutable std::mutex m_mutex;
    bool m_isOpen{false};
    QString m_openedPortName;
    std::vector<libremidi::input_port> m_cachedPorts;
    std::unique_ptr<libremidi::midi_in> m_midiIn;
    std::vector<UmpRawEvent> m_eventQueue;
};
