// RinggitTheSalesBell
// Bell control
// Rob Dobson 2018

#pragma once

#include "application.h"
#include "ConfigPinMap.h"
#include "SparkIntervalTimer.h"

static const int ledPin = D7;
volatile int __stepRun = false;
volatile int __stepCount = 0;
volatile int __stepNum = 0;
volatile bool __stepInProgress = false;
volatile int __stepIntvCount = 0;
volatile int __stepPin = 2;
volatile int __stepISRCount = 0;
volatile int __stepRateFactor = 1;
IntervalTimer _stepTimer;

class BellControl
{
private:
    static const int MAX_RINGS = 10;
    static const int RING_STEPS = 200;
    static const int RING_STEP_SCALING = 10;
    static const int RETURN_TO_IDLE_MS = 1000;
    static const bool STEP_ENABLE_ON = false;
    static const int STEP_ISR_BLINK_LED_COUNT = 1000;
    static const int STEP_ISR_RATE_US = 100;

    int _bellStepPin;
    int _bellDirnPin;
    int _bellEnablePin;
    bool _isRinging;
    int _ringPhase;
    int _ringsToDoCount;
    uint32_t phaseStartMs;

public:
    BellControl()
    {
        _bellStepPin = -1;
        _bellDirnPin = -1;
        _bellEnablePin = -1;
        _isRinging = false;
        _ringPhase = 0;
        _ringsToDoCount = 0;
        pinMode(ledPin, OUTPUT);
        _stepTimer.begin(timerFn, STEP_ISR_RATE_US, uSec);
    }

    void configure(const char* configStr)
    {
        // Handle bell pins
        _bellStepPin = -1;
        _bellDirnPin = -1;
        _bellEnablePin = -1;
        bool isValid = false;
        String bellStepPinName = RdJson::getString("bellStepPin", "-1", configStr, isValid);
        if (isValid)
        {
            _bellStepPin = ConfigPinMap::getPinFromName(bellStepPinName.c_str());
            if (_bellStepPin >= 0)
            {
                pinMode(_bellStepPin, OUTPUT);
                __stepPin = _bellStepPin;
            }
        }
        String bellDirnPinName = RdJson::getString("bellDirnPin", "-1", configStr, isValid);
        if (isValid)
        {
            _bellDirnPin = ConfigPinMap::getPinFromName(bellDirnPinName.c_str());
            if (_bellDirnPin >= 0)
                pinMode(_bellDirnPin, OUTPUT);
        }
        String bellEnablePinName = RdJson::getString("bellEnablePin", "-1", configStr, isValid);
        if (isValid)
        {
            _bellEnablePin = ConfigPinMap::getPinFromName(bellEnablePinName.c_str());
            if (_bellEnablePin >= 0)
            {
                pinMode(_bellEnablePin, OUTPUT);
                digitalWrite(_bellEnablePin, !STEP_ENABLE_ON);
            }
        }
        Log.info("BellControl: Step %s(%d), Dirn %s(%d), Enable %s(%d), Steps %d, StepUs %ld", bellStepPinName.c_str(), _bellStepPin,
                     bellDirnPinName.c_str(), _bellDirnPin, bellEnablePinName.c_str(), _bellEnablePin);
    }

    static void timerFn()
    {
        // Debug
        __stepISRCount++;
        if (__stepISRCount >= STEP_ISR_BLINK_LED_COUNT)
        {
            digitalWriteFast(ledPin,!pinReadFast(ledPin));
            __stepISRCount = 0;
        }
        // Check if active
        if (!__stepRun)
            return;
        // Finish bringing step line low if part way through a step command
        if (__stepInProgress)
        {
          digitalWriteFast(__stepPin, 0);
          __stepInProgress = false;
          return;
        }
        // Handle interval between Steps
        __stepIntvCount++;
        if (__stepIntvCount < __stepRateFactor)
            return;
        digitalWriteFast(__stepPin, 1);
        __stepInProgress = true;
        __stepIntvCount = 0;
        // See if stepping complete
        __stepCount++;
        if (__stepCount > __stepNum)
        {
            __stepRun = false;
            __stepCount = 0;
        }
    }

    void moveMotor(int dirn, int steps, int rateFactor)
    {
        Log.trace("BellControl: moveMotor dirn %d steps %d", dirn, steps);
        enableMotor(true);
        setDirection(dirn);
        __stepCount = 0;
        __stepNum = steps;
        __stepIntvCount = 0;
        __stepRun = true;
        __stepRateFactor = rateFactor;
    }

    void service()
    {
        // Check if ringing
        if (!_isRinging)
            return;

        if (!__stepRun)
        {
            // Next phase
            if (_ringPhase == 0)
            {
                // If we've rung already then disable motors to allow
                // motion to return to idle position
                enableMotor(false);
                Log.trace("BellControl: entering phase 1 at ms %ld", millis());
                _ringPhase = 1;
                phaseStartMs = millis();
            }
            else if (Utils::isTimeout(millis(), phaseStartMs, RETURN_TO_IDLE_MS))
            {
                _ringPhase = 0;
                // Completed that ring so check total rings
                _ringsToDoCount--;
                Log.trace("BellControl: ms %ld rings still to do %d", millis(), _ringsToDoCount);
                if (_ringsToDoCount <= 0)
                {
                    _ringsToDoCount = 0;
                    _isRinging = false;
                    enableMotor(false);
                    Log.trace("BellControl: Finished ringing");
                }
                else
                {
                    Log.trace("BellControl: starting next ring");
                    moveMotor(_ringPhase, RING_STEPS, RING_STEP_SCALING);
                }
            }
        }
    }

    void setDirection(int stepDirn)
    {
      if (_bellDirnPin != -1)
          digitalWrite(_bellDirnPin, stepDirn);
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
        Log.trace("BellControl: Enable motor pin %d level %d", _bellEnablePin, STEP_ENABLE_ON == en);
        if (_bellEnablePin != -1)
            digitalWrite(_bellEnablePin, STEP_ENABLE_ON == en);
    }

    bool ring(int numRings)
    {
        // Debug
        Log.trace("BellControl: Ring the bell %d times %s", numRings, _isRinging ? " already ringing" : "");
        // Ignore if ringing already
        if (_isRinging)
        {
            // Add rings
            _ringsToDoCount += numRings;
            if (_ringsToDoCount >= MAX_RINGS)
                _ringsToDoCount = MAX_RINGS;
            Log.trace("BellControl: Now ringing %d more times", _ringsToDoCount);
            return true;
        }
        _ringPhase = 0;
        _ringsToDoCount = numRings;
        if (_ringsToDoCount >= MAX_RINGS)
            _ringsToDoCount = MAX_RINGS;
        moveMotor(_ringPhase, RING_STEPS, RING_STEP_SCALING);
        _isRinging = true;
        return true;
    }

    bool getStatus(bool& isRinging)
    {
        isRinging = _isRinging;
        return true;
    }

};
