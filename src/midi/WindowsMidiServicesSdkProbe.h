#pragma once

#include <QString>

/**
 * @brief (v4.3.0) Probe experimental para detecção teórica do SDK Windows MIDI Services.
 * 
 * Esta classe é estéril. Não embute `<winrt>` nem pacotes C++ da Microsoft por padrão.
 * Sua finalidade é aferir se a flag de compilação foi engatilhada e emitir uma resposta em string segura.
 */
class WindowsMidiServicesSdkProbe {
public:
    WindowsMidiServicesSdkProbe();
    
    // Verifica se a diretiva USE_WINDOWS_MIDI_SERVICES_SDK_EXPERIMENT está ativa
    bool isWindowsMidiServicesSdkExperimentEnabled() const;
    
    // Retorna uma mensagem de status do experimento de build atual
    QString windowsMidiServicesSdkExperimentStatus() const;
};
