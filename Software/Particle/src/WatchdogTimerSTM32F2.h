// RinggitTheSalesBell
// Rob Dobson 2018

#pragma once

// #define TEST_WATCHDOG 1
#include "application.h"

class WatchdogTimerSTM32F2
{
  public:
    // Setting the prescaler to 256 results in approx 8ms per reload count
    // i.e. if reload value is 0xfff == 4095 then the watchdog will reset after
    // around 36 seconds
    static const uint8_t RWDT_PRESCALE = IWDG_Prescaler_256;
    static const uint16_t RWDT_RELOAD = 0xfff;
    static void init()
    {
      IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
      IWDG_SetPrescaler(RWDT_PRESCALE);
      IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
      IWDG_SetReload(RWDT_RELOAD);   // This is the max value
      IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
      IWDG_ReloadCounter();
      IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
      IWDG_Enable();
    }

    static void kick()
    {
      #ifndef TEST_WATCHDOG
        IWDG_ReloadCounter();
      #endif
    }
};
