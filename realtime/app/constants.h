#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include <vector>

//Steinhart-Hart coefficients for heat block thermistor (QTI Z Curve)
const double kQTICurveZThermistorACoefficient = 0.001116401465500;
const double kQTICurveZThermistorBCoefficient = 0.000237982973213;
const double kQTICurveZThermistorCCoefficient = -0.000000372283234;
const double kQTICurveZThermistorDCoefficient = 0.000000099063233;

//Lid
const unsigned long kLidPIDMin = 0;
const unsigned long kLidPIDMax = 1;
const unsigned long kLidPWMPeriodNs = 500000;

const double kLidThermistorBetaCoefficient = 3970;  //kelvins
const double kLidThermistorT0Resistance = 10000;    //ohms
const double kLidThermistorT0 = 298.15;             //kelvins

const double kLidMinTargetTemp = 0;
const double kLidMaxTargetTemp = 130;

const double kProgramStartLidTempThreshold = 2;

//PID
const int kPIDDerivativeGainLimiter = 12;

//ADC
const int kADCRepeatFrequency = 80; // Hz

//thermistor & ADC params
const unsigned int kLidThermistorVoltageDividerResistanceOhms = 33000;
const int kLTC2444ADCBits = 24;
const long kHeatSinkADCInterval = 1000;

//HTTP server params
const int kHttpServerPort = 8000;

//Fan PWM params
const unsigned long kFanPWMPeriodNs = 500000;

//Heat Block params
const unsigned int kHeatBlockThermistorVoltageDividerResistanceOhms = 33000;
const unsigned long kHeatBlockZone1PWMPeriodNs = 100000;
const unsigned long kHeatBlockZone2PWMPeriodNs = 100000;

const double kHeatBlockZonesPIDMin = -1;
const double kHeatBlockZonesPIDMax = 1;

const double kHeatBlockZonesMinTargetTemp = -10;
const double kHeatBlockZonesMaxTargetTemp = 105;

const double kMaxHeatBlockRampSpeed = 5.0;

//LED constants
const int kMinLEDCurrent = 5; //mA
const int kDefaultLEDCurrent = 60; //mA

const int kLedBlankPwmPeriodNs = 1000000;
const int kLedBlankPwmDutyNs = 500000;

//Optics
const long kFluorescenceDataCollectionDelayTimeMs = 70;
const std::vector<int> kWellList = {4, 3, 2, 1, 16, 15, 14, 13, 5, 8, 7, 6, 11, 10, 9, 12};

//Steps
const double kPCRBeginStepTemperatureThreshold = 0.5;

//Experiment Controller
const long kTemperatureLoggerInterval = 1000;
const long kTemperatureLoggerFlushInterval = 10000; //ms

//Heat Sink
const double kHeatSinkTargetTemperature = 38; //C
const double kHeatSinkThermistorBetaCoefficient = 3970;  //kelvins
const double kHeatSinkThermistorT0Resistance = 10000;    //ohms
const double kHeatSinkThermistorT0 = 298.15;             //kelvins
const unsigned int kHeatSinkThermistorVoltageDividerResistanceOhms = 6800;

const double kHeatSinkMinTargetTemp = 0;
const double kHeatSinkMaxTargetTemp = 80;

const double kHeatSinkPIDMin = -1;
const double kHeatSinkPIDMax = 0;

//beaglebone
const unsigned int kBeagleboneADCBits = 12;

//App
const long kAppSignalInterval = 5 * 1000 * 1000; //Nanosec

#endif
