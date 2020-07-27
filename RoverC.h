#ifndef _ROVERC_H
#define _ROVERC_H

class RoverC
{
  public:
    RoverC();
    RoverC(TwoWire &w);

    void begin();
    void run();

    // control
    void stop();
    void move(int16_t speed, uint16_t duration = 0);
    void turn(int16_t speed, uint16_t duration = 0);
    void rotate(int16_t speed, uint16_t duration = 0);

    int16_t speed_buff[4];
    int8_t speed_sendbuff[4];
    uint32_t count;
    uint8_t IIC_ReState;
    uint8_t SendBuff[9];
    
  private:
    TwoWire *wire;
}

#endif // _ROVERC_H
