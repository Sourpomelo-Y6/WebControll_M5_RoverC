#ifndef _ROVERC_H
#define _ROVERC_H

#include <Wire.h>

class RoverC
{
  public:
    RoverC();
    RoverC(TwoWire &w);

    void begin();
    void run();

    // control
    void stop();
    void move(int16_t x_speed,int16_t y_speed, uint16_t duration);
    void turn(int16_t speed, uint16_t duration = 0);
    void rotate(int16_t speed, uint16_t duration = 0);

    uint8_t Setspeed(int16_t Vtx, int16_t Vty, int16_t Wt);

    uint8_t I2CWrite1Byte(uint8_t Addr, uint8_t Data);
    uint8_t I2CWritebuff(uint8_t Addr, uint8_t *Data, uint16_t Length);
    
    int16_t x_val, y_val;
    int16_t yaw_val;
    int16_t loop_interval;

    int16_t speed_buff[4];
    int8_t speed_sendbuff[4];
    uint32_t count;
    uint8_t IIC_ReState;
    uint8_t SendBuff[9];
    
  private:
    TwoWire *wire;
};

#endif // _ROVERC_H
