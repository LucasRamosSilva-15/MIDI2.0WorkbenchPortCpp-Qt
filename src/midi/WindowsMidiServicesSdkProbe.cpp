#include "WindowsMidiServicesSdkProbe.h"

WindowsMidiServicesSdkProbe::WindowsMidiServicesSdkProbe() {
    // Inicialização da casca de aferição
}

bool WindowsMidiServicesSdkProbe::isWindowsMidiServicesSdkExperimentEnabled() const {
#ifdef USE_WINDOWS_MIDI_SERVICES_SDK_EXPERIMENT
    return true;
#else
    return false;
#endif
}

QString WindowsMidiServicesSdkProbe::windowsMidiServicesSdkExperimentStatus() const {
#ifdef USE_WINDOWS_MIDI_SERVICES_SDK_EXPERIMENT
    return "Windows MIDI Services SDK experiment compile flag is enabled, but real SDK probing is not implemented in v4.3.0.";
#else
    return "Windows MIDI Services SDK experiment is disabled.";
#endif
}
