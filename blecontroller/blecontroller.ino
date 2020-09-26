// Built for the esp32
// Requires https://github.com/madhephaestus/ESP32Encoder

#include <ESP32Encoder.h>

#include <BLECharacteristic.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define DEVICE_NAME         "IIDX Entry model" // or "SOUND VOLTEX controller Entry Model"
#define SERVICE_UUID        "ff00"
#define CHARACTERISTIC_UUID "ff01"

#define BUTTON_1      15
#define BUTTON_1_LED  2
#define BUTTON_2      0
#define BUTTON_2_LED  4
#define BUTTON_3      16
#define BUTTON_3_LED  17
#define BUTTON_4      5
#define BUTTON_4_LED  18
#define BUTTON_5      19
#define BUTTON_5_LED  21
#define BUTTON_6      22
#define BUTTON_6_LED  23
#define BUTTON_7      25
#define BUTTON_7_LED  26
#define BUTTON_8      32
#define BUTTON_8_LED  33
#define BUTTON_9      12
#define BUTTON_9_LED  14

#define SENSOR_A1     27
#define SENSOR_A2     13
#define SENSOR_B1     100 // TODO
#define SENSOR_B2     101 // TODO

// Sensor A is for turntable in IIDX
// Sensor A and B are L and R knobs respectively for SDVX
ESP32Encoder sensorA;
ESP32Encoder sensorB;

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

uint8_t frameCounter = 0;

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;

bool deviceConnected = false;
bool oldDeviceConnected = false;

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

  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);
  pinMode(BUTTON_3, INPUT_PULLUP);
  pinMode(BUTTON_4, INPUT_PULLUP);
  pinMode(BUTTON_5, INPUT_PULLUP);
  pinMode(BUTTON_6, INPUT_PULLUP);
  pinMode(BUTTON_7, INPUT_PULLUP);
  pinMode(BUTTON_8, INPUT_PULLUP);
  pinMode(BUTTON_9, INPUT_PULLUP);
  
  pinMode(BUTTON_1_LED, OUTPUT);
  pinMode(BUTTON_2_LED, OUTPUT);
  pinMode(BUTTON_3_LED, OUTPUT);
  pinMode(BUTTON_4_LED, OUTPUT);
  pinMode(BUTTON_5_LED, OUTPUT);
  pinMode(BUTTON_6_LED, OUTPUT);
  pinMode(BUTTON_7_LED, OUTPUT);
  pinMode(BUTTON_8_LED, OUTPUT);
  pinMode(BUTTON_9_LED, OUTPUT);

  ESP32Encoder::useInternalWeakPullResistors=UP;  
  sensorA.attachHalfQuad(SENSOR_A1, SENSOR_A2);
  sensorA.clearCount();
  
  sensorB.attachHalfQuad(SENSOR_B1, SENSOR_B2);
  sensorB.clearCount();

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
  
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
    // Add your button update code here    
    if (digitalRead(BUTTON_1) == LOW) {
      Serial.println("Pressed BUTTON_1");
      digitalWrite(BUTTON_1_LED, LOW);
      keyPress1 |= 0x01;
    } else {
      digitalWrite(BUTTON_1_LED, HIGH);
      keyPress1 &= ~0x01;
    }

    if (digitalRead(BUTTON_2) == LOW) {
      Serial.println("Pressed BUTTON_2");
      digitalWrite(BUTTON_2_LED, LOW);
      keyPress1 |= 0x02;
    } else {
      digitalWrite(BUTTON_2_LED, HIGH);
      keyPress1 &= ~0x02;
    }
    
    if (digitalRead(BUTTON_3) == LOW) {
      Serial.println("Pressed BUTTON_3");
      digitalWrite(BUTTON_3_LED, LOW);
      keyPress1 |= 0x04;
    } else {
      digitalWrite(BUTTON_3_LED, HIGH);
      keyPress1 &= ~0x04;
    }
    
    if (digitalRead(BUTTON_4) == LOW) {
      Serial.println("Pressed BUTTON_4");
      digitalWrite(BUTTON_4_LED, LOW);
      keyPress1 |= 0x08;
    } else {
      digitalWrite(BUTTON_4_LED, HIGH);
      keyPress1 &= ~0x08;
    }
    
    if (digitalRead(BUTTON_5) == LOW) {
      Serial.println("Pressed BUTTON_5");
      digitalWrite(BUTTON_5_LED, LOW);
      keyPress1 |= 0x10;
    } else {
      digitalWrite(BUTTON_5_LED, HIGH);
      keyPress1 &= ~0x10;
    }
    
    if (digitalRead(BUTTON_6) == LOW) {
      Serial.println("Pressed BUTTON_6");
      digitalWrite(BUTTON_6_LED, LOW);
      keyPress1 |= 0x20;
    } else {
      digitalWrite(BUTTON_6_LED, HIGH);
      keyPress1 &= ~0x20;
    }
    
    if (digitalRead(BUTTON_7) == LOW) {
      Serial.println("Pressed BUTTON_7");
      digitalWrite(BUTTON_7_LED, LOW);
      keyPress1 |= 0x40;
    } else {
      digitalWrite(BUTTON_7_LED, HIGH);
      keyPress1 &= ~0x40;
    }
    
    if (digitalRead(BUTTON_8) == LOW) {
      Serial.println("Pressed BUTTON_8");
      digitalWrite(BUTTON_8_LED, LOW);
      keyPress2 |= 0x01;
    } else {
      digitalWrite(BUTTON_8_LED, HIGH);
      keyPress2 &= ~0x01;
    }
    
    if (digitalRead(BUTTON_9) == LOW) {
      Serial.println("Pressed BUTTON_9");
      digitalWrite(BUTTON_9_LED, LOW);
      keyPress2 |= 0x02;
    } else {
      digitalWrite(BUTTON_9_LED, HIGH);
      keyPress2 &= ~0x02;
    }

    if (sensorA.getCount() > 255) {
      sensorA.clearCount();
    } else if (sensorA.getCount() < 0) {
      sensorA.setCount(255);
    }

    if (sensorB.getCount() > 255) {
      sensorB.clearCount();
    } else if (sensorB.getCount() < 0) {
      sensorA.setCount(255);
    }
  
    Serial.print("sensorA:  ");
    Serial.print((int32_t)sensorA.getCount());
    Serial.print(", sensorB:  ");
    Serial.println((int32_t)sensorB.getCount());

    if (deviceConnected) {
        // Send key press updates as much as possible to the game.
        // Each notification contains two frames worth of updates.
        uint8_t values[10] = { 
          sensorA.getCount(), sensorB.getCount(), keyPress1, keyPress2, frameCounter++,
          sensorA.getCount(), sensorB.getCount(), keyPress1, keyPress2, frameCounter++,
        };

        pCharacteristic->setValue(values, 10);
        pCharacteristic->notify();
        
        delay(25); // TODO: Adjust for more optimal latency. Setting this too low will flood the Bluetooth stack, resulting in inputs being dropped/not sent in time
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
