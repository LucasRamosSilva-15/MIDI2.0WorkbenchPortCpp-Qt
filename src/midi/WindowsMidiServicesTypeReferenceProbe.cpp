#include "WindowsMidiServicesTypeReferenceProbe.h"

#if defined(WINDOWS_MIDI_SERVICES_TYPE_REFERENCE_EXPERIMENT_COMPILED)

#if defined(WINDOWS_MIDI_SERVICES_ALLOW_REAL_CPPWINRT_INCLUDE_ATTEMPT)

#if __has_include(<winrt/Microsoft.Windows.Devices.Midi2.h>)
#include <winrt/Microsoft.Windows.Devices.Midi2.h>
#define MIDIUMP_HAS_WINRT_MICROSOFT_MIDI2_HEADER 1
#elif __has_include(<Microsoft.Windows.Devices.Midi2.h>)
#include <Microsoft.Windows.Devices.Midi2.h>
#define MIDIUMP_HAS_MICROSOFT_MIDI2_HEADER 1
#elif __has_include(<winrt/Windows.Devices.Midi2.h>)
#include <winrt/Windows.Devices.Midi2.h>
#define MIDIUMP_HAS_WINRT_MIDI2_HEADER 1
#elif __has_include(<Windows.Devices.Midi2.h>)
#include <Windows.Devices.Midi2.h>
#define MIDIUMP_HAS_MIDI2_HEADER 1
#endif

namespace {
    constexpr bool compileOnlyTypeReferenceProbe()
    {
#if defined(MIDIUMP_HAS_WINRT_MICROSOFT_MIDI2_HEADER)
        return true;
#elif defined(MIDIUMP_HAS_MICROSOFT_MIDI2_HEADER)
        return true;
#elif defined(MIDIUMP_HAS_WINRT_MIDI2_HEADER)
        return true;
#elif defined(MIDIUMP_HAS_MIDI2_HEADER)
        return true;
#else
        return false;
#endif
    }
}

#endif // WINDOWS_MIDI_SERVICES_ALLOW_REAL_CPPWINRT_INCLUDE_ATTEMPT

#endif

QString WindowsMidiServicesTypeReferenceProbe::typeReferenceExperimentStatus()
{
#if defined(WINDOWS_MIDI_SERVICES_TYPE_REFERENCE_EXPERIMENT_COMPILED)
    #if defined(WINDOWS_MIDI_SERVICES_ALLOW_REAL_CPPWINRT_INCLUDE_ATTEMPT)
        return QStringLiteral("Type reference compile experiment completed with REAL CPPWINRT ATTEMPT. This is a local diagnostic mode.");
    #else
        return QStringLiteral("Type reference experiment is blocked pending C++/WinRT projection alignment. No real include attempt is made in safe mode.");
    #endif
#else
    return QStringLiteral("Windows MIDI Services type reference experiment not compiled.");
#endif
}
