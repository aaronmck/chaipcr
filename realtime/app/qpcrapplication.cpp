//
// Chai PCR - Software platform for Open qPCR and Chai's Real-Time PCR instruments.
// For more information visit http://www.chaibio.com
//
// Copyright 2016 Chai Biotechnologies Inc. <info@chaibio.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <Poco/Net/HTTPServer.h>

#include <unistd.h>
#include <fcntl.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "pcrincludes.h"
#include "icontrol.h"
#include "experimentcontroller.h"
#include "qpcrrequesthandlerfactory.h"
#include "qpcrfactory.h"
#include "qpcrapplication.h"
#include "dbcontrol.h"
#include "exceptionhandler.h"
#include "wirelessmanager.h"
#include "maincontrollers.h"
#include "timechecker.h"
#include "settings.h"
#include "updatemanager.h"
#include "logger.h"

using namespace std;
using namespace Poco::Net;
using namespace Poco::Util;

class QPCRServerSocket : public ServerSocket
{
public:
    QPCRServerSocket(Poco::UInt16 port): ServerSocket(port)
    {
        fcntl(sockfd(), F_SETFD, FD_CLOEXEC);
    }
};

// Class QPCRApplication
void QPCRApplication::stopExperiment(const string &message) {
    _experimentController->stop(message);
}

bool QPCRApplication::isMachinePaused() const
{
    return _experimentController->machineState() == ExperimentController::PausedMachineState;
}

int QPCRApplication::getUserId(const std::string &token) const
{
    return _dbControl->getUserId(token);
}

void QPCRApplication::defineOptions(OptionSet &options)
{
    options.addOption(Option("enable_log_file", "flog", "enables log files", false, "path", true));

    ServerApplication::defineOptions(options);
}

void QPCRApplication::handleOption(const string &name, const string &value)
{
    ServerApplication::handleOption(name, value);

    if (name == "enable_log_file")
    {
        Logger::setup(kAppLogName, value);

        setLogger(Logger::get());
    }
}

void QPCRApplication::initialize(Application&) {
    _workState = false;

    try {
        if (!Logger::isSetup())
        {
            Logger::setup(kAppLogName);

            setLogger(Logger::get());
        }

        readDeviceFile();
        readConfigurationFile();

        QPCRFactory::constructMachine(_controlUnits, _threadControlUnits);

        _dbControl.reset(new DBControl());
        _experimentController = ExperimentController::createInstance(_dbControl);
        _wirelessManager.reset(new WirelessManager());
        _timeChecker.reset(new TimeChecker());
        _updateManager.reset(new UpdateManager(_dbControl));

        _timeChecker->timeStateChanged.connect([&](bool state)
        {
            Settings settings;
            settings.setTimeValid(state);

            _dbControl->updateSettings(settings);
        });

        initSignals();
    }
    catch (const std::exception &ex) {
        cout << "Initialize - exception occured: " << ex.what() << '\n';
        throw;
    }
    catch (...) {
        cout << "Initialize - unknown exception occured\n";
        throw;
    }
}

int QPCRApplication::main(const vector<string>&) {
    HTTPServerParams *params = new HTTPServerParams;
    QPCRServerSocket socket(kHttpServerPort);
    HTTPServer server(new QPCRRequestHandlerFactory, socket, params);
    Poco::LogStream logStream(Logger::get());

    try
    {
        server.start();
        _updateManager->startChecking();

        for (auto threadControlUnit: _threadControlUnits)
            threadControlUnit->start();

        HeatSinkInstance::getInstance()->startADCReading();

        _workState = true;
        while (!waitSignal() && _workState) {
            for (auto controlUnit: _controlUnits)
                controlUnit->process();

            if (_exception)
                rethrow_exception(_exception);
        }

        params->setKeepAlive(false);
        server.stopAll(true);

        _experimentController->stop();

        for (auto threadControlUnit: _threadControlUnits)
            threadControlUnit->stop();

        return EXIT_OK;
    }
    catch (const exception &ex)
    {
        logStream << "Exception occured: " << ex.what() << std::endl;

        params->setKeepAlive(false);
        server.stopAll(true);

        _experimentController->stop(ex.what());

        for (auto threadControlUnit: _threadControlUnits)
            threadControlUnit->stop();

        return EXIT_SOFTWARE;
    }
    catch (...)
    {
        logStream << "Unknown exception occured" << std::endl;

        params->setKeepAlive(false);
        server.stopAll(true);

        _experimentController->stop("Unknown exception occured");

        for (auto threadControlUnit: _threadControlUnits)
            threadControlUnit->stop();

        return EXIT_SOFTWARE;
    }
}

void QPCRApplication::readDeviceFile()
{
    Poco::Logger &logger = Poco::Logger::get(kAppLogName);
    Poco::LogStream stream(logger);
    std::ifstream deviceFile(kDeviceFilePath);

    if (deviceFile.is_open())
    {
        boost::property_tree::ptree ptree;
        boost::property_tree::read_json(deviceFile, ptree);

        boost::optional<boost::property_tree::ptree&> array = ptree.get_child_optional("capabilities.optics.emission_channels");

        if (array)
            _settings.device.opticsChannels = array.get().size();
        else
            _settings.device.opticsChannels = 1;

        boost::property_tree::ptree::const_assoc_iterator it = ptree.find("serial_number");

        if (it != ptree.not_found())
            _settings.device.serialNumber = it->second.get_value<std::string>();

        it = ptree.find("model_number");

        if (it != ptree.not_found())
            _settings.device.modelNumber = it->second.get_value<std::string>();
    }
    else
        stream << "QPCRApplication::readDeviceFile - unable to read device file: " << std::strerror(errno) << std::endl;
}

void QPCRApplication::readConfigurationFile()
{
    Poco::Logger &logger = Poco::Logger::get(kAppLogName);
    Poco::LogStream stream(logger);
    std::ifstream deviceFile(kConfigurationFilePath);

    if (deviceFile.is_open())
    {
        boost::property_tree::ptree ptree;
        boost::property_tree::read_json(deviceFile, ptree);
        boost::property_tree::ptree::const_assoc_iterator softwareIt = ptree.find("software");

        if (softwareIt != ptree.not_found())
        {
            boost::property_tree::ptree::const_assoc_iterator it = softwareIt->second.find("version");

            if (it != softwareIt->second.not_found())
                _settings.configuration.version = it->second.get_value<std::string>();

            it = softwareIt->second.find("platform");

            if (it != softwareIt->second.not_found())
                _settings.configuration.platform = it->second.get_value<std::string>();
        }
    }
    else
        stream << "QPCRApplication::readConfigurationFile - unable to read configuration file: " << std::strerror(errno) << std::endl;
}

void QPCRApplication::initSignals() {
    sigemptyset(&_signalsSet);
    sigaddset(&_signalsSet, SIGQUIT);
    sigaddset(&_signalsSet, SIGINT);
    sigaddset(&_signalsSet, SIGTERM);
    sigprocmask(SIG_BLOCK, &_signalsSet, nullptr);
}

bool QPCRApplication::waitSignal() const {
    siginfo_t signalInfo;
    timespec time;

    time.tv_nsec = kAppSignalInterval;
    time.tv_sec = 0;

    return sigtimedwait(&_signalsSet, &signalInfo, &time) > 0;
}
