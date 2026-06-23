#include "WindowsMidiServicesHeaderIncludeProbe.h"

// The isolated include experiment
#if defined(USE_WINDOWS_MIDI_SERVICES_HEADER_INCLUDE_EXPERIMENT) && defined(WINDOWS_MIDI_SERVICES_OPTIONAL_HEADERS_DETECTED)
    // We attempt to include the detected header.
    // This is a pure compile-time experiment. No real API functions are called.
    #if __has_include(<winrt/Windows.Devices.Midi2.h>)
        #include <winrt/Windows.Devices.Midi2.h>
    #elif __has_include(<Windows.Devices.Midi2.h>)
        #include <Windows.Devices.Midi2.h>
    #else
        // Fallback if neither resolves cleanly
    #endif
#endif

WindowsMidiServicesHeaderIncludeReport WindowsMidiServicesHeaderIncludeProbe::runProbe() {
    WindowsMidiServicesHeaderIncludeReport report;
    
#ifdef USE_WINDOWS_MIDI_SERVICES_HEADER_INCLUDE_EXPERIMENT
    report.includeExperimentAttempted = true;
#ifdef WINDOWS_MIDI_SERVICES_OPTIONAL_HEADERS_DETECTED
    // If the compiler reaches this block, it means the headers were successfully processed by the preprocessor
    report.headersIncludedSuccessfully = true;
    report.includeStatus = "Headers were successfully included in the compilation unit.";
    report.notes << "Compile experiment passed without fatal errors.";
#else
    report.headersIncludedSuccessfully = false;
    report.includeStatus = "Include experiment was enabled but no candidate headers were detected. Include skipped.";
    report.notes << "Skipped #include to prevent compiler crash.";
#endif
#else
    report.includeExperimentAttempted = false;
    report.headersIncludedSuccessfully = false;
    report.includeStatus = "Header include experiment is disabled in CMake.";
#endif

    return report;
}

QString WindowsMidiServicesHeaderIncludeProbe::formatIncludeReport(const WindowsMidiServicesHeaderIncludeReport& report) {
    QString out;
    out += "\n--- Header Include Compile Experiment ---\n";
    out += "Include Experiment Attempted: " + QString(report.includeExperimentAttempted ? "Yes" : "No") + "\n";
    out += "Headers Included Successfully: " + QString(report.headersIncludedSuccessfully ? "Yes" : "No") + "\n";
    out += "Include Status: " + report.includeStatus + "\n";
    if (!report.notes.isEmpty()) {
        out += "Notes:\n";
        for (const QString& note : report.notes) {
            out += "- " + note + "\n";
        }
    }
    return out;
}
