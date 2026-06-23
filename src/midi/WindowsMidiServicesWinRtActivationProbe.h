#pragma once

#include <QString>

class WindowsMidiServicesWinRtActivationProbe {
public:
    static QString winRtActivationExperimentStatus();
    static bool initializeWinRtApartment();
};
