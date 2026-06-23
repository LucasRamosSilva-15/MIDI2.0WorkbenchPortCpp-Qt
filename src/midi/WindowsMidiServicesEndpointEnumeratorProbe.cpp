#include "WindowsMidiServicesEndpointEnumeratorProbe.h"

#if defined(WINDOWS_MIDI_SERVICES_ENDPOINT_ENUMERATION_ENABLED)
// In a real scenario, this would include the generated C++/WinRT headers
// like #include <winrt/Microsoft.Windows.Devices.Midi2.h>
// and use MidiSession to list endpoints.
#endif

QStringList WindowsMidiServicesEndpointEnumeratorProbe::enumerateEndpoints()
{
    QStringList endpoints;
    
#if defined(WINDOWS_MIDI_SERVICES_ENDPOINT_ENUMERATION_ENABLED)
    // Prototype: Here we would instantiate MidiSession and call
    // the enumeration API. Since this is an isolated probe and we are
    // avoiding the actual generated WinRT header inclusion in Safe Mode,
    // we return a diagnostic string if enabled but without headers.
    // If we included real headers, we would iterate and append real names.
    endpoints.append("Experimental Endpoint Enumeration is ON. (Actual hardware query requires WinRT headers)");
#endif

    return endpoints;
}
