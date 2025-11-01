#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

volatile bool buttonPressed = false;
unsigned int press = 0;

const int interruptPin = 2;  

// UUIDs para o serviço e característica BLE
#define SERVICE_UUID        "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define CHARACTERISTIC_UUID "6e400003-b5a3-f393-e0a9-e50e24dcca9e"

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

// Função de interrupção para detectar o pressionamento do botão
void IRAM_ATTR handleInterrupt() {
  buttonPressed = true;
}

// Callbacks para conexão/desconexão do cliente BLE
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

  // Configura o pino do botão com pull-up interno e anexa a interrupção
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, FALLING);

  BLEDevice::init("ESP32_ESTESIdig");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Cria o serviço BLE e a característica 
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_NOTIFY);
  pCharacteristic->addDescriptor(new BLE2902());
  pService->start();
// Inicia a publicidade BLE
  pServer->getAdvertising()->start();
  Serial.println("🔵 BLE pronto.");
}

void loop() {
  // Variável estática para evitar múltiplas contagens por um único pressionamento
  static unsigned int lastPress = 0;

  // Verifica se o botão foi pressionado
  if ((buttonPressed) && (press < 7)){
    buttonPressed = false;
    press++;

    Serial.printf("Press: %u\n", press);

// Atualiza a característica BLE com o novo valor de press
    if (deviceConnected) {
      String msg = "Press: " + String(press);
      pCharacteristic->setValue(msg.c_str());
      pCharacteristic->notify();
    }
  }
  // Verifica se o contador atingiu 7 para resetar
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

