#pragma once
#include <QString>
#include <QStringList>
#include <vector>
#include "../core/UmpRawEvent.h"

class IUmpInputBackend {
public:
    virtual ~IUmpInputBackend() = default;

    virtual QString backendName() const = 0;
    virtual QStringList listInputPorts() = 0;

    virtual bool openInputPort(int portIndex) = 0;
    virtual void closeInputPort() = 0;
    virtual bool isOpen() const = 0;

    virtual std::vector<UmpRawEvent> pollUmpEvents() = 0;
};
