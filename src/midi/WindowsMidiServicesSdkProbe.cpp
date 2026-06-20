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
    
#ifdef WINDOWS_MIDI_SERVICES_OPTIONAL_HEADER_DETECTION_ATTEMPTED
    report.optionalHeaderDetectionAttempted = true;
#ifdef WINDOWS_MIDI_SERVICES_OPTIONAL_HEADERS_DETECTED
    report.optionalHeadersDetected = true;
    report.headerDetectionStatus = "Candidate Windows MIDI Services headers were detected inside the user-provided SDK root. Headers are not included or consumed in v4.7.0.";
#else
    report.optionalHeadersDetected = false;
    report.headerDetectionStatus = "Candidate Windows MIDI Services headers were not detected inside the user-provided SDK root. This is non-fatal in v4.7.0.";
#endif
#else
    report.optionalHeaderDetectionAttempted = false;
    report.optionalHeadersDetected = false;
    report.headerDetectionStatus = "Optional header detection was not attempted because no user-provided SDK root was configured.";
#endif
    
    report.packageDetectionStatus = "Optional package detection is documented but not performed in v4.9.0.";
    
#ifdef WINDOWS_MIDI_SERVICES_SDK_CANDIDATE_DISCOVERY_ATTEMPTED
    report.sdkCandidateDiscoveryAttempted = true;
#else
    report.sdkCandidateDiscoveryAttempted = false;
#endif

#ifdef WINDOWS_MIDI_SERVICES_HEADER_CANDIDATE_DETECTED
    report.headerCandidateDetected = true;
#else
    report.headerCandidateDetected = false;
#endif

#ifdef WINDOWS_MIDI_SERVICES_WINMD_CANDIDATE_DETECTED
    report.winmdCandidateDetected = true;
#else
    report.winmdCandidateDetected = false;
#endif

    if (report.sdkCandidateDiscoveryAttempted) {
        if (report.headerCandidateDetected && report.winmdCandidateDetected) {
            report.sdkCandidateDiscoveryStatus = "Header and WinMD candidates were detected inside the user-provided SDK root. No API calls are made in v4.9.0.";
        } else if (report.headerCandidateDetected) {
            report.sdkCandidateDiscoveryStatus = "Header candidates were detected inside the user-provided SDK root. No API calls are made in v4.9.0.";
        } else if (report.winmdCandidateDetected) {
            report.sdkCandidateDiscoveryStatus = "WinMD metadata candidate was detected inside the user-provided SDK root. Header include compilation is still not attempted without a header candidate.";
        } else {
            report.sdkCandidateDiscoveryStatus = "SDK candidate discovery was attempted, but no header or WinMD candidates were detected inside the user-provided SDK root. This is non-fatal in v4.9.0.";
        }
    } else {
        report.sdkCandidateDiscoveryStatus = "SDK candidate discovery was not attempted because no user-provided SDK root was configured.";
    }
    
    if (report.experimentCompileFlagEnabled) {
        report.compileMode = "SDK experiment compile flag enabled";
        report.status = "SDK experiment flag is enabled, but real Windows MIDI Services SDK probing is not implemented in v4.4.0.";
    } else {
        report.compileMode = "SDK experiment disabled";
        report.status = "SDK experiment is disabled. Normal build path does not require Windows MIDI Services SDK.";
    }
    
#ifdef WINDOWS_MIDI_SERVICES_SDK_ROOT_PROVIDED
    report.userProvidedSdkRootConfigured = true;
#ifdef WINDOWS_MIDI_SERVICES_SDK_ROOT_MISSING
    report.userProvidedSdkRootMissing = true;
    report.userProvidedSdkRootAcceptedForResearch = false;
    report.sdkRootStatus = "A user-provided Windows MIDI Services SDK root was configured but the path was reported missing by CMake. This is non-fatal in v4.6.0.";
#else
    report.userProvidedSdkRootMissing = false;
    report.userProvidedSdkRootAcceptedForResearch = true;
    report.sdkRootStatus = "A user-provided Windows MIDI Services SDK root was configured for research. No headers or libraries are consumed in v4.7.0.";
#endif
#else
    report.userProvidedSdkRootConfigured = false;
    report.userProvidedSdkRootMissing = false;
    report.userProvidedSdkRootAcceptedForResearch = false;
    report.sdkRootStatus = "No user-provided Windows MIDI Services SDK root is configured.";
#endif
    
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
    out += "\n--- Optional Header Detection ---\n";
    out += "Header Detection Attempted: " + QString(report.optionalHeaderDetectionAttempted ? "Yes" : "No") + "\n";
    out += "Candidate Headers Detected: " + QString(report.optionalHeadersDetected ? "Yes" : "No") + "\n";
    out += "Real Headers Used: No\n";
    out += "Header Detection Status: " + report.headerDetectionStatus + "\n";
    
    out += "\n--- SDK Candidate Discovery ---\n";
    out += "Discovery Attempted: " + QString(report.sdkCandidateDiscoveryAttempted ? "Yes" : "No") + "\n";
    out += "Header Candidate Detected: " + QString(report.headerCandidateDetected ? "Yes" : "No") + "\n";
    out += "WinMD Candidate Detected: " + QString(report.winmdCandidateDetected ? "Yes" : "No") + "\n";
    out += "Discovery Status: " + report.sdkCandidateDiscoveryStatus + "\n";
    
    out += "\n--- User-Provided SDK Root Research ---\n";
    out += "SDK Root Configured: " + QString(report.userProvidedSdkRootConfigured ? "Yes" : "No") + "\n";
    out += "SDK Root Missing: " + QString(report.userProvidedSdkRootMissing ? "Yes" : "No") + "\n";
    out += "SDK Root Accepted For Research: " + QString(report.userProvidedSdkRootAcceptedForResearch ? "Yes" : "No") + "\n";
    out += "SDK Root Status: " + report.sdkRootStatus + "\n";
    
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
