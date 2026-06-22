#pragma once

#include <QString>
#include <QStringList>
#include <QMap>

struct WindowsMidiServicesEndpointMetadata {
    QString transport;
    QString manufacturer;
    QString capabilities;
};

class WindowsMidiServicesMetadataProbe {
public:
    static QMap<QString, WindowsMidiServicesEndpointMetadata> extractMetadata();
};
