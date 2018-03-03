// RinggitTheSalesBell
// Rob Dobson 2018

#include "application.h"
#include "ConfigPinMap.h"

#if PLATFORM_ID == 6 // Photon
int ConfigPinMap::_pinMapD[] = {D0,D1,D2,D3,D4,D5,D6,D7};
int ConfigPinMap::_pinMapDLen = sizeof(ConfigPinMap::_pinMapD)/sizeof(int);
int ConfigPinMap::_pinMapA[] = {A0,A1,A2,A3,A4,A5,A6,A7};
int ConfigPinMap::_pinMapALen = sizeof(ConfigPinMap::_pinMapA)/sizeof(int);
#endif

#if PLATFORM_ID == 88 // RedBear Duo
int ConfigPinMap::_pinMapD[] = {D0,D1,D2,D3,D4,D5,D6,D7,D8,D9,D10,D11,D12,D13,D14,D15,D16,D17};
int ConfigPinMap::_pinMapDLen = sizeof(ConfigPinMap::_pinMapD)/sizeof(int);
int ConfigPinMap::_pinMapA[] = {A0,A1,A2,A3,A4,A5,A6,A7};
int ConfigPinMap::_pinMapALen = sizeof(ConfigPinMap::_pinMapA)/sizeof(int);
#endif
