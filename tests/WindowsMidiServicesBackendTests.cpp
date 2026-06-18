#include "WindowsMidiServicesBackendTests.h"
#include "TestUtils.h"
#include "../src/midi/WindowsMidiServicesBackend.h"
#include "../src/midi/WindowsMidiServicesSdkProbe.h"

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

void runWindowsMidiServicesBackendTests() {
    std::cout << "\nStarting WindowsMidiServicesBackend Skeleton Tests\n\n";
    testWmsBackend_SkeletonBasics();
    testWmsSdkProbe_Status();
}
