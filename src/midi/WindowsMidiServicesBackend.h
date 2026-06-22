#pragma once

#include "IUmpInputBackend.h"
#include <QStringList>
#include <vector>
#include <mutex>

/**
 * @brief Skeleton para o futuro Backend Nativo do Windows MIDI Services.
 *
 * (v4.1.0) Esta classe é apenas uma carcaça arquitetural provando a escalabilidade do IUmpInputBackend.
 * Ela não contém as diretrizes #include <winrt/...> nem escuta portas reais ainda.
 */
#include <mutex>

#if defined(WINDOWS_MIDI_SERVICES_ALLOW_REAL_WINRT_ACTIVATION_ATTEMPT)
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Microsoft.Windows.Devices.Midi2.h>
#endif

class WindowsMidiServicesBackend : public IUmpInputBackend {
public:
    enum class ConnectionState {
        Disconnected,
        Ready,
        Active,
        Error
    };

    WindowsMidiServicesBackend();
    virtual ~WindowsMidiServicesBackend();

    // Implementação da IUmpInputBackend
    QString backendName() const override;
    QStringList listInputPorts() override;
    bool openInputPort(int portIndex) override;
    void closeInputPort() override;
    bool isOpen() const override;
    std::vector<UmpRawEvent> pollUmpEvents() override;
    
    ConnectionState getState() const;
    QString getLastError() const;

private:
    ConnectionState m_state;
    QString m_lastError;
    std::mutex m_stateMutex;
    bool m_isOpen;

    // Buffer FIFO e lock para receber eventos de callbacks assíncronos WinRT
    std::mutex m_mutex;
    std::vector<UmpRawEvent> m_eventBuffer;

    // Stub de pesquisa: simula a listagem real sem engatilhar o SDK ainda.
    QStringList queryAvailableEndpoints() const;

#if defined(WINDOWS_MIDI_SERVICES_ALLOW_REAL_WINRT_ACTIVATION_ATTEMPT)
    winrt::Microsoft::Windows::Devices::Midi2::MidiSession m_session{ nullptr };
    winrt::Microsoft::Windows::Devices::Midi2::MidiEndpointConnection m_endpoint{ nullptr };
    winrt::event_token m_eventToken;
#endif
};
