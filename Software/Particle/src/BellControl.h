// RinggitTheSalesBell
// Bell control
// Rob Dobson 2018

#pragma once

#include "application.h"
#include "ConfigPinMap.h"

class BellControl
{
private:
    static const int RING_STEPS_DEFAULT = 900;
    static const uint32_t STEP_US_DEFAULT = 400;
    static const bool STEP_ENABLE_ON = false;

    int _bellStepPin;
    int _bellDirnPin;
    int _bellEnablePin;
    int _ringSteps;
    uint32_t _betweenStepsUs;
    uint32_t _lastStepTimeUs;
    bool _isRinging;
    int _ringPhase;
    int _ringStepCount;
    int _ringCount;
    int _numRings;

public:
    BellControl()
    {
        _bellStepPin = -1;
        _bellDirnPin = -1;
        _bellEnablePin = -1;
        _ringSteps = 0;
        _lastStepTimeUs = 0;
        _betweenStepsUs = 0;
        _isRinging = false;
        _ringPhase = 0;
        _ringStepCount = 0;
        _ringCount = 0;
        _numRings = 1;
    }

    void configure(const char* configStr)
    {
        // Handle bell pins
        _bellStepPin = -1;
        _bellDirnPin = -1;
        _bellEnablePin = -1;
        _ringSteps = 0;
        bool isValid = false;
        String bellStepPinName = RdJson::getString("bellStepPin", "-1", configStr, isValid);
        if (isValid)
        {
            _bellStepPin = ConfigPinMap::getPinFromName(bellStepPinName.c_str());
            if (_bellStepPin > 0)
                pinMode(_bellStepPin, OUTPUT);
        }
        String bellDirnPinName = RdJson::getString("bellDirnPin", "-1", configStr, isValid);
        if (isValid)
        {
            _bellDirnPin = ConfigPinMap::getPinFromName(bellDirnPinName.c_str());
            if (_bellDirnPin > 0)
                pinMode(_bellDirnPin, OUTPUT);
        }
        String bellEnablePinName = RdJson::getString("bellEnablePin", "-1", configStr, isValid);
        if (isValid)
        {
            _bellEnablePin = ConfigPinMap::getPinFromName(bellEnablePinName.c_str());
            if (_bellEnablePin > 0)
            {
                pinMode(_bellEnablePin, OUTPUT);
                digitalWrite(_bellEnablePin, !STEP_ENABLE_ON);
            }
        }
        _ringSteps = (int) RdJson::getLong("ringSteps", RING_STEPS_DEFAULT, configStr);
        _betweenStepsUs = RdJson::getLong("stepUs", STEP_US_DEFAULT, configStr);
        Log.info("BellControl: Step %s(%d), Dirn %s(%d), Enable %s(%d), Steps %d, StepUs %ld", bellStepPinName.c_str(), _bellStepPin,
                     bellDirnPinName.c_str(), _bellDirnPin, bellEnablePinName.c_str(), _bellEnablePin, _ringSteps, _betweenStepsUs);
    }

    void service()
    {
        // Check if ringing
        if (!_isRinging)
            return;

        // See if next step required
        if (!Utils::isTimeout(micros(), _lastStepTimeUs, _betweenStepsUs))
            return;

        // Process the next step of ringing
        if (_ringStepCount < _ringSteps)
        {
            for (int i = 0; i < 50; i++)
            {
              // See if next step required
                _ringStepCount++;
                stepMotor(_ringPhase);
                delayMicroseconds(500);
            }
            _lastStepTimeUs = micros();

        }
        else
        {
            // Next phase
            _ringStepCount = 0;
            if (_ringPhase == 0)
            {
                _ringPhase = 1;
            }
            else
            {
                _ringPhase = 0;
                // Completed that ring so check total rings
                _ringCount++;
                if (_ringCount >= _numRings)
                {
                    _isRinging = false;
                }
            }
        }
    }

    void stepMotor(int stepDirn)
    {
        // Log.trace("Step motor pin %d dirnPin %d dirn %d", _bellStepPin, _bellDirnPin, stepDirn);
        if (_bellDirnPin != -1)
            digitalWrite(_bellDirnPin, stepDirn);
        if (_bellStepPin != -1)
        {
            digitalWrite(_bellStepPin, 1);
            delayMicroseconds(1);
            digitalWrite(_bellStepPin, 0);
        }
    }

    void enableMotor(bool en)
    {
        Log.trace("Enable motor pin %d level %d", _bellEnablePin, STEP_ENABLE_ON == en);
        if (_bellEnablePin != -1)
            digitalWrite(_bellEnablePin, STEP_ENABLE_ON == en);
    }

    bool ring(int numRings)
    {
        // Debug
        Log.trace("Ring the bell %d times %s", numRings, _isRinging ? " already ringing" : "");
        // Ignore if ringing already
        if (_isRinging)
            return false;
        _numRings = numRings;
        _isRinging = true;
        enableMotor(true);
        _ringCount = 0;
        _ringStepCount = 0;
        _ringPhase = 0;
        return true;
    }

    bool getStatus(bool& isRinging)
    {
        isRinging = _isRinging;
        return true;
    }

};
