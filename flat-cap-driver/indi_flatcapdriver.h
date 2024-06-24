#pragma once

#include "libindi/defaultdevice.h"
#include "libindi/indidustcapinterface.h"
#include "libindi/indilightboxinterface.h"

namespace Connection
{
    class Serial;
}

class FlatCapDriver : public INDI::DefaultDevice, public INDI::DustCapInterface, public INDI::LightBoxInterface
{
public:
    FlatCapDriver();
    virtual ~FlatCapDriver() = default;

    virtual bool initProperties() override;
    virtual bool updateProperties() override;

    virtual void ISGetProperties(const char *dev) override;
    virtual bool ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n) override;
    virtual bool ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n) override;
    virtual bool ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n) override;
    virtual bool ISSnoopDevice(XMLEle *root) override;

    virtual void TimerHit() override;

protected:
    virtual const char *getDefaultName() override;

    virtual IPState ParkCap() override;
    virtual IPState UnParkCap() override;

    virtual bool SetLightBoxBrightness(uint16_t value);
    virtual bool EnableLightBox(bool enable);

private:
    bool Handshake();
    bool sendCommand(const char *cmd);
    int PortFD{-1};

    Connection::Serial *serialConnection{nullptr};
};
