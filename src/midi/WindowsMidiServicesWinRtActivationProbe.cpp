#include "WindowsMidiServicesWinRtActivationProbe.h"

#if defined(WINDOWS_MIDI_SERVICES_WINRT_ACTIVATION_EXPERIMENT_COMPILED)

#if defined(WINDOWS_MIDI_SERVICES_ALLOW_REAL_WINRT_ACTIVATION_ATTEMPT)

// The actual COM/WinRT initialization headers
#include <winrt/base.h>

#endif // WINDOWS_MIDI_SERVICES_ALLOW_REAL_WINRT_ACTIVATION_ATTEMPT

#endif

QString WindowsMidiServicesWinRtActivationProbe::winRtActivationExperimentStatus()
{
#if defined(WINDOWS_MIDI_SERVICES_WINRT_ACTIVATION_EXPERIMENT_COMPILED)
    #if defined(WINDOWS_MIDI_SERVICES_ALLOW_REAL_WINRT_ACTIVATION_ATTEMPT)
        return QStringLiteral("WinRT Activation compile experiment attempted (REAL ACTIVATION ENABLED). This may have initialized COM apartments in the background.");
    #else
        return QStringLiteral("WinRT Activation experiment is blocked. No real init_apartment attempt is made in safe mode.");
    #endif
#else
    return QStringLiteral("Windows MIDI Services WinRT activation experiment not compiled.");
#endif
}

bool WindowsMidiServicesWinRtActivationProbe::initializeWinRtApartment()
{
#if defined(WINDOWS_MIDI_SERVICES_WINRT_ACTIVATION_EXPERIMENT_COMPILED)
    #if defined(WINDOWS_MIDI_SERVICES_ALLOW_REAL_WINRT_ACTIVATION_ATTEMPT)
        try {
            // Attempt to initialize the COM apartment for WinRT
            winrt::init_apartment();
            return true;
        } catch (...) {
            return false;
        }
    #else
        // Safe mode: do nothing.
        return false;
    #endif
#else
    return false;
#endif
}
