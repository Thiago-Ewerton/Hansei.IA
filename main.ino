#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

volatile bool buttonPressed = false;
unsigned int press = 0;

const int interruptPin = 2;  

#define SERVICE_UUID        "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define CHARACTERISTIC_UUID "6e400003-b5a3-f393-e0a9-e50e24dcca9e"

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

void IRAM_ATTR handleInterrupt() {
  buttonPressed = true;
}

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("✅ Dispositivo BLE conectado!");
  }
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("❌ Dispositivo BLE desconectado.");
  }
};

void setup() {
  Serial.begin(115200);

  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, FALLING);

  BLEDevice::init("ESP32_BLE_Interrupt");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_NOTIFY);
  pCharacteristic->addDescriptor(new BLE2902());
  pService->start();

  pServer->getAdvertising()->start();
  Serial.println("🔵 BLE pronto.");
}

void loop() {
  static unsigned int lastPress = 0;

  if ((buttonPressed) && (press < 7)){
    buttonPressed = false;
    press++;

    Serial.printf("Press: %u\n", press);

    if (deviceConnected) {
      String msg = "Press: " + String(press);
      pCharacteristic->setValue(msg.c_str());
      pCharacteristic->notify();
    }
  }
  else if ((buttonPressed) && (press = 7)){
     buttonPressed = false;
    press = 0;
    Serial.println("🔄 Resetando contagem para 0");

    if (deviceConnected) {
      pCharacteristic->setValue("Press: 0");
      pCharacteristic->notify();
    }
  }
  delay(50);
}
