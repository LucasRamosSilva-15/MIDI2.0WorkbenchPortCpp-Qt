#include "WindowsMidiServicesBackendTests.h"
#include "TestUtils.h"
#include "../src/midi/WindowsMidiServicesBackend.h"
#include "../src/midi/WindowsMidiServicesSdkProbe.h"
#include "../src/midi/WindowsMidiServicesHeaderIncludeProbe.h"

void testWmsBackend_SkeletonBasics() {
    WindowsMidiServicesBackend backend;

    assertTest("WMS Backend Name", backend.backendName().contains("Windows MIDI Services"));
    assertTest("WMS Initially Closed", backend.isOpen() == false);
    assertTest("WMS Initial State is Disconnected", backend.getState() == WindowsMidiServicesBackend::ConnectionState::Disconnected);

    // Call openInputPort and ensure it fails gracefully in Safe Mode
    bool opened = backend.openInputPort(0);
    assertTest("WMS openInputPort fails in Safe Mode", opened == false);
    assertTest("WMS state transitions to Error in Safe Mode", backend.getState() == WindowsMidiServicesBackend::ConnectionState::Error);
    assertTest("WMS Error is recorded", !backend.getLastError().isEmpty());
    
    // Close the port and ensure it resets
    backend.closeInputPort();
    assertTest("WMS closed after failed open", backend.isOpen() == false);
    assertTest("WMS State is Disconnected after close", backend.getState() == WindowsMidiServicesBackend::ConnectionState::Disconnected);

    // Test listInputPorts string outputeryAvailableEndpoints returns empty via listInputPorts", wms.listInputPorts().isEmpty());
}

void testWmsSdkProbe_Status() {
    bool enabled = WindowsMidiServicesSdkProbe::isSdkExperimentEnabled();
    auto report = WindowsMidiServicesSdkProbe::buildDetectionReport();
    QString formatted = WindowsMidiServicesSdkProbe::formatDetectionReport(report);

    assertTest("SDK Probe Headers Used False", report.realSdkHeadersUsed == false);
    assertTest("SDK Probe Endpoint Listing False", report.realEndpointListingAvailable == false);
    assertTest("SDK Probe UMP Capture False", report.realUmpCaptureAvailable == false);

    assertTest("SDK Probe Formatted contains Windows MIDI Services", formatted.contains("Windows MIDI Services"));
    assertTest("SDK Probe Formatted contains Endpoint listing", formatted.contains("Endpoint listing"));
    assertTest("SDK Probe Formatted contains UMP capture", formatted.contains("UMP capture"));
    
    assertTest("SDK Probe Optional Package Detection Attempted False", report.optionalPackageDetectionAttempted == false);
    assertTest("SDK Probe Optional Package Detected False", report.optionalPackageDetected == false);
    
    assertTest("SDK Probe Formatted contains Optional Header Detection", formatted.contains("Optional Header Detection"));
    assertTest("SDK Probe Formatted contains Header Detection Attempted", formatted.contains("Header Detection Attempted"));
    assertTest("SDK Probe Formatted contains Candidate Headers Detected", formatted.contains("Candidate Headers Detected"));
    assertTest("SDK Probe Formatted contains Real Headers Used", formatted.contains("Real Headers Used"));
    assertTest("SDK Probe headerDetectionStatus not empty", !report.headerDetectionStatus.isEmpty());
    
#ifdef WINDOWS_MIDI_SERVICES_OPTIONAL_HEADER_DETECTION_ATTEMPTED
    assertTest("SDK Probe Optional Header Detection Attempted True", report.optionalHeaderDetectionAttempted == true);
#else
    assertTest("SDK Probe Optional Header Detection Attempted False", report.optionalHeaderDetectionAttempted == false);
#endif

#ifdef WINDOWS_MIDI_SERVICES_OPTIONAL_HEADERS_DETECTED
    assertTest("SDK Probe Optional Headers Detected True", report.optionalHeadersDetected == true);
#else
    assertTest("SDK Probe Optional Headers Detected False", report.optionalHeadersDetected == false);
#endif

    assertTest("SDK Probe sdkCandidateDiscoveryStatus not empty", !report.sdkCandidateDiscoveryStatus.isEmpty());
    assertTest("SDK Probe Formatted contains SDK Candidate Discovery", formatted.contains("SDK Candidate Discovery"));
    assertTest("SDK Probe Formatted contains Discovery Attempted", formatted.contains("Discovery Attempted"));
    assertTest("SDK Probe Formatted contains Header Candidate Detected", formatted.contains("Header Candidate Detected"));
    assertTest("SDK Probe Formatted contains WinMD Candidate Detected", formatted.contains("WinMD Candidate Detected"));
    
#ifdef WINDOWS_MIDI_SERVICES_SDK_CANDIDATE_DISCOVERY_ATTEMPTED
    assertTest("SDK Probe Discovery Attempted True", report.sdkCandidateDiscoveryAttempted == true);
#else
    assertTest("SDK Probe Discovery Attempted False", report.sdkCandidateDiscoveryAttempted == false);
#endif

#ifdef WINDOWS_MIDI_SERVICES_HEADER_CANDIDATE_DETECTED
    assertTest("SDK Probe Header Candidate Detected True", report.headerCandidateDetected == true);
#else
    assertTest("SDK Probe Header Candidate Detected False", report.headerCandidateDetected == false);
#endif

#ifdef WINDOWS_MIDI_SERVICES_WINMD_CANDIDATE_DETECTED
    assertTest("SDK Probe WinMD Candidate Detected True", report.winmdCandidateDetected == true);
#else
    assertTest("SDK Probe WinMD Candidate Detected False", report.winmdCandidateDetected == false);
#endif

    assertTest("SDK Probe typeReferenceExperimentStatus not empty", !report.typeReferenceExperimentStatus.isEmpty());
    assertTest("SDK Probe Formatted contains API Type Reference Compile Experiment", formatted.contains("API Type Reference Compile Experiment"));
    assertTest("SDK Probe Formatted contains Type Reference Requested", formatted.contains("Type Reference Requested"));
    assertTest("SDK Probe Formatted contains Type Reference Compiled", formatted.contains("Type Reference Compiled"));
    assertTest("SDK Probe Formatted contains Type Reference Status", formatted.contains("Type Reference Status"));

#ifdef WINDOWS_MIDI_SERVICES_TYPE_REFERENCE_EXPERIMENT_REQUESTED
    assertTest("SDK Probe Type Reference Requested True", report.typeReferenceExperimentRequested == true);
#else
    assertTest("SDK Probe Type Reference Requested False", report.typeReferenceExperimentRequested == false);
#endif

#ifdef WINDOWS_MIDI_SERVICES_TYPE_REFERENCE_EXPERIMENT_COMPILED
    assertTest("SDK Probe Type Reference Compiled True", report.typeReferenceExperimentCompiled == true);
#else
    assertTest("SDK Probe Type Reference Compiled False", report.typeReferenceExperimentCompiled == false);
#endif

    assertTest("SDK Probe cppwinrtAlignmentStatus not empty", !report.cppwinrtAlignmentStatus.isEmpty());
    assertTest("SDK Probe Formatted contains C++/WinRT Projection Alignment", formatted.contains("C++/WinRT Projection Alignment"));
    assertTest("SDK Probe Formatted contains Alignment Research Enabled", formatted.contains("Alignment Research Enabled"));
    assertTest("SDK Probe Formatted contains Real Include Attempt Enabled", formatted.contains("Real Include Attempt Enabled"));
    assertTest("SDK Probe Formatted contains Projection Aligned", formatted.contains("Projection Aligned"));
    assertTest("SDK Probe Formatted contains Projection Blocked", formatted.contains("Projection Blocked"));

#ifdef WINDOWS_MIDI_SERVICES_CPPWINRT_ALIGNMENT_RESEARCH_ENABLED
    assertTest("SDK Probe Alignment Research Enabled True", report.cppwinrtAlignmentResearchEnabled == true);
#else
    assertTest("SDK Probe Alignment Research Enabled False", report.cppwinrtAlignmentResearchEnabled == false);
#endif

#ifdef WINDOWS_MIDI_SERVICES_ALLOW_REAL_CPPWINRT_INCLUDE_ATTEMPT
    assertTest("SDK Probe Real Include Attempt Enabled True", report.realCppWinRtIncludeAttemptEnabled == true);
#else
    assertTest("SDK Probe Real Include Attempt Enabled False", report.realCppWinRtIncludeAttemptEnabled == false);
#endif
    
    assertTest("SDK Probe Formatted contains Generated Projections Strategy", formatted.contains("Generated Projections Strategy"));
    assertTest("SDK Probe Formatted contains Strategy Research Enabled", formatted.contains("Strategy Research Enabled"));
    assertTest("SDK Probe generatedProjectionStrategyStatus not empty", !report.generatedProjectionStrategyStatus.isEmpty());

#ifdef WINDOWS_MIDI_SERVICES_GENERATED_PROJECTION_STRATEGY_RESEARCH_ENABLED
    assertTest("SDK Probe Strategy Research Enabled True", report.generatedProjectionStrategyResearchEnabled == true);
#else
    assertTest("SDK Probe Strategy Research Enabled False", report.generatedProjectionStrategyResearchEnabled == false);
#endif

    assertTest("SDK Probe Formatted contains WinRT Activation Experiment", formatted.contains("WinRT Activation Experiment"));
    assertTest("SDK Probe Formatted contains Activation Experiment Compiled", formatted.contains("Activation Experiment Compiled"));
    assertTest("SDK Probe Formatted contains Real Activation Attempt Enabled", formatted.contains("Real Activation Attempt Enabled"));
    assertTest("SDK Probe winRtActivationExperimentStatus not empty", !report.winRtActivationExperimentStatus.isEmpty());

#ifdef WINDOWS_MIDI_SERVICES_WINRT_ACTIVATION_EXPERIMENT_COMPILED
    assertTest("SDK Probe Activation Experiment Compiled True", report.winRtActivationExperimentCompiled == true);
#else
    assertTest("SDK Probe Activation Experiment Compiled False", report.winRtActivationExperimentCompiled == false);
#endif

#ifdef WINDOWS_MIDI_SERVICES_ALLOW_REAL_WINRT_ACTIVATION_ATTEMPT
    assertTest("SDK Probe Real Activation Attempt Enabled True", report.realWinRtActivationAttemptEnabled == true);
#else
    assertTest("SDK Probe Real Activation Attempt Enabled False", report.realWinRtActivationAttemptEnabled == false);
#endif

    assertTest("SDK Probe Formatted contains Windows MIDI Services API Surface", formatted.contains("Windows MIDI Services API Surface"));
    assertTest("SDK Probe Formatted contains API Surface Mapping Enabled", formatted.contains("API Surface Mapping Enabled"));
    assertTest("SDK Probe apiSurfaceMappingStatus not empty", !report.apiSurfaceMappingStatus.isEmpty());

#ifdef WINDOWS_MIDI_SERVICES_API_SURFACE_MAPPING_ENABLED
    assertTest("SDK Probe API Surface Mapping Enabled True", report.apiSurfaceMappingEnabled == true);
#else
    assertTest("SDK Probe API Surface Mapping Enabled False", report.apiSurfaceMappingEnabled == false);
#endif

    assertTest("SDK Probe Formatted contains Windows MIDI Services Endpoint Enumeration", formatted.contains("Windows MIDI Services Endpoint Enumeration"));
    assertTest("SDK Probe Formatted contains Enumeration Enabled", formatted.contains("Enumeration Enabled"));
    assertTest("SDK Probe Formatted contains Discovered Endpoints", formatted.contains("Discovered Endpoints"));

#ifdef WINDOWS_MIDI_SERVICES_ENDPOINT_ENUMERATION_ENABLED
    assertTest("SDK Probe Endpoint Enumeration Enabled True", report.endpointEnumerationEnabled == true);
#else
    assertTest("SDK Probe Endpoint Enumeration Enabled False", report.endpointEnumerationEnabled == false);
#endif

#ifdef WINDOWS_MIDI_SERVICES_METADATA_RESEARCH_ENABLED
    assertTest("SDK Probe Metadata Research Enabled True", report.metadataResearchEnabled == true);
#else
    assertTest("SDK Probe Metadata Research Enabled False", report.metadataResearchEnabled == false);
#endif

    assertTest("SDK Probe Formatted contains Windows MIDI Services Input Stream", formatted.contains("Windows MIDI Services Input Stream"));
    assertTest("SDK Probe Formatted contains Stream Prototype Enabled", formatted.contains("Stream Prototype Enabled"));

#ifdef WINDOWS_MIDI_SERVICES_INPUT_STREAM_EXPERIMENT_ENABLED
    assertTest("SDK Probe Input Stream Prototype Enabled True", report.inputStreamPrototypeEnabled == true);
#else
    assertTest("SDK Probe Input Stream Prototype Enabled False", report.inputStreamPrototypeEnabled == false);
#endif

    assertTest("SDK Probe Formatted contains Backend Integration Preparation", formatted.contains("Backend Integration Preparation"));
    assertTest("SDK Probe Formatted contains Backend Orchestrator Ready", formatted.contains("Backend Orchestrator Ready"));

#ifdef WINDOWS_MIDI_SERVICES_BACKEND_INTEGRATION_PREP_ENABLED
    assertTest("SDK Probe Backend Integration Prep Enabled True", report.backendIntegrationPrepEnabled == true);
#else
    assertTest("SDK Probe Backend Integration Prep Enabled False", report.backendIntegrationPrepEnabled == false);
#endif

    assertTest("SDK Probe Formatted contains Experimental Backend Capture", formatted.contains("Experimental Backend Capture"));
    assertTest("SDK Probe Formatted contains Capture Engine Armed", formatted.contains("Capture Engine Armed"));

#ifdef WINDOWS_MIDI_SERVICES_BACKEND_EXPERIMENTAL_CAPTURE_ENABLED
    assertTest("SDK Probe Experimental Capture Armed True", report.experimentalCaptureArmed == true);
#else
    assertTest("SDK Probe Experimental Capture Armed False", report.experimentalCaptureArmed == false);
#endif

    assertTest("SDK Probe Formatted contains User-Provided SDK Root", formatted.contains("User-Provided SDK Root"));
    assertTest("SDK Probe Formatted contains SDK Root Configured", formatted.contains("SDK Root Configured"));
    assertTest("SDK Probe Formatted contains SDK Root Status", formatted.contains("SDK Root Status"));
    assertTest("SDK Probe sdkRootStatus not empty", !report.sdkRootStatus.isEmpty());
    
#ifdef WINDOWS_MIDI_SERVICES_SDK_ROOT_PROVIDED
    assertTest("SDK Probe SDK Root Configured True", report.userProvidedSdkRootConfigured == true);
#else
    assertTest("SDK Probe SDK Root Configured False", report.userProvidedSdkRootConfigured == false);
#endif

#ifdef USE_WINDOWS_MIDI_SERVICES_SDK_EXPERIMENT
    assertTest("SDK Probe Experiment is ENABLED", enabled == true);
    assertTest("SDK Probe Report Flag True", report.experimentCompileFlagEnabled == true);
#else
    assertTest("SDK Probe Experiment is DISABLED", enabled == false);
    assertTest("SDK Probe Report Flag False", report.experimentCompileFlagEnabled == false);
#endif
}

void testWmsHeaderIncludeProbe() {
    auto report = WindowsMidiServicesHeaderIncludeProbe::runProbe();
    QString formatted = WindowsMidiServicesHeaderIncludeProbe::formatIncludeReport(report);
    
    assertTest("Include Probe Formatted contains Header Include Compile Experiment", formatted.contains("Header Include Compile Experiment"));
    assertTest("Include Probe Formatted contains Include Experiment Attempted", formatted.contains("Include Experiment Attempted"));
    assertTest("Include Probe Formatted contains Headers Included Successfully", formatted.contains("Headers Included Successfully"));
    
#ifdef USE_WINDOWS_MIDI_SERVICES_HEADER_INCLUDE_EXPERIMENT
    assertTest("Include Probe Attempted True", report.includeExperimentAttempted == true);
#ifdef WINDOWS_MIDI_SERVICES_OPTIONAL_HEADERS_DETECTED
    assertTest("Include Probe Included Successfully True", report.headersIncludedSuccessfully == true);
#else
    assertTest("Include Probe Included Successfully False (No Headers)", report.headersIncludedSuccessfully == false);
#endif
#else
    assertTest("Include Probe Attempted False", report.includeExperimentAttempted == false);
    assertTest("Include Probe Included Successfully False (Experiment Disabled)", report.headersIncludedSuccessfully == false);
#endif
}

void runWindowsMidiServicesBackendTests() {
    std::cout << "\nStarting WindowsMidiServicesBackend Skeleton Tests\n\n";
    testWmsBackend_SkeletonBasics();
    testWmsSdkProbe_Status();
    testWmsHeaderIncludeProbe();
}
