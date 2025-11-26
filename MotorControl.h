#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

#include <Arduino.h>

class MotorControl {
  private:
    int pwmPinA;
    int pwmPinB;
    int currentSpeed;
    
    // Configurações de velocidade baseadas no valor de press (0-7)
    const int speedLevels[8] = {0, 40, 80, 120, 160, 200, 240, 255};
    
  public:
    MotorControl(int pinA, int pinB);
    void begin();
    void runSequence(int pressValue);
    void setSpeed(int speed);
    void stop();
    int getCurrentSpeed();
};

#endif