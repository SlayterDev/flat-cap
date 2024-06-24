#include <termios.h>

#include "config.h"
#include "indi_flatcapdriver.h"

#include "libindi/connectionplugins/connectionserial.h"
#include "libindi/indicom.h"

static std::unique_ptr<FlatCapDriver> flatcapdriver(new FlatCapDriver());

FlatCapDriver::FlatCapDriver() : LightBoxInterface(this, true)
{
    setVersion(CDRIVER_VERSION_MAJOR, CDRIVER_VERSION_MINOR);
}

const char *FlatCapDriver::getDefaultName()
{
    return "FlatCap Driver";
}

bool FlatCapDriver::initProperties()
{
    INDI::DefaultDevice::initProperties();

    initDustCapProperties(getDeviceName(), MAIN_CONTROL_TAB);
    initLightBoxProperties(getDeviceName(), MAIN_CONTROL_TAB);

    LightIntensityN[0].min = 0;
    LightIntensityN[0].max = 4096;
    LightIntensityN[0].step = 1;

    setDriverInterface(AUX_INTERFACE | LIGHTBOX_INTERFACE | DUSTCAP_INTERFACE);

    addAuxControls();

    serialConnection = new Connection::Serial(this);
    serialConnection->registerHandshake([&]() { return Handshake(); });
    serialConnection->setDefaultBaudRate(Connection::Serial::B_57600);
    serialConnection->setDefaultPort("/dev/ttyACM0");
    registerConnection(serialConnection);

    return true;
}

void FlatCapDriver::ISGetProperties(const char *dev)
{
    INDI::DefaultDevice::ISGetProperties(dev);

    isGetLightBoxProperties(dev);
}

bool FlatCapDriver::updateProperties()
{
    INDI::DefaultDevice::updateProperties();

    if (isConnected())
    {
        defineProperty(&ParkCapSP);
        defineProperty(&LightSP);
        defineProperty(&LightIntensityNP);

        updateLightBoxProperties();
    }
    else
    {
        deleteProperty(ParkCapSP.name);
        deleteProperty(LightSP.name);
        deleteProperty(LightIntensityNP.name);

        updateLightBoxProperties();
    }

    return true;
}

bool FlatCapDriver::Handshake()
{
    if (isSimulation())
    {
        LOGF_INFO("Connnected successfully to simulated %s.", getDeviceName());
        return true;
    }

    return true;
}

bool FlatCapDriver::ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n)
{
    if (processLightBoxNumber(dev, name, values, names, n))
        return true;
    
    return INDI::DefaultDevice::ISNewNumber(dev, name, values, names, n);
}

bool FlatCapDriver::ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n)
{
    if (dev != nullptr && strcmp(dev, getDeviceName()) == 0)
    {
        if (processLightBoxText(dev, name, texts, names, n))
            return true;
    }

    return INDI::DefaultDevice::ISNewText(dev, name, texts, names, n);
}

bool FlatCapDriver::ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n)
{
    if (dev != nullptr && strcmp(dev, getDeviceName()) == 0)
    {
        if (processDustCapSwitch(dev, name, states, names, n))
            return true;
        
        if (processLightBoxSwitch(dev, name, states, names, n))
            return true;
    }

    return INDI::DefaultDevice::ISNewSwitch(dev, name, states, names, n);
}

bool FlatCapDriver::ISSnoopDevice(XMLEle *root)
{
    // Is this needed?

    return INDI::DefaultDevice::ISSnoopDevice(root);
}

void FlatCapDriver::TimerHit()
{
    if (!isConnected())
        return;

    SetTimer(POLLMS);
}

bool FlatCapDriver::sendCommand(const char *cmd)
{
    int nbytes_read = 0, nbytes_written = 0, tty_rc = 0;
    char res[8] = {0};
    LOGF_DEBUG("CMD <%s>", cmd);

    if (!isSimulation())
    {
        tcflush(PortFD, TCIOFLUSH);
        if ((tty_rc = tty_write_string(PortFD, cmd, &nbytes_written)) != TTY_OK)
        {
            char errorMessage[MAXRBUF];
            tty_error_msg(tty_rc, errorMessage, MAXRBUF);
            LOGF_ERROR("Serial write error: %s", errorMessage);
            return false;
        }
    }

    if (isSimulation())
    {
        strncpy(res, "OK#", 8);
        nbytes_read = 3;
    }
    else
    {
        if ((tty_rc = tty_read_section(PortFD, res, '#', 1, &nbytes_read)) != TTY_OK)
        {
            char errorMessage[MAXRBUF];
            tty_error_msg(tty_rc, errorMessage, MAXRBUF);
            LOGF_ERROR("Serial read error: %s", errorMessage);
            return false;
        }
    }

    res[nbytes_read - 1] = '\0';
    LOGF_DEBUG("RES <%s>", res);

    return true;
}

IPState FlatCapDriver::ParkCap()
{
    // TODO: Park cap

    return IPS_BUSY;
}

IPState FlatCapDriver::UnParkCap()
{
    // TODO: UnPark Cap

    return IPS_BUSY;
}

bool FlatCapDriver::EnableLightBox(bool enable)
{
    return true;
}

bool FlatCapDriver::SetLightBoxBrightness(uint16_t value)
{
    return true;
}
