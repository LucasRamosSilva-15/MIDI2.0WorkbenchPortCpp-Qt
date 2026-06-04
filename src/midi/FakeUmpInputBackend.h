#pragma once
#include "IUmpInputBackend.h"

class FakeUmpInputBackend : public IUmpInputBackend {
public:
    FakeUmpInputBackend();
    ~FakeUmpInputBackend() override;

    QString backendName() const override;
    QStringList listInputPorts() override;

    bool openInputPort(int portIndex) override;
    void closeInputPort() override;
    bool isOpen() const override;

    std::vector<UmpRawEvent> pollUmpEvents() override;

private:
    bool m_isOpen;
    QString m_portName;
    int m_pollCount;
};
