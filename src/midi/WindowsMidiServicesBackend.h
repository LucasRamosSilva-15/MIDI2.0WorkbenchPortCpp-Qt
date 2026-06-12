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
class WindowsMidiServicesBackend : public IUmpInputBackend {
public:
    WindowsMidiServicesBackend();
    virtual ~WindowsMidiServicesBackend();

    // Implementação da IUmpInputBackend
    QString backendName() const override;
    QStringList listInputPorts() override;
    bool openInputPort(int portIndex) override;
    void closeInputPort() override;
    bool isOpen() const override;
    std::vector<UmpRawEvent> pollUmpEvents() override;

private:
    bool m_isOpen;
    std::mutex m_mutex;
    // Futuro: ponteiros nativos de IMidiEndpointConnection, sessões, etc.
};
