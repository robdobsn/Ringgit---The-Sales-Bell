// RinggitTheSalesBell
// Rob Dobson 2018

// API used for particle cloud, web, UDP and BLE - very short to allow over BLE UART
//   Query status:      /Q                   - returns status including build version
//   Set WiFi:          /W/ssss/pppp         - ssss = ssid, pppp = password - assumes WPA2 - does not clear previous WiFi so clear first if required
//   Clear WiFi:        /WC                  - clears all stored SSID, etc
//   External antenna:  /WAX                 - external antenna for WiFi
//   Internal antenna:  /WAI                 - internal antenna for WiFi
//   Ring the bell:     /R/#                 - # is number of times to ring
//   Reset:             /RESET               - reset device

// To use through particle cloud
//   status varialble
//   exec function

// Settings for the bell motor
static const char* BELL_CONTROL_CONFIG =
      "{\"bellStepPin\":\"D2\",\"bellDirnPin\":\"D3\","
      "\"bellEnablePin\":\"A2\"}";

//#define USE_STATUS_DISPLAY 1
#define ENABLE_WEB_SERVER 1
#define ENABLE_UDP_REST_API_SERVER 1
#define INCLUDE_STM32F2_HW_WATCHDOG 1
#define ENABLE_CLOUD_SUPPORT 1

// Handle Redbear Duo platform - this supports Bluetooth Low Energy (BLE)
#if PLATFORM_ID == 88 // Duo
#define REDBEAR_DUO
#define INCLUDE_BLE_SUPPORT 1
#endif

// Main include for Particle applications
#include "application.h"

// Main log handler
SerialLogHandler logHandler(LOG_LEVEL_TRACE);

// Common utility functions
#include "Utils.h"

// Watchdog timer - uses hardware timer to ensure reset if main loop stalls
#ifdef INCLUDE_STM32F2_HW_WATCHDOG
#include "WatchdogTimerSTM32F2.h"
#endif
#include "RestAPIEndpoints.h"
#ifdef ENABLE_WEB_SERVER
#include "RdWebServer.h"
#endif
#ifdef ENABLE_UDP_REST_API_SERVER
#include "UdpRestApiServer.h"
#endif
#ifdef ENABLE_CLOUD_SUPPORT
#include "ParticleCloud.h"
#endif
#ifdef INCLUDE_BLE_SUPPORT
#include "BLEHandler.h"
#endif
#ifdef USE_STATUS_DISPLAY
#include "StatusDisplay.h"
#endif

// Resources for Web Server
#include "GenResourcesOv.h"

// Serial console - for configuration
#include "SerialConsole.h"

// Bell controller
#include "BellControl.h"

// Debug loop used to time main loop
#include "DebugLoopTimer.h"

/*
 * SYSTEM_MODE:
 *     - AUTOMATIC: Automatically try to connect to Wi-Fi and the Particle Cloud and handle the cloud messages.
 *     - SEMI_AUTOMATIC: Manually connect to Wi-Fi and the Particle Cloud, but automatically handle the cloud messages.
 *     - MANUAL: Manually connect to Wi-Fi and the Particle Cloud and handle the cloud messages.
 *
 * SYSTEM_MODE(AUTOMATIC) does not need to be called, because it is the default state.
 * However the user can invoke this method to make the mode explicit.
 * Learn more about system modes: https://docs.particle.io/reference/firmware/photon/#system-modes .
 */
SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(AUTOMATIC);
STARTUP(System.enableFeature(FEATURE_RESET_INFO));

// Debug loop timer and callback function
void debugLoopInfoCallback(String& infoStr)
{
  String ipAddr = WiFi.localIP();
  infoStr = String::format(" IP %s FW %s RST %d", ipAddr.c_str(),
            System.version().c_str(), System.resetReason());
}
DebugLoopTimer debugLoopTimer(10000, debugLoopInfoCallback);

// Particle Cloud
#ifdef ENABLE_CLOUD_SUPPORT
ParticleCloud* pParticleCloud = NULL;
#endif

// REST API Endpoints
RestAPIEndpoints restAPIEndpoints;

// Web server and UDP server
#ifdef ENABLE_WEB_SERVER
int webServerPort = 80;
RdWebServer* pWebServer = NULL;
#endif
#ifdef ENABLE_UDP_REST_API_SERVER
int udpRestApiServerPort = 7193;
UdpRestApiServer* pUdpRestApiServer = NULL;
#endif

// Bell
BellControl* pBellControl = NULL;

// BLE
#ifdef INCLUDE_BLE_SUPPORT
BLEHandler* pBLEHandler = NULL;
// BLECallbackType BLEHandler::_pBLERxCallback = NULL;
#endif

// Status display
#ifdef USE_STATUS_DISPLAY
StatusDisplay statusDisplay;
#endif

// Rest API Implementations
#include "RestAPIUtils.h"
#include "RestAPISystem.h"
#include "RestAPIBellControl.h"
#include "RestAPINetwork.h"
#include "RestAPIHelpers.h"

// Serial console
SerialConsole serialConsole(0, handleReceivedApiStr);

void setupRestAPIEndpoints()
{
    setupRestAPI_System();
    setupRestAPI_BellControl();
    setupRestAPI_Network();
    setupRestAPI_Helpers();
}

void setup()
{
    // Status display
#ifdef USE_STATUS_DISPLAY
    statusDisplay.init(STATUS_DISPLAY_TYPE, STATUS_DISPLAY_CLK, STATUS_DISPLAY_MOSI,
            STATUS_DISPLAY_DC, STATUS_DISPLAY_RST, STATUS_DISPLAY_CS);
#endif

    // Start the watchdog timer
#if INCLUDE_STM32F2_HW_WATCHDOG
    WatchdogTimerSTM32F2::init();
#endif

    // Initial message
    Serial.begin(115200);
    delay(1000);
    String systemName = "Ringgit";
    Log.info("%s (built %s %s)", systemName.c_str(), __DATE__, __TIME__);

    // Particle Cloud
#ifdef ENABLE_CLOUD_SUPPORT
    pParticleCloud = new ParticleCloud(handleReceivedApiStr,
                    restHelper_ReportHealth, restHelper_ReportHealthHash,
                    500, systemName);
    pParticleCloud->registerVariables();
#endif

    // Web server
#ifdef ENABLE_WEB_SERVER
    pWebServer = new RdWebServer();
#endif
#ifdef ENABLE_UDP_REST_API_SERVER
    pUdpRestApiServer = new UdpRestApiServer(handleReceivedApiStr);
#endif

    // Bell control
    pBellControl = new BellControl();
    pBellControl->configure(BELL_CONTROL_CONFIG);

    // BLE Handler - BLE is not started yet as WiFi needs to be started first on RedBear Duo it seems
#ifdef INCLUDE_BLE_SUPPORT
    pBLEHandler = new BLEHandler(handleReceivedApiStr);
#endif

    // Setup REST API endpoints
    setupRestAPIEndpoints();

    // Web server
#ifdef ENABLE_WEB_SERVER
    if (pWebServer)
    {
        // Add resources to web server
        pWebServer->addStaticResources(genResourcesOv, genResourcesOvCount);
        pWebServer->addRestAPIEndpoints(&restAPIEndpoints);
        // Start the web server
        pWebServer->start(webServerPort);
    }
#endif
#ifdef ENABLE_UDP_REST_API_SERVER
    if (pUdpRestApiServer)
        pUdpRestApiServer->start(udpRestApiServerPort);
#endif

    // Inform the BLE handler that setup is complete
#ifdef INCLUDE_BLE_SUPPORT
    if (pBLEHandler)
        pBLEHandler->setupComplete();
#endif

    // Add debug blocks
    debugLoopTimer.blockAdd(0, "Watchdog");
    debugLoopTimer.blockAdd(1, "Serial");
    debugLoopTimer.blockAdd(2, "Cloud");
    debugLoopTimer.blockAdd(3, "Web");
    debugLoopTimer.blockAdd(4, "UDP");
    debugLoopTimer.blockAdd(5, "Bell");
    debugLoopTimer.blockAdd(6, "BLE");
}

void loop()
{
#ifdef INCLUDE_STM32F2_HW_WATCHDOG
    // Kick the hardware watchdog
    debugLoopTimer.blockStart(0);
    WatchdogTimerSTM32F2::kick();
    debugLoopTimer.blockEnd(0);
#endif

    // Serial console
    debugLoopTimer.blockStart(1);
    serialConsole.service();
    debugLoopTimer.blockEnd(1);

    // Service the particle cloud
#ifdef ENABLE_CLOUD_SUPPORT
    debugLoopTimer.blockStart(2);
    if (pParticleCloud)
        pParticleCloud->service();
    debugLoopTimer.blockEnd(2);
#endif

    // Debug loop Timing
    debugLoopTimer.service();

    // Service web server
#ifdef ENABLE_WEB_SERVER
    debugLoopTimer.blockStart(3);
    if (pWebServer)
        pWebServer->service();
    debugLoopTimer.blockEnd(3);
#endif
#ifdef ENABLE_UDP_REST_API_SERVER
    debugLoopTimer.blockStart(4);
    if (pUdpRestApiServer)
        pUdpRestApiServer->service();
    debugLoopTimer.blockEnd(4);
#endif

    // Service the bell control
    debugLoopTimer.blockStart(6);
    if (pBellControl)
        pBellControl->service();
    debugLoopTimer.blockEnd(6);

    // Service BLE
#ifdef INCLUDE_BLE_SUPPORT
    debugLoopTimer.blockStart(7);
    if (pBLEHandler)
        pBLEHandler->service();
    debugLoopTimer.blockEnd(7);
#endif

}
