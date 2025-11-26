#include "MotorControl.h"

// Construtor
MotorControl::MotorControl(int pinA, int pinB) {
  pwmPinA = pinA;
  pwmPinB = pinB;
  currentSpeed = 0;
}

// Inicialização do motor
void MotorControl::begin() {
  // Configura os pinos como saída
  pinMode(pwmPinA, OUTPUT);
  pinMode(pwmPinB, OUTPUT);
  
  // Para o motor inicialmente
  stop();
  
  Serial.println("🟢 MotorControl inicializado");
}

// Executa a sequência baseada no valor de press
void MotorControl::runSequence(int pressValue) {
  if (pressValue < 0 || pressValue > 7) {
    Serial.println("❌ Valor de press inválido. Deve ser entre 0 e 7.");
    return;
  }
  
  int speed = speedLevels[pressValue];
  setSpeed(speed);
  
  Serial.print("🎯 Press: ");
  Serial.print(pressValue);
  Serial.print(" | Velocidade: ");
  Serial.print(speed);
  Serial.print("/255 | ");
  
  // Feedback visual da intensidade
  for (int i = 0; i < pressValue; i++) {
    Serial.print("█");
  }
  for (int i = pressValue; i < 7; i++) {
    Serial.print("░");
  }
  Serial.println();
}

// Define a velocidade do motor (0-255)
void MotorControl::setSpeed(int speed) {
  speed = constrain(speed, 0, 255);
  currentSpeed = speed;
  
  if (speed == 0) {
    // Para o motor
    analogWrite(pwmPinA, 0);
    analogWrite(pwmPinB, 0);
  } else {
    // Sentido horário (PWM no pino A, pino B em LOW)
    analogWrite(pwmPinA, speed);
    digitalWrite(pwmPinB, LOW);
    
    // Alternativa para sentido anti-horário:
    // digitalWrite(pwmPinA, LOW);
    // analogWrite(pwmPinB, speed);
  }
}

// Para o motor completamente
void MotorControl::stop() {
  setSpeed(0);
  Serial.println("🛑 Motor parado");
}

// Retorna a velocidade atual
int MotorControl::getCurrentSpeed() {
  return currentSpeed;
}