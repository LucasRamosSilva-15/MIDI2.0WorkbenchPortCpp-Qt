#pragma once

#include "IUmpInputBackend.h"
#include <QStringList>
#include <QMap>
#include <QThread>
#include <QObject>
#include <vector>
#include <mutex>
#include <memory>

#if defined(WINDOWS_MIDI_SERVICES_ALLOW_REAL_WINRT_ACTIVATION_ATTEMPT)
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Microsoft.Windows.Devices.Midi2.h>
#endif

// Buffer FIFO e lock encapsulados para segurança no ciclo de vida (RAII)
struct SharedBuffer {
    std::mutex mutex;
    std::vector<UmpRawEvent> buffer;
};

// WmsWorker isola completamente a API WinRT numa QThread dedicada em background (MTA)
class WmsWorker : public QObject {
    Q_OBJECT
public:
    explicit WmsWorker(std::shared_ptr<SharedBuffer> sharedBuffer, QObject *parent = nullptr);
    ~WmsWorker();

public slots:
    void doInit();
    void queryAvailableEndpoints();
    void openPort(QString deviceId, QString portName);
    void closePort();

signals:
    void endpointsDiscovered(QStringList names, QMap<QString, QString> idMap);
    void portOpened(bool success, QString message);
    void portClosed();
    void errorReported(QString error);

private:
    std::shared_ptr<SharedBuffer> m_sharedBuffer;
    bool m_mtaInitialized = false;
    QString m_openedPortName;

#if defined(WINDOWS_MIDI_SERVICES_ALLOW_REAL_WINRT_ACTIVATION_ATTEMPT)
    winrt::Microsoft::Windows::Devices::Midi2::MidiSession m_session{ nullptr };
    winrt::Microsoft::Windows::Devices::Midi2::MidiEndpointConnection m_endpoint{ nullptr };
    winrt::event_token m_eventToken;
#endif
};

class WindowsMidiServicesBackend : public QObject, public IUmpInputBackend {
    Q_OBJECT
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

signals:
    // Sinais para despachar comandos para o Worker
    void requestInit();
    void requestEndpoints();
    void requestOpenPort(QString deviceId, QString portName);
    void requestClosePort();

private:
    ConnectionState m_state;
    QString m_lastError;
    std::mutex m_stateMutex;
    bool m_isOpen;

    std::shared_ptr<SharedBuffer> m_sharedBuffer;

    QThread m_workerThread;
    WmsWorker* m_worker;

    // Cache local dos endpoints descobertos
    QStringList m_cachedEndpointNames;
    QMap<QString, QString> m_cachedEndpointMap;
};
