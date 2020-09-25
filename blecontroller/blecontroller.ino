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

#define DEVICE_NAME         "IIDX Entry model" // or "SOUND VOLTEX controller Entry Model"
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

  // Add your button init code here
  pinMode(0, INPUT_PULLUP); // Button
  pinMode(16, OUTPUT); // LED

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


// Sensor A is for turntable in IIDX
// Sensor A and B are L and R knobs respectively for SDVX
uint8_t sensorA = 0, sensorB = 0;

/*
  IIDX:
    0x01 Button 1
    0x02 Button 2
    0x04 Button 3
    0x08 Button 4
    0x10 Button 5
    0x20 Button 6
    0x40 Button 7
    
  SDVX:
    0x01 BT-A
    0x02 BT-B
    0x04 BT-C
    0x08 BT-D
    0x10 FX-L
    0x20 FX-R
 */
uint8_t keyPress1 = 0;

/*
  IIDX:
    0x01 E1
    0x02 E2

  SDVX:
    0x01 Start
 */
uint8_t keyPress2 = 0;


void loop() {
    // Add your button update code here    
    if (digitalRead(0) == LOW) {
      digitalWrite(16, LOW);
      keyPress1 = 0x01;
    } else {
      digitalWrite(16, HIGH);
      keyPress1 &= ~0x01;
    }

    if (deviceConnected) {
        // Send key press updates as much as possible to the game.
        // Each notification contains two frames worth of updates.
        uint8_t values[10] = { 
          sensorA, sensorB, keyPress1, keyPress2, frameCounter++,
          sensorA, sensorB, keyPress1, keyPress2, frameCounter++,
        };

        pCharacteristic->setValue(values, 10);
        pCharacteristic->notify();
        
        delay(50); // TODO: Adjust for more optimal latency. Setting this too low will flood the Bluetooth stack, resulting in inputs being dropped/not sent in time
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
