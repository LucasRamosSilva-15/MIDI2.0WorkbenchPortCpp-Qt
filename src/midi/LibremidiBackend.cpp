#include "LibremidiBackend.h"
#include "../UmpParser.h"
#include <chrono>

LibremidiBackend::LibremidiBackend() = default;

LibremidiBackend::~LibremidiBackend() {
    closeInputPort();
}

QString LibremidiBackend::backendName() const {
    return "libremidi (Cross-Platform UMP)";
}

QString LibremidiBackend::getPortDisplayName(const libremidi::input_port& port, int fallbackIndex) {
    QString displayName = QString::fromStdString(port.display_name).trimmed();
    QString portName = QString::fromStdString(port.port_name).trimmed();
    QString devName = QString::fromStdString(port.device_name).trimmed();

    QString baseName;
    if (!displayName.isEmpty() && displayName.compare("input", Qt::CaseInsensitive) != 0 && displayName.compare("capture", Qt::CaseInsensitive) != 0) {
        baseName = displayName;
    } else if (!portName.isEmpty() && portName.compare("input", Qt::CaseInsensitive) != 0 && portName.compare("capture", Qt::CaseInsensitive) != 0) {
        baseName = portName;
    } else if (!devName.isEmpty()) {
        if (!displayName.isEmpty()) {
            baseName = devName + " (" + displayName + ")";
        } else if (!portName.isEmpty()) {
            baseName = devName + " (" + portName + ")";
        } else {
            baseName = devName;
        }
    } else if (!displayName.isEmpty()) {
        baseName = displayName;
    } else if (!portName.isEmpty()) {
        baseName = portName;
    } else {
        baseName = QString("Port %1").arg(fallbackIndex + 1);
    }

    QString prefix = libremidi::is_midi2(port.api) ? "[UMP] " : "[MIDI 1.0] ";
    return prefix + baseName;
}

uint32_t LibremidiBackend::convertMidi1BytesToUmpWord(const uint8_t* data, size_t size, uint8_t group) {
    if (!data || size == 0) return 0;

    uint8_t status = data[0];
    uint8_t d1 = size > 1 ? data[1] : 0;
    uint8_t d2 = size > 2 ? data[2] : 0;
    uint8_t grp = group & 0x0F;

    if (status >= 0x80 && status <= 0xEF) {
        // MIDI 1.0 Channel Voice -> UMP MT 0x2
        return 0x20000000 | (static_cast<uint32_t>(grp) << 24) |
               (static_cast<uint32_t>(status) << 16) |
               (static_cast<uint32_t>(d1) << 8) |
               static_cast<uint32_t>(d2);
    } else if (status >= 0xF0) {
        // System Common / Real-Time -> UMP MT 0x1
        return 0x10000000 | (static_cast<uint32_t>(grp) << 24) |
               (static_cast<uint32_t>(status) << 16) |
               (static_cast<uint32_t>(d1) << 8) |
               static_cast<uint32_t>(d2);
    }

    return 0;
}

QStringList LibremidiBackend::listInputPorts() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return listInputPortsLocked();
}

QStringList LibremidiBackend::listInputPortsLocked() {
    m_cachedPorts.clear();
    QStringList result;

    libremidi::observer_configuration obs_conf;
    obs_conf.track_hardware = true;
    obs_conf.track_virtual = true;
    obs_conf.track_network = true;
    obs_conf.track_any = true;

    auto addPortsFromObserver = [&](libremidi::observer& obs) {
        auto ports = obs.get_input_ports();
        for (auto&& p : ports) {
            QString name = getPortDisplayName(p, static_cast<int>(m_cachedPorts.size()));
            bool duplicate = false;
            for (size_t i = 0; i < m_cachedPorts.size(); ++i) {
                if (result[static_cast<int>(i)] == name) {
                    duplicate = true;
                    break;
                }
            }
            if (!duplicate) {
                m_cachedPorts.push_back(std::move(p));
                result.append(name);
            }
        }
    };

    // 1. Gather all compiled native UMP APIs (e.g. alsa_seq_ump, pipewire_ump, coremidi_ump, winmidi)
    for (auto api : libremidi::available_ump_apis()) {
        try {
            libremidi::observer obs{obs_conf, libremidi::observer_configuration_for(api)};
            addPortsFromObserver(obs);
        } catch (...) {}
    }

    // 2. Gather default and standard MIDI 1.0 APIs (e.g. alsa_seq, pipewire, winmm, coremidi)
    for (auto api : libremidi::available_apis()) {
        // Skip raw character device layer when sequencer is available to avoid duplicates
        if (api == libremidi::API::ALSA_RAW) continue;
        try {
            libremidi::observer obs{obs_conf, libremidi::observer_configuration_for(api)};
            addPortsFromObserver(obs);
        } catch (...) {}
    }

    // 3. Fallback to default observer if no ports were discovered
    if (m_cachedPorts.empty()) {
        try {
            libremidi::observer obs{obs_conf};
            addPortsFromObserver(obs);
        } catch (...) {}
    }

    return result;
}

bool LibremidiBackend::openInputPort(int portIndex) {
    closeInputPort();

    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_cachedPorts.empty()) {
        listInputPortsLocked();
    }

    if (portIndex < 0 || static_cast<size_t>(portIndex) >= m_cachedPorts.size()) {
        return false;
    }

    const auto& port = m_cachedPorts[portIndex];
    QString portDisplayName = getPortDisplayName(port, portIndex);

    try {
        std::unique_ptr<libremidi::midi_in> midiIn;

        if (libremidi::is_midi2(port.api)) {
            // Native UMP / MIDI 2.0 configuration
            libremidi::ump_input_configuration conf;
            conf.on_message = [this](libremidi::ump&& msg) {
                int count = UmpParser::getWordCountForMessageType((msg.data[0] >> 28) & 0x0F);
                UmpRawEvent ev;
                ev.sourceType = InputSourceType::LiveUmpWords;
                if (msg.timestamp > 0) {
                    ev.timestampMs = static_cast<double>(msg.timestamp) / 1'000'000.0;
                } else {
                    static auto startTime = std::chrono::steady_clock::now();
                    auto now = std::chrono::steady_clock::now();
                    ev.timestampMs = std::chrono::duration<double, std::milli>(now - startTime).count();
                }
                ev.umpWords.reserve(count);
                for (int i = 0; i < count; ++i) {
                    ev.umpWords.push_back(msg.data[i]);
                }
                ev.backendName = backendName();

                std::lock_guard<std::mutex> lk(m_mutex);
                ev.portName = m_openedPortName;
                m_eventQueue.push_back(std::move(ev));
            };

            try {
                midiIn = std::make_unique<libremidi::midi_in>(conf, libremidi::midi_in_configuration_for(port.api));
            } catch (...) {
                try {
                    midiIn = std::make_unique<libremidi::midi_in>(conf, libremidi::midi2::in_default_configuration());
                } catch (...) {
                    midiIn = std::make_unique<libremidi::midi_in>(conf);
                }
            }
        } else {
            // Standard / Legacy MIDI 1.0 configuration: converts bytes into UMP Word MT 0x2 / MT 0x1
            libremidi::input_configuration conf;
            conf.on_message = [this](libremidi::message&& msg) {
                uint32_t umpWord = convertMidi1BytesToUmpWord(msg.bytes.data(), msg.bytes.size(), 0);
                if (umpWord == 0) return;

                UmpRawEvent ev;
                ev.sourceType = InputSourceType::LiveUmpWords;
                if (msg.timestamp > 0) {
                    ev.timestampMs = static_cast<double>(msg.timestamp) / 1'000'000.0;
                } else {
                    static auto startTime = std::chrono::steady_clock::now();
                    auto now = std::chrono::steady_clock::now();
                    ev.timestampMs = std::chrono::duration<double, std::milli>(now - startTime).count();
                }
                ev.umpWords = { umpWord };
                ev.backendName = backendName();

                std::lock_guard<std::mutex> lk(m_mutex);
                ev.portName = m_openedPortName;
                m_eventQueue.push_back(std::move(ev));
            };

            try {
                midiIn = std::make_unique<libremidi::midi_in>(conf, libremidi::midi_in_configuration_for(port.api));
            } catch (...) {
                try {
                    midiIn = std::make_unique<libremidi::midi_in>(conf, libremidi::midi1::in_default_configuration());
                } catch (...) {
                    midiIn = std::make_unique<libremidi::midi_in>(conf);
                }
            }
        }

        auto err = midiIn->open_port(port);
        if (err != stdx::error{}) {
            return false;
        }

        m_midiIn = std::move(midiIn);
        m_isOpen = true;
        m_openedPortName = portDisplayName;
        return true;
    } catch (...) {
        return false;
    }
}

void LibremidiBackend::closeInputPort() {
    std::unique_ptr<libremidi::midi_in> oldMidiIn;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_isOpen = false;
        m_openedPortName.clear();
        oldMidiIn = std::move(m_midiIn);
    }
    if (oldMidiIn) {
        try {
            if (oldMidiIn->is_port_open()) {
                oldMidiIn->close_port();
            }
        } catch (...) {}
        oldMidiIn.reset();
    }
}

bool LibremidiBackend::isOpen() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_isOpen && m_midiIn && m_midiIn->is_port_open();
}

std::vector<UmpRawEvent> LibremidiBackend::pollUmpEvents() {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<UmpRawEvent> events = std::move(m_eventQueue);
    m_eventQueue.clear();
    return events;
}
