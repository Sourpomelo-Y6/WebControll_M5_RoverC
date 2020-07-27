#include "RoverC.h"

RoverC::RoverC() {
  wire = &Wire;
}

RoverC::RoverC(TwoWire &w) {
  wire = &w;
}

void RoverC::begin() 
{
  for(int i=0; i<4 ;i++)
  {
    speed_buff[i] = 0;
    speed_sendbuff[i] = 0;
  }
  count = 0;
  IIC_ReState = I2C_ERROR_NO_BEGIN;
  
  // Motor
  Setspeed(0, 0, 0); 

  loop_interval = 0;
  left_offset = 0;
  right_offset = 0;
  forward_offset = 0;

  SendBuff[0] = 0xAA; 
  SendBuff[1] = 0x55;
  SendBuff[2] = 0x00;
  SendBuff[3] = 0x00;
  SendBuff[4] = 0x00;
  SendBuff[5] = 0x00;
  SendBuff[6] = 0x00;
  SendBuff[7] = 0x00;
  SendBuff[8] = 0xee;
}

uint8_t RoverC::I2CWrite1Byte(uint8_t Addr, uint8_t Data)
{
    Wire.beginTransmission(0x38);
    Wire.write(Addr);
    Wire.write(Data);
    return Wire.endTransmission();
}

uint8_t RoverC::I2CWritebuff(uint8_t Addr, uint8_t *Data, uint16_t Length)
{
    Wire.beginTransmission(0x38);
    Wire.write(Addr);
    for (int i = 0; i < Length; i++)
    {
        Wire.write(Data[i]);
    }
    return Wire.endTransmission();
}

uint8_t RoverC::Setspeed(int16_t Vtx, int16_t Vty, int16_t Wt)
{
    Wt = (Wt > 100) ? 100 : Wt;
    Wt = (Wt < -100) ? -100 : Wt;

    Vtx = (Vtx > 100) ? 100 : Vtx;
    Vtx = (Vtx < -100) ? -100 : Vtx;
    Vty = (Vty > 100) ? 100 : Vty;
    Vty = (Vty < -100) ? -100 : Vty;

    Vtx = (Wt != 0) ? Vtx * (100 - abs(Wt)) / 100 : Vtx;
    Vty = (Wt != 0) ? Vty * (100 - abs(Wt)) / 100 : Vty;

    speed_buff[0] = Vty - Vtx - Wt;
    speed_buff[1] = Vty + Vtx + Wt;
    speed_buff[3] = Vty - Vtx + Wt;
    speed_buff[2] = Vty + Vtx - Wt;

    for (int i = 0; i < 4; i++)
    {
        speed_buff[i] = (speed_buff[i] > 100) ? 100 : speed_buff[i];
        speed_buff[i] = (speed_buff[i] < -100) ? -100 : speed_buff[i];
        speed_sendbuff[i] = speed_buff[i];
    }
    return I2CWritebuff(0x00, (uint8_t *)speed_sendbuff, 4);
}

void RoverC::run() {
  if (micros() >= loop_interval) {
    loop_interval = micros() + 10000;

    // Attitude sample
    imu->update();
    pitch = imu->getAngleX() + angle_offset;

    if (imu_id == MPU9250_ID)
      pitch = -pitch;
    // #ifndef MPU6050_IMU
    // pitch = -pitch;
    // #endif

    // Car down
    if (abs(pitch) > 45) {
      setMotor(0, 0);
      return;
    }

    // Encoder sample
    readEncder();

    // PID Compute
    PIDCompute();

    // Motor out
    setMotor(pwm_out0 + forward_offset + left_offset, 
         pwm_out1 + forward_offset + right_offset);
  }
}

void RoverC::stop() {
  left_offset = 0;
  right_offset = 0;
}

void RoverC::move(int16_t speed, uint16_t duration) {
  forward_offset = -speed;
  if (duration != 0) {
    delay(duration);
    stop();
  }
}

void RoverC::turn(int16_t speed, uint16_t duration) {
  if (speed > 0) {
    left_offset = speed;
    right_offset = 0;

  } else if (speed < 0) {
    left_offset = 0;
    right_offset = -speed;
  }

  if (duration != 0) {
    delay(duration);
    stop();
  }
}

void RoverC::rotate(int16_t speed, uint16_t duration) {
  left_offset = speed;
  right_offset = -speed;

  if (duration != 0) {
    delay(duration);
    stop();
  }
}
