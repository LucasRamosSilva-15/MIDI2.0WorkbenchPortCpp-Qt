#include "WindowsMidiServicesSdkProbe.h"

bool WindowsMidiServicesSdkProbe::isSdkExperimentEnabled() {
#ifdef USE_WINDOWS_MIDI_SERVICES_SDK_EXPERIMENT
    return true;
#else
    return false;
#endif
}

WindowsMidiServicesSdkDetectionReport WindowsMidiServicesSdkProbe::buildDetectionReport() {
    WindowsMidiServicesSdkDetectionReport report;
    
    report.experimentCompileFlagEnabled = isSdkExperimentEnabled();
    report.realSdkHeadersUsed = false;
    report.realEndpointListingAvailable = false;
    report.realUmpCaptureAvailable = false;
    
    report.optionalPackageDetectionAttempted = false;
    report.optionalPackageDetected = false;
    report.optionalHeaderDetectionAttempted = false;
    report.optionalHeadersDetected = false;
    
    report.packageDetectionStatus = "Optional package detection is documented but not performed in v4.5.0.";
    report.headerDetectionStatus = "Optional header detection is documented but not performed in v4.5.0.";
    
    if (report.experimentCompileFlagEnabled) {
        report.compileMode = "SDK experiment compile flag enabled";
        report.status = "SDK experiment flag is enabled, but real Windows MIDI Services SDK probing is not implemented in v4.4.0.";
    } else {
        report.compileMode = "SDK experiment disabled";
        report.status = "SDK experiment is disabled. Normal build path does not require Windows MIDI Services SDK.";
    }
    
    report.notes << "No real Windows MIDI Services SDK headers are included in this build."
                 << "No Microsoft.Windows.Devices.Midi2 package is required in this version."
                 << "No C++/WinRT projection is required in this version."
                 << "Endpoint listing is still deferred."
                 << "UMP capture is still deferred.";
                 
    report.nextSteps << "Install/verify Windows MIDI Services SDK Runtime/Tools."
                     << "Study Microsoft.Windows.Devices.Midi2 C++/WinRT setup."
                     << "Add optional SDK detection in a future build."
                     << "Prototype endpoint listing only after detection is stable.";
                     
    return report;
}

QString WindowsMidiServicesSdkProbe::formatDetectionReport(const WindowsMidiServicesSdkDetectionReport& report) {
    QString out;
    out += "--- Windows MIDI Services SDK Detection Report ---\n";
    out += "Compile Mode: " + report.compileMode + "\n";
    out += "Status: " + report.status + "\n";
    out += "Headers Used: " + QString(report.realSdkHeadersUsed ? "Yes" : "No") + "\n";
    out += "Endpoint listing available: " + QString(report.realEndpointListingAvailable ? "Yes" : "No") + "\n";
    out += "UMP capture available: " + QString(report.realUmpCaptureAvailable ? "Yes" : "No") + "\n";
    
    out += "\n--- Optional Package Detection ---\n";
    out += "Package Detection Attempted: " + QString(report.optionalPackageDetectionAttempted ? "Yes" : "No") + "\n";
    out += "Package Detected: " + QString(report.optionalPackageDetected ? "Yes" : "No") + "\n";
    out += "Header Detection Attempted: " + QString(report.optionalHeaderDetectionAttempted ? "Yes" : "No") + "\n";
    out += "Headers Detected: " + QString(report.optionalHeadersDetected ? "Yes" : "No") + "\n";
    out += "Package Status: " + report.packageDetectionStatus + "\n";
    out += "Header Status: " + report.headerDetectionStatus + "\n";
    
    out += "\nNotes:\n";
    for (const QString& note : report.notes) {
        out += "- " + note + "\n";
    }
    out += "\nNext Steps:\n";
    for (const QString& step : report.nextSteps) {
        out += "- " + step + "\n";
    }
    return out;
}

QString WindowsMidiServicesSdkProbe::sdkExperimentStatusText() {
    return buildDetectionReport().status;
}
