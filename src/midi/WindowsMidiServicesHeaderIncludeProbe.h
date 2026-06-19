#pragma once

#include <QString>
#include <QStringList>

struct WindowsMidiServicesHeaderIncludeReport {
    bool includeExperimentAttempted = false;
    bool headersIncludedSuccessfully = false;
    QString includeStatus;
    QStringList notes;
};

class WindowsMidiServicesHeaderIncludeProbe {
public:
    static WindowsMidiServicesHeaderIncludeReport runProbe();
    static QString formatIncludeReport(const WindowsMidiServicesHeaderIncludeReport& report);
};
