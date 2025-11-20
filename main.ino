#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

volatile bool buttonPressed = false;
volatile bool resetPressed = false;   // <-- nova interrupção

unsigned int press = 0;

const int interruptPin = 0;   // botão 1
const int resetPin = 1;       // <-- botão 2 (defina o pino que deseja)

// UUIDs para o serviço e característica BLE
#define SERVICE_UUID        "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define CHARACTERISTIC_UUID "6e400003-b5a3-f393-e0a9-e50e24dcca9e"

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

// Interrupção do botão 1
void IRAM_ATTR handleInterrupt() {
  buttonPressed = true;
}

// Interrupção do botão 2 (reset)
void IRAM_ATTR handleResetInterrupt() {
  resetPressed = true;
}

// Callbacks BLE
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

  // Configuração dos botões
  pinMode(interruptPin, INPUT_PULLUP);
  pinMode(resetPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(resetPin), handleResetInterrupt, FALLING);

  // BLE
  BLEDevice::init("ESP32_ESTESIdig");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_NOTIFY
  );

  pCharacteristic->addDescriptor(new BLE2902());
  pService->start();

  pServer->getAdvertising()->start();
  Serial.println("🔵 BLE pronto.");
}

void loop() {

  // Botão 1 → Contador
  if ((buttonPressed) && (press < 7)) {
    buttonPressed = false;
    press++;

    Serial.print(press);

    if (deviceConnected) {
      String msg = String(press);
      pCharacteristic->setValue(msg.c_str());
      pCharacteristic->notify();
    }
  }

  // Botão 1 → Reset automático ao chegar em 7
  else if (buttonPressed && press == 7) {
    buttonPressed = false;
    press = 0;

    Serial.println("🔄 Resetando contagem para 0");

    if (deviceConnected) {
      pCharacteristic->setValue("Press: 0");
      pCharacteristic->notify();
    }
  }

  // Botão 2 → Reset manual + mensagem "Enviado"
  if (resetPressed) {
    resetPressed = false;

    Serial.println("Enviado");

    if (deviceConnected) {
      pCharacteristic->setValue(String(press));
      pCharacteristic->setValue("Enviado");
      pCharacteristic->notify();
    }
    press = 0;
  }

  delay(50);
}
