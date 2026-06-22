#pragma once

#include <QString>
#include <QStringList>

struct WindowsMidiServicesSdkDetectionReport {
    bool experimentCompileFlagEnabled = false;
    bool realSdkHeadersUsed = false;
    bool realEndpointListingAvailable = false;
    bool realUmpCaptureAvailable = false;
    
    bool optionalPackageDetectionAttempted = false;
    bool optionalPackageDetected = false;
    bool optionalHeaderDetectionAttempted = false;
    bool optionalHeadersDetected = false;
    QString headerDetectionStatus;
    
    bool sdkCandidateDiscoveryAttempted = false;
    bool headerCandidateDetected = false;
    bool winmdCandidateDetected = false;
    QString sdkCandidateDiscoveryStatus;
    
    bool typeReferenceExperimentRequested = false;
    bool typeReferenceExperimentCompiled = false;
    QString typeReferenceExperimentStatus;
    
    bool cppwinrtAlignmentResearchEnabled = false;
    bool realCppWinRtIncludeAttemptEnabled = false;
    bool cppwinrtProjectionAligned = false;
    bool cppwinrtProjectionBlocked = true;
    QString cppwinrtAlignmentStatus;
    
    bool generatedProjectionStrategyResearchEnabled = false;
    QString generatedProjectionStrategyStatus;
    
    bool winRtActivationExperimentCompiled = false;
    bool realWinRtActivationAttemptEnabled = false;
    QString winRtActivationExperimentStatus;
    
    bool userProvidedSdkRootMissing = false;
    bool userProvidedSdkRootAcceptedForResearch = false;
    bool userProvidedSdkRootConfigured = false;
    
    QString packageDetectionStatus;
    QString sdkRootStatus;
    
    QString compileMode;
    QString status;
    QStringList notes;
    QStringList nextSteps;
};

/**
 * @brief (v4.4.0) Probe experimental para detecção teórica do SDK Windows MIDI Services.
 * 
 * Esta classe é estéril. Não embute `<winrt>` nem pacotes C++ da Microsoft por padrão.
 * Sua finalidade é aferir se a flag de compilação foi engatilhada e emitir uma resposta em string segura.
 */
class WindowsMidiServicesSdkProbe {
public:
    static bool isSdkExperimentEnabled();
    static WindowsMidiServicesSdkDetectionReport buildDetectionReport();
    static QString formatDetectionReport(const WindowsMidiServicesSdkDetectionReport& report);
    static QString sdkExperimentStatusText();
};
