#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <iostream>
#include <string>

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;

bool deviceConnected = false;
bool oldDeviceConnected = false;

uint32_t delayMS;

#define SERVICE_UUID  "95298e6a-6dbc-11ea-93d4-63607a02728e"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

int leedSwitch = 36;


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

  pinMode(leedSwitch, INPUT);
 

  BLEDevice::init("Sensor_Something");

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  //BLEService *pService = pServer->createService(SERVICE_UUID);
  BLEService *pService = pServer->createService((uint16_t)0x181A);

  pCharacteristic = pService->createCharacteristic(
                      (uint16_t)0x181A,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();

  pServer->getAdvertising()->start();
  Serial.println("Waiting for a client to connect...");
}

void loop() {

  // connected
  if (deviceConnected) {
    delay(1000);
 
    char dataString[12];
    int switchState = digitalRead(leedSwitch);
    sprintf(dataString,"%u", switchState);

    pCharacteristic->setValue(dataString);

    pCharacteristic->notify();
    Serial.print("*** Data String: ");
    Serial.print(dataString);
    Serial.println(" ***");
  }

  //  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("start advertising");
    oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }

}
