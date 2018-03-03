// RinggitTheSalesBell
// Status Display
// Rob Dobson 2018

#pragma once
#include <Adafruit_SSD1306.h>

// Status display Hardware
const StatusDisplay::DisplayType STATUS_DISPLAY_TYPE = StatusDisplay::DisplayType_SSD1306_128x64;
const int STATUS_DISPLAY_CLK = A3;
const int STATUS_DISPLAY_MOSI = A5;
const int STATUS_DISPLAY_DC = D6;
const int STATUS_DISPLAY_RST = DAC;
const int STATUS_DISPLAY_CS = A4;

class StatusDisplay
{
public:
    enum DisplayType
    {
            DisplayType_SSD1306_128x64,
    };
    StatusDisplay()
    {
        _pDisplay = NULL;
    };

    void init(DisplayType dispType, int clkPin, int mosiPin, int dcPin,
        int rstPin, int csPin)
    {
        if (dispType == DisplayType_SSD1306_128x64)
        {
            _pDisplay = new Adafruit_SSD1306(mosiPin, clkPin, dcPin, rstPin, csPin);
            _pDisplay->begin(SSD1306_SWITCHCAPVCC);
            _pDisplay->display();
        }
    }

private:
     Adafruit_SSD1306* _pDisplay;
};
