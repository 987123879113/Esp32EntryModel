#include <BLECharacteristic.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;

bool deviceConnected = false;
bool oldDeviceConnected = false;

uint8_t frameCounter = 0;

#define DEVICE_NAME         "IIDX Entry model"
#define SERVICE_UUID        "ff00"
#define CHARACTERISTIC_UUID "ff01"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

void setup() {
  Serial.begin(115200);

  // Create the BLE Device
  BLEDevice::init(DEVICE_NAME);

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  BLEDevice::startAdvertising();
  
  frameCounter = 0;
  Serial.println("Waiting a client connection to notify...");
}


uint8_t turntableValue = 0;

/*
  0x01 Button 1
  0x02 Button 2
  0x04 Button 3
  0x08 Button 4
  0x10 Button 5
  0x20 Button 6
  0x40 Button 7
 */
uint8_t keyPress1 = 0;

/*
  0x01 E1
  0x02 E2
 */
uint8_t keyPress2 = 0;


uint32_t keyAnim = 0;
uint8_t keyValues[] = {
  0x00, // 0
  0x01, // 1
  0x01 | 0x02,  // 2
  0x01 | 0x02 | 0x04,  // 3
  0x01 | 0x02 | 0x04 | 0x08,  // 4
  0x01 | 0x02 | 0x04 | 0x08 | 0x10, // 5
  0x01 | 0x02 | 0x04 | 0x08 | 0x10 | 0x20,  // 6
  0x01 | 0x02 | 0x04 | 0x08 | 0x10 | 0x20 | 0x40, // 7
  0x01 | 0x02 | 0x04 | 0x08 | 0x10 | 0x20,  // 8
  0x01 | 0x02 | 0x04 | 0x08 | 0x10, // 9
  0x01 | 0x02 | 0x04 | 0x08,  // 10
  0x01 | 0x02 | 0x04,  // 11
  0x01 | 0x02,  // 12
  0x01, // 13
};

void loop() {
    if (deviceConnected) {
        // Send key press updates as much as possible to the game.
        // Each notification contains two frames worth of updates.
        uint8_t values[10] = { 
          turntableValue, 0x00, keyPress1, keyPress2, frameCounter++,
          turntableValue, 0x00, keyPress1, keyPress2, frameCounter++,
        };

        if ((frameCounter % 30) == 0) {
          keyPress1 = keyValues[keyAnim];

          keyAnim++;
          
          if (keyAnim > 13) {
            keyAnim = 0;
          }
        }
        
        pCharacteristic->setValue(values, 10);
        pCharacteristic->notify();
        
        delay(1);
    }
    
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
        frameCounter = 0;
    }
    
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
        frameCounter = 0;
    }
}
