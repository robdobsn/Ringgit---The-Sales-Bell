// RinggitTheSalesBell
// BLE Handler
// Rob Dobson 2018

#pragma once

const int START_BLE_AFTER_SETUP_MS = 10000;

//#define BLE_DEBUGGING_TO_SERIAL 1
//#define BLE_ERRORS_TO_SERIAL 1

typedef void (*BLECallbackType)(const char*cmdStr, String& retStr);
const int MAX_RX_DATA_LEN = 30;
char receiveBuffer[MAX_RX_DATA_LEN];

/*
 * BLE peripheral preferred connection parameters:
 *     - Minimum connection interval = MIN_CONN_INTERVAL * 1.25 ms, where MIN_CONN_INTERVAL ranges from 0x0006 to 0x0C80
 *     - Maximum connection interval = MAX_CONN_INTERVAL * 1.25 ms,  where MAX_CONN_INTERVAL ranges from 0x0006 to 0x0C80
 *     - The SLAVE_LATENCY ranges from 0x0000 to 0x03E8
 *     - Connection supervision timeout = CONN_SUPERVISION_TIMEOUT * 10 ms, where CONN_SUPERVISION_TIMEOUT ranges from 0x000A to 0x0C80
 */
#define MIN_CONN_INTERVAL          0x0028 // 50ms.
#define MAX_CONN_INTERVAL          0x0190 // 500ms.
#define SLAVE_LATENCY              0x0000 // No slave latency.
#define CONN_SUPERVISION_TIMEOUT   0x03E8 // 10s.

// Learn about appearance: http://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.gap.appearance.xml
#define BLE_PERIPHERAL_APPEARANCE  BLE_APPEARANCE_UNKNOWN

#define BLE_DEVICE_NAME            "Ringgit"

#define CHARACTERISTIC1_MAX_LEN    20
#define CHARACTERISTIC2_MAX_LEN    20
#define TXRX_BUF_LEN               20

/******************************************************
 *               Variable Definitions
 ******************************************************/
//static uint8_t UARTServiceUUID[16]    = { 0x71,0x3d,0x00,0x00,0x50,0x3e,0x4c,0x75,0xba,0x94,0x31,0x48,0xf1,0x8d,0x94,0x1e };
//static uint8_t UARTServiceTXCharacteristicUUID[16] = { 0x71,0x3d,0x00,0x03,0x50,0x3e,0x4c,0x75,0xba,0x94,0x31,0x48,0xf1,0x8d,0x94,0x1e };
//static uint8_t UARTServiceRXCharacteristicUUID[16] = { 0x71,0x3d,0x00,0x02,0x50,0x3e,0x4c,0x75,0xba,0x94,0x31,0x48,0xf1,0x8d,0x94,0x1e };

const uint16_t UARTServiceShortUUID                 = 0x0001;
const uint16_t UARTServiceTXCharacteristicShortUUID = 0x0002;
const uint16_t UARTServiceRXCharacteristicShortUUID = 0x0003;
static uint8_t UARTServiceUUID[16] =
    { 0x6E, 0x40,
    (uint8_t)(UARTServiceShortUUID >> 8),
    (uint8_t)(UARTServiceShortUUID & 0xFF),
    0xB5, 0xA3, 0xF3, 0x93,
    0xE0, 0xA9, 0xE5, 0x0E, 0x24, 0xDC, 0xCA, 0x9E };
static uint8_t UARTServiceTXCharacteristicUUID[16] =
    { 0x6E, 0x40,
    (uint8_t)(UARTServiceTXCharacteristicShortUUID >> 8),
    (uint8_t)(UARTServiceTXCharacteristicShortUUID & 0xFF),
    0xB5, 0xA3, 0xF3, 0x93,
    0xE0, 0xA9, 0xE5, 0x0E, 0x24, 0xDC, 0xCA, 0x9E };
static uint8_t UARTServiceRXCharacteristicUUID[16] =
    { 0x6E, 0x40,
    (uint8_t)(UARTServiceRXCharacteristicShortUUID >> 8),
    (uint8_t)(UARTServiceRXCharacteristicShortUUID & 0xFF),
    0xB5, 0xA3, 0xF3, 0x93,
    0xE0, 0xA9, 0xE5, 0x0E, 0x24, 0xDC, 0xCA, 0x9E };
//static const uint8_t UARTServiceUUID_reversed[LENGTH_OF_LONG_UUID] = {
//    0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0,
//    0x93, 0xF3, 0xA3, 0xB5, (uint8_t)(UARTServiceShortUUID & 0xFF), (uint8_t)(UARTServiceShortUUID >> 8), 0x40, 0x6E
//};

// GAP and GATT characteristics value
static uint8_t  appearance[2] = {
    LOW_BYTE(BLE_PERIPHERAL_APPEARANCE),
    HIGH_BYTE(BLE_PERIPHERAL_APPEARANCE)
};

static uint8_t  change[4] = {
    0x00, 0x00, 0xFF, 0xFF
};

static uint8_t  conn_param[8] = {
    LOW_BYTE(MIN_CONN_INTERVAL), HIGH_BYTE(MIN_CONN_INTERVAL),
    LOW_BYTE(MAX_CONN_INTERVAL), HIGH_BYTE(MAX_CONN_INTERVAL),
    LOW_BYTE(SLAVE_LATENCY), HIGH_BYTE(SLAVE_LATENCY),
    LOW_BYTE(CONN_SUPERVISION_TIMEOUT), HIGH_BYTE(CONN_SUPERVISION_TIMEOUT)
};

/*
 * BLE peripheral advertising parameters:
 *     - advertising_interval_min: [0x0020, 0x4000], default: 0x0800, unit: 0.625 msec
 *     - advertising_interval_max: [0x0020, 0x4000], default: 0x0800, unit: 0.625 msec
 *     - advertising_type:
 *           BLE_GAP_ADV_TYPE_ADV_IND
 *           BLE_GAP_ADV_TYPE_ADV_DIRECT_IND
 *           BLE_GAP_ADV_TYPE_ADV_SCAN_IND
 *           BLE_GAP_ADV_TYPE_ADV_NONCONN_IND
 *     - own_address_type:
 *           BLE_GAP_ADDR_TYPE_PUBLIC
 *           BLE_GAP_ADDR_TYPE_RANDOM
 *     - advertising_channel_map:
 *           BLE_GAP_ADV_CHANNEL_MAP_37
 *           BLE_GAP_ADV_CHANNEL_MAP_38
 *           BLE_GAP_ADV_CHANNEL_MAP_39
 *           BLE_GAP_ADV_CHANNEL_MAP_ALL
 *     - filter policies:
 *           BLE_GAP_ADV_FP_ANY
 *           BLE_GAP_ADV_FP_FILTER_SCANREQ
 *           BLE_GAP_ADV_FP_FILTER_CONNREQ
 *           BLE_GAP_ADV_FP_FILTER_BOTH
 *
 * Note:  If the advertising_type is set to BLE_GAP_ADV_TYPE_ADV_SCAN_IND or BLE_GAP_ADV_TYPE_ADV_NONCONN_IND,
 *        the advertising_interval_min and advertising_interval_max should not be set to less than 0x00A0.
 */
static advParams_t adv_params =
{
    .adv_int_min   = 0x0030,
    .adv_int_max   = 0x0030,
    .adv_type      = BLE_GAP_ADV_TYPE_ADV_IND,
    .dir_addr_type = BLE_GAP_ADDR_TYPE_PUBLIC,
    .dir_addr      = {0,0,0,0,0,0},
    .channel_map   = BLE_GAP_ADV_CHANNEL_MAP_ALL,
    .filter_policy = BLE_GAP_ADV_FP_ANY
};

static uint8_t adv_data[] =
{
    0x02,
    BLE_GAP_AD_TYPE_FLAGS,
    BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE,

    0x09,
    BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME,
    'K','e','r','b','e','r','u','s',

    0x11,
    BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_COMPLETE,
    //  0x1e,0x94,0x8d,0xf1,0x48,0x31,0x94,0xba,0x75,0x4c,0x3e,0x50,0x00,0x00,0x3d,0x71
    0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0,
    0x93, 0xF3, 0xA3, 0xB5, (uint8_t)(UARTServiceShortUUID & 0xFF), (uint8_t)(UARTServiceShortUUID >> 8), 0x40, 0x6E
};

static uint16_t character1_handle = 0x0000;
static uint16_t character2_handle = 0x0000;
// static uint16_t character3_handle = 0x0000;

static uint8_t characteristic1_data[CHARACTERISTIC1_MAX_LEN] = { 0x01 };
static uint8_t characteristic2_data[CHARACTERISTIC2_MAX_LEN] = { 0x00 };

static btstack_timer_source_t characteristic2;

char rx_buf[TXRX_BUF_LEN];

// Chunked sending of response data
static const int MAX_RET_STR_LEN = 1000;
static char _pTxBuffer[MAX_RET_STR_LEN+1];
static int _txBufLen;
static int _txBufPos;
static const int MIN_MS_BETWEEN_CHUNK_SENDS = 50;
static unsigned long lastTimeChunkSent = 0;

class BLEHandler
{
private:
    unsigned long _setupCompleteMs;
    bool _isStarted;
public:
    static BLECallbackType _pBLERxCallback;
    BLEHandler(BLECallbackType pBLERxCallback)
    {
        _pBLERxCallback = pBLERxCallback;
        _pTxBuffer[0] = 0;
        _txBufLen = 0;
        _txBufPos = 0;
        _setupCompleteMs = 0;
        _isStarted = false;
    }

    void start()
    {
        Log.info("BLE start");

        #ifdef BLE_DEBUGGING_TO_SERIAL
        ble.debugLogger(true);
        #endif
        #ifdef BLE_ERRORS_TO_SERIAL
        ble.debugError(true);
        #endif
        // Initialize ble_stack.
        ble.init();

        Log.trace("BLE after init");

        // Register BLE callback functions
        ble.onConnectedCallback(deviceConnectedCallback);
        ble.onDisconnectedCallback(deviceDisconnectedCallback);
        ble.onDataWriteCallback(gattWriteCallback);

        Log.trace("BLE adding services and chars");

        // Add GAP service and characteristics
        ble.addService(BLE_UUID_GAP);
        ble.addCharacteristic(BLE_UUID_GAP_CHARACTERISTIC_DEVICE_NAME, ATT_PROPERTY_READ|ATT_PROPERTY_WRITE, (uint8_t*)BLE_DEVICE_NAME, sizeof(BLE_DEVICE_NAME));
        ble.addCharacteristic(BLE_UUID_GAP_CHARACTERISTIC_APPEARANCE, ATT_PROPERTY_READ, appearance, sizeof(appearance));
        ble.addCharacteristic(BLE_UUID_GAP_CHARACTERISTIC_PPCP, ATT_PROPERTY_READ, conn_param, sizeof(conn_param));

        // Add GATT service and characteristics
        ble.addService(BLE_UUID_GATT);
        ble.addCharacteristic(BLE_UUID_GATT_CHARACTERISTIC_SERVICE_CHANGED, ATT_PROPERTY_INDICATE, change, sizeof(change));

        // Add user defined service and characteristics
        ble.addService(UARTServiceUUID);
        character1_handle = ble.addCharacteristicDynamic(UARTServiceTXCharacteristicUUID, ATT_PROPERTY_NOTIFY|ATT_PROPERTY_WRITE|ATT_PROPERTY_WRITE_WITHOUT_RESPONSE, characteristic1_data, CHARACTERISTIC1_MAX_LEN);
        character2_handle = ble.addCharacteristicDynamic(UARTServiceRXCharacteristicUUID, ATT_PROPERTY_NOTIFY, characteristic2_data, CHARACTERISTIC2_MAX_LEN);

        // Set BLE advertising parameters
        ble.setAdvertisementParams(&adv_params);

        // // Set BLE advertising data
        ble.setAdvertisementData(sizeof(adv_data), adv_data);

        // BLE peripheral starts advertising now.
        ble.startAdvertising();
        Log.info("BLE start advertising");

        // set one-shot timer
        characteristic2.process = &characteristic2_notify;
        ble.setTimer(&characteristic2, 500);//100ms
        ble.addTimer(&characteristic2);
    }

    void setupComplete()
    {
        _setupCompleteMs = millis();
    }

    void service()
    {
        // Check if BLE needs to be started (and enough time has elapsed since setup complete)
        // Connect to BLE but only after a little while as it seems to fail if both WiFi and BLE started simultaneously
        if (!_isStarted && (Utils::isTimeout(millis(), _setupCompleteMs, START_BLE_AFTER_SETUP_MS)))
        {
            pBLEHandler->start();
            _isStarted = true;
        }

        // Check if data to be sent over BLE
        if (_txBufLen != 0)
        {
            // Leave some time between sends
            if ((lastTimeChunkSent == 0) || Utils::isTimeout(millis(), lastTimeChunkSent, MIN_MS_BETWEEN_CHUNK_SENDS))
            {
                int lenToTx = 0;
                for (int i = 0; i < TXRX_BUF_LEN; i++)
                {
                    if (_pTxBuffer[_txBufPos + i] == 0)
                    break;
                    rx_buf[i] = _pTxBuffer[_txBufPos + i];
                    lenToTx++;
                }
                ble.sendNotify(character2_handle, (uint8_t*)rx_buf, lenToTx);
                _txBufPos += lenToTx;
                if (_txBufPos >= _txBufLen)
                _txBufLen = 0;
                lastTimeChunkSent = millis();
            }
        }
    }

    static void deviceConnectedCallback(BLEStatus_t status, uint16_t handle)
    {
        switch (status)
        {
            case BLE_STATUS_OK:
                Log.info("BLEHandler: BLE connected");
                break;
            case BLE_STATUS_CONNECTION_ERROR:
                Log.info("BLEHandler: BLE connection error");
                break;
            default: break;
        }
    }

    static void deviceDisconnectedCallback(uint16_t handle)
    {
        Log.info("BLE Disconnected");
    }

    static int gattWriteCallback(uint16_t value_handle, uint8_t *buffer, uint16_t size)
    {
        Log.trace("gattWriteCallback: %04x", value_handle);

        if (character1_handle == value_handle)
        {
            memcpy(characteristic1_data, buffer, min(size,CHARACTERISTIC1_MAX_LEN));
            // Log.trace("BLE UART rx...");
            // for (uint8_t index = 0; index < min(size,CHARACTERISTIC1_MAX_LEN); index++)
            // {
            //     Log.trace("%04x ", characteristic1_data[index]);
            // }

            // Check if there is a callback to call
            if (_pBLERxCallback)
            {

                // Copy received data
                for (uint8_t index = 0; index < min(size,CHARACTERISTIC1_MAX_LEN); index++)
                {
                    receiveBuffer[index] = characteristic1_data[index];
                }
                // Null terminate
                receiveBuffer[min(size,CHARACTERISTIC1_MAX_LEN)] = '\0';
                // Callback
                String retStr;
                BLEHandler::_pBLERxCallback(receiveBuffer, retStr);

                // Put return string into buffer
                strncpy(_pTxBuffer, retStr.c_str(), MAX_RET_STR_LEN);
                _pTxBuffer[MAX_RET_STR_LEN] = 0;
                _txBufLen = strlen(_pTxBuffer);
                _txBufPos = 0;
                Log.trace("TxBuffer: ", _pTxBuffer);
                //        memset(rx_buf, 0x00, 20);

            }
        }
        return 0;
    }

    /*void m_uart_rx_handle() {   //update characteristic data
    ble.sendNotify(character2_handle, rx_buf, CHARACTERISTIC2_MAX_LEN);
    memset(rx_buf, 0x00,20);
    rx_state = 0;
}*/

    static void characteristic2_notify(btstack_timer_source_t *ts)
    {
        // reset
        ble.setTimer(ts, 200);
        ble.addTimer(ts);
    }
};
