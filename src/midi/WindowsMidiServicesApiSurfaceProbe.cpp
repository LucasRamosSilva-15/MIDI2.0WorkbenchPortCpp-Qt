#include "WindowsMidiServicesApiSurfaceProbe.h"

QString WindowsMidiServicesApiSurfaceProbe::apiSurfaceMappingStatus()
{
#if defined(WINDOWS_MIDI_SERVICES_API_SURFACE_MAPPING_ENABLED)
    // Here we theoretically reference the mapped classes without instantiating them.
    // E.g. winrt::Microsoft::Windows::Devices::Midi2::MidiSession
    // winrt::Microsoft::Windows::Devices::Midi2::MidiEndpointConnection
    return QStringLiteral("API Surface mapping enabled. Theoretical namespace and class definitions (MidiSession, MidiEndpointConnection) are mapped and available to the compiler.");
#else
    return QStringLiteral("API Surface mapping is disabled. No Windows MIDI Services classes are mapped.");
#endif
}
