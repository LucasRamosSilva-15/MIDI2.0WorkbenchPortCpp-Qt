#include "WindowsMidiServicesBackendTests.h"
#include "TestUtils.h"
#include "../src/midi/WindowsMidiServicesBackend.h"
#include "../src/midi/WindowsMidiServicesSdkProbe.h"
#include "../src/midi/WindowsMidiServicesHeaderIncludeProbe.h"

void testWmsBackend_SkeletonBasics() {
    WindowsMidiServicesBackend wms;
    assertTest("WMS Backend Name", wms.backendName().contains("Windows MIDI Services"));
    assertTest("WMS Initially Closed", !wms.isOpen());
    assertTest("WMS Backend queryAvailableEndpoints returns empty via listInputPorts", wms.listInputPorts().isEmpty());
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
