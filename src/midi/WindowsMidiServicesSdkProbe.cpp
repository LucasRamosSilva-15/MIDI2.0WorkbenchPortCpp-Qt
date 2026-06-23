#include "WindowsMidiServicesSdkProbe.h"
#include "WindowsMidiServicesWinRtActivationProbe.h"
#include "WindowsMidiServicesApiSurfaceProbe.h"
#include "WindowsMidiServicesEndpointEnumeratorProbe.h"

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
    
#ifdef WINDOWS_MIDI_SERVICES_TYPE_REFERENCE_EXPERIMENT_REQUESTED
    report.typeReferenceExperimentRequested = true;
#else
    report.typeReferenceExperimentRequested = false;
#endif

#ifdef WINDOWS_MIDI_SERVICES_TYPE_REFERENCE_EXPERIMENT_COMPILED
    report.typeReferenceExperimentCompiled = true;
#ifdef WINDOWS_MIDI_SERVICES_ALLOW_REAL_CPPWINRT_INCLUDE_ATTEMPT
    report.typeReferenceExperimentStatus = "Type reference compile experiment attempted (REAL INCLUDE ENABLED). This may have caused compile errors if mismatched.";
#else
    report.typeReferenceExperimentStatus = "Type reference experiment is blocked pending C++/WinRT projection alignment. No real include attempt is made in safe mode.";
#endif
#else
    report.typeReferenceExperimentCompiled = false;
    if (report.typeReferenceExperimentRequested) {
        report.typeReferenceExperimentStatus = "Type reference experiment was requested, but candidate headers were not available. Non-fatal in v4.11.0.";
    } else {
        report.typeReferenceExperimentStatus = "Type reference experiment is disabled.";
    }
#endif

#ifdef WINDOWS_MIDI_SERVICES_CPPWINRT_ALIGNMENT_RESEARCH_ENABLED
    report.cppwinrtAlignmentResearchEnabled = true;
    report.cppwinrtAlignmentStatus = "C++/WinRT projection alignment research is enabled. Previous type reference attempts showed CPPWINRT_VERSION mismatch or missing generated impl headers. No endpoint listing is enabled in v4.11.0.";
#else
    report.cppwinrtAlignmentResearchEnabled = false;
    report.cppwinrtAlignmentStatus = "C++/WinRT projection alignment research is disabled.";
#endif

#ifdef WINDOWS_MIDI_SERVICES_ALLOW_REAL_CPPWINRT_INCLUDE_ATTEMPT
    report.realCppWinRtIncludeAttemptEnabled = true;
    report.cppwinrtAlignmentStatus += " Real C++/WinRT include attempt is enabled. This mode may fail compilation and is for local diagnostics only.";
#else
    report.realCppWinRtIncludeAttemptEnabled = false;
#endif

    // Em v4.11.0 nós sabemos que a projeção ainda está bloqueada.
    report.cppwinrtProjectionAligned = false;
    report.cppwinrtProjectionBlocked = true;
    
#ifdef WINDOWS_MIDI_SERVICES_GENERATED_PROJECTION_STRATEGY_RESEARCH_ENABLED
    report.generatedProjectionStrategyResearchEnabled = true;
    report.generatedProjectionStrategyStatus = "C++/WinRT generated projection strategy research is enabled. Projection is passively studied via isolation. No runtime WINRT API is initialized in v4.13.0.";
#else
    report.generatedProjectionStrategyResearchEnabled = false;
    report.generatedProjectionStrategyStatus = "C++/WinRT generated projection strategy research is disabled.";
#endif

#ifdef WINDOWS_MIDI_SERVICES_WINRT_ACTIVATION_EXPERIMENT_COMPILED
    report.winRtActivationExperimentCompiled = true;
    report.winRtActivationExperimentStatus = WindowsMidiServicesWinRtActivationProbe::winRtActivationExperimentStatus();
#else
    report.winRtActivationExperimentCompiled = false;
    report.winRtActivationExperimentStatus = "WinRT Activation experiment is disabled.";
#endif

#ifdef WINDOWS_MIDI_SERVICES_ALLOW_REAL_WINRT_ACTIVATION_ATTEMPT
    report.realWinRtActivationAttemptEnabled = true;
#else
    report.realWinRtActivationAttemptEnabled = false;
#endif

#ifdef WINDOWS_MIDI_SERVICES_API_SURFACE_MAPPING_ENABLED
    report.apiSurfaceMappingEnabled = true;
    report.apiSurfaceMappingStatus = WindowsMidiServicesApiSurfaceProbe::apiSurfaceMappingStatus();
#else
    report.apiSurfaceMappingEnabled = false;
    report.apiSurfaceMappingStatus = "API Surface mapping is disabled.";
#endif

#ifdef WINDOWS_MIDI_SERVICES_ENDPOINT_ENUMERATION_ENABLED
    report.endpointEnumerationEnabled = true;
    report.discoveredEndpoints = WindowsMidiServicesEndpointEnumeratorProbe::enumerateEndpoints();
#else
    report.endpointEnumerationEnabled = false;
#endif

#ifdef WINDOWS_MIDI_SERVICES_METADATA_RESEARCH_ENABLED
    report.metadataResearchEnabled = true;
    report.endpointMetadata = WindowsMidiServicesMetadataProbe::extractMetadata();
#else
    report.metadataResearchEnabled = false;
#endif

#ifdef WINDOWS_MIDI_SERVICES_INPUT_STREAM_EXPERIMENT_ENABLED
    report.inputStreamPrototypeEnabled = true;
    report.inputStreamPrototypeStatus = WindowsMidiServicesInputStreamProbe::simulateUmpCallbackStructure();
#else
    report.inputStreamPrototypeEnabled = false;
    report.inputStreamPrototypeStatus = "Input Stream Callback Prototype is disabled.";
#endif

#ifdef WINDOWS_MIDI_SERVICES_BACKEND_INTEGRATION_PREP_ENABLED
    report.backendIntegrationPrepEnabled = true;
#else
    report.backendIntegrationPrepEnabled = false;
#endif

#ifdef WINDOWS_MIDI_SERVICES_BACKEND_EXPERIMENTAL_CAPTURE_ENABLED
    report.experimentalCaptureArmed = true;
#else
    report.experimentalCaptureArmed = false;
#endif

#ifdef WINDOWS_MIDI_SERVICES_UI_INTEGRATION_RESEARCH_ENABLED
    report.uiIntegrationResearchEnabled = true;
    report.uiIntegrationStrategyStatus = WindowsMidiServicesUiIntegrationProbe::validateHotSwapStrategy();
#else
    report.uiIntegrationResearchEnabled = false;
    report.uiIntegrationStrategyStatus = "UI Integration Research is currently disabled.";
#endif
    
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
    
    out += "\n--- API Type Reference Compile Experiment ---\n";
    out += "Type Reference Requested: " + QString(report.typeReferenceExperimentRequested ? "Yes" : "No") + "\n";
    out += "Type Reference Compiled: " + QString(report.typeReferenceExperimentCompiled ? "Yes" : "No") + "\n";
    out += "Type Reference Status: " + report.typeReferenceExperimentStatus + "\n";
    
    out += "\n--- C++/WinRT Projection Alignment ---\n";
    out += "Alignment Research Enabled: " + QString(report.cppwinrtAlignmentResearchEnabled ? "Yes" : "No") + "\n";
    out += "Real Include Attempt Enabled: " + QString(report.realCppWinRtIncludeAttemptEnabled ? "Yes" : "No") + "\n";
    out += "Projection Aligned: " + QString(report.cppwinrtProjectionAligned ? "Yes" : "No") + "\n";
    out += "Projection Blocked: " + QString(report.cppwinrtProjectionBlocked ? "Yes" : "No") + "\n";
    out += "Alignment Status: " + report.cppwinrtAlignmentStatus + "\n";
    
    out += "\n--- Generated Projections Strategy ---\n";
    out += "Strategy Research Enabled: " + QString(report.generatedProjectionStrategyResearchEnabled ? "Yes" : "No") + "\n";
    out += "Strategy Status: " + report.generatedProjectionStrategyStatus + "\n";
    
    out += "\n--- WinRT Activation Experiment ---\n";
    out += "Activation Experiment Compiled: " + QString(report.winRtActivationExperimentCompiled ? "Yes" : "No") + "\n";
    out += "Real Activation Attempt Enabled: " + QString(report.realWinRtActivationAttemptEnabled ? "Yes" : "No") + "\n";
    out += "Activation Status: " + report.winRtActivationExperimentStatus + "\n";
    
    out += "\n--- Windows MIDI Services API Surface ---\n";
    out += "API Surface Mapping Enabled: " + QString(report.apiSurfaceMappingEnabled ? "Yes" : "No") + "\n";
    out += "Mapping Status: " + report.apiSurfaceMappingStatus + "\n";
    
    out += "\n--- Windows MIDI Services Endpoint Enumeration ---\n";
    out += "Enumeration Enabled: " + QString(report.endpointEnumerationEnabled ? "Yes" : "No") + "\n";
    out += "Discovered Endpoints:\n";
    if (report.discoveredEndpoints.isEmpty()) {
        out += " - None or blocked by Safe Mode\n";
    } else {
        for (const QString& ep : report.discoveredEndpoints) {
            out += " - " + ep + "\n";
            if (report.metadataResearchEnabled && report.endpointMetadata.contains(ep)) {
                const auto& meta = report.endpointMetadata[ep];
                out += "     Transport: " + meta.transport + "\n";
                out += "     Manufacturer: " + meta.manufacturer + "\n";
                out += "     Capabilities: " + meta.capabilities + "\n";
            }
        }
    }
    
    out += "\n--- Windows MIDI Services Input Stream ---\n";
    out += "Stream Prototype Enabled: " + QString(report.inputStreamPrototypeEnabled ? "Yes" : "No") + "\n";
    out += "Status: " + report.inputStreamPrototypeStatus + "\n";
    
    out += "\n--- Backend Integration Preparation ---\n";
    out += "Backend Orchestrator Ready: " + QString(report.backendIntegrationPrepEnabled ? "Yes" : "No") + "\n";
    
    out += "\n--- Experimental Backend Capture ---\n";
    out += "Capture Engine Armed: " + QString(report.experimentalCaptureArmed ? "Yes" : "No") + "\n";
    
    out += "\n--- UI Integration Research ---\n";
    out += "Hot-Swap Architecture Planned: " + QString(report.uiIntegrationResearchEnabled ? "Yes" : "No") + "\n";
    out += "Status: " + report.uiIntegrationStrategyStatus + "\n";
    
    out += "\n--- Production Hardening ---\n";
    out += "Resilience Checks Active: Yes\n";
    out += "COM Exception Handling: Armed\n";
    
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
