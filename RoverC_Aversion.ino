//StickC  "EAC"  works
//usbserial-95522B3EAC
//MAC: 94:b9:7e:8c:b9:7c
#include <Wire.h>
#include <M5StickCPlus.h>
#define ROVER_ADDRESS 0X38
#define VL53L0X_REG_IDENTIFICATION_MODEL_ID 0xc0
#define VL53L0X_REG_IDENTIFICATION_REVISION_ID 0xc2
#define VL53L0X_REG_PRE_RANGE_CONFIG_VCSEL_PERIOD 0x50
#define VL53L0X_REG_FINAL_RANGE_CONFIG_VCSEL_PERIOD 0x70
#define VL53L0X_REG_SYSRANGE_START 0x00
#define VL53L0X_REG_RESULT_INTERRUPT_STATUS 0x13
#define VL53L0X_REG_RESULT_RANGE_STATUS 0x14
#define ToF_ADDR 0x29  //the iic address of tof
byte gbuf[16];
uint16_t dist;

//declared variables
int fwd;
int back;
int turn_right;
int turn_left;
int left_front;
int right_front;
int left_rear;
int right_rear;
int angle;
int Speed;
int degree = 45;
bool sweepRight = true;
bool sweepLeft;

void setup() {
  M5.begin();
  delay(100);
  Wire.begin(0, 26, 100);
  delay(100);
  M5.Axp.ScreenBreath(30);
  M5.Lcd.setRotation(0);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(5, 0);  //horiz, vert
  M5.Lcd.print("Fwd=");
  M5.Lcd.setCursor(5, 25);  //horiz, vert
  M5.Lcd.print("Left=");
  M5.Lcd.setCursor(5, 50);  //horiz, vert
  M5.Lcd.print("Right=");
  M5.Lcd.setCursor(5, 75);  //horiz, vert
  M5.Lcd.print("angle=");
  M5.Lcd.setCursor(5, 100);  //horiz, vert
  M5.Lcd.print("dist=");

  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(5, 130);  //horiz, vert
  M5.Lcd.print("LF");
  M5.Lcd.setCursor(5, 160);  //horiz, vert
  M5.Lcd.print("RF");

  M5.Lcd.setCursor(5, 190);  //horiz, vert
  M5.Lcd.print("LR");
  M5.Lcd.setCursor(5, 220);  //horiz, vert
  M5.Lcd.print("RR");

  delay(500);
}

void loop() {
  M5.update();
  MapDisplay();
  movement();
}


void movement() {
  Servosweep();
  measure_distance();

  if (dist < 200 && (degree >= 40 && degree <= 50)) {
    fwd = -25;
    turn_left = 10;
  } else if (dist > 200 && (degree >= 40 && degree <= 50)) {
    fwd = 35;
  }

  if (fwd > 0) {

    if (dist < 300 && degree < 40 && fwd > 0) {
      turn_left = 20;
    } else {
      turn_left = 0;
    }

    if (dist < 300 && degree > 50 && fwd > 0) {
      turn_right = 20;
    } else {
      turn_right = 0;
    }
  }
  left_front = fwd + turn_right - turn_left;
  right_front = fwd - turn_right + turn_left;
  left_rear = fwd + turn_right - turn_left;
  right_rear = fwd - turn_right + turn_left;
  Send_iic(0x00, left_front);   //Left Front
  Send_iic(0x01, right_front);  //Right Front
  Send_iic(0x02, left_rear);    //Left Rear
  Send_iic(0x03, right_rear);   //Right Rear
}

void Servosweep() {

  if (sweepRight && degree > 0) {  //towards 0, angle is to be decreasing
    degree = degree - 5;
    Send_iic(0x11, degree);  //0 is to the right
  } else {
    sweepRight = false;
    sweepLeft = true;
  }

  if (sweepLeft && degree < 90) {  //towards 90, angle is to be increasing
    degree = degree + 5;
    Send_iic(0x11, degree);  //0 is to the right
  } else {
    sweepLeft = false;
    sweepRight = true;
  }
}

void measure_distance() {
  write_byte_data_at(VL53L0X_REG_SYSRANGE_START, 0x01);
  read_block_data_at(VL53L0X_REG_RESULT_RANGE_STATUS, 12);
  dist = makeuint16(gbuf[11], gbuf[10]);  //split distance data to "dist"
  byte DeviceRangeStatusInternal = ((gbuf[0] & 0x78) >> 3);

  if (dist > 25 && dist < 900) {
    dist = dist;
  } else {
    dist = 900;
  }
  Serial.print("dist= ");
  Serial.println(dist);
  Serial.println("   ");
}

void MapDisplay() {
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(80, 0);
  M5.Lcd.fillRect(80, 0, 100, 20, BLACK);
  M5.Lcd.print(fwd);

  M5.Lcd.setCursor(80, 25);
  M5.Lcd.fillRect(80, 25, 100, 20, BLACK);
  M5.Lcd.print(turn_left);

  M5.Lcd.setCursor(80, 50);
  M5.Lcd.fillRect(80, 50, 100, 20, BLACK);
  M5.Lcd.print(turn_right);

  M5.Lcd.setCursor(80, 75);
  M5.Lcd.fillRect(80, 75, 100, 20, BLACK);
  M5.Lcd.print(degree);

  M5.Lcd.setCursor(80, 100);
  M5.Lcd.fillRect(80, 100, 100, 20, BLACK);
  M5.Lcd.print(dist);


  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(80, 130);
  M5.Lcd.fillRect(80, 130, 100, 20, BLACK);
  M5.Lcd.print(left_front);

  M5.Lcd.setCursor(80, 160);
  M5.Lcd.fillRect(80, 160, 100, 20, BLACK);
  M5.Lcd.print(right_front);

  M5.Lcd.setCursor(80, 190);
  M5.Lcd.fillRect(80, 190, 100, 20, BLACK);
  M5.Lcd.print(left_rear);

  M5.Lcd.setCursor(80, 220);
  M5.Lcd.fillRect(80, 220, 100, 20, BLACK);
  M5.Lcd.print(right_rear);
}

uint16_t bswap(byte b[]) {
  // Big Endian unsigned short to little endian unsigned short
  uint16_t val = ((b[0] << 8) & b[1]);
  return val;
}

uint16_t makeuint16(int lsb, int msb) {
  return ((msb & 0xFF) << 8) | (lsb & 0xFF);
}

uint16_t VL53L0X_decode_vcsel_period(short vcsel_period_reg) {
  // Converts the encoded VCSEL period register value into the real
  // period in PLL clocks
  uint16_t vcsel_period_pclks = (vcsel_period_reg + 1) << 1;
  return vcsel_period_pclks;
}

void write_byte_data(byte data) {
  Wire.beginTransmission(ToF_ADDR);
  Wire.write(data);
  Wire.endTransmission();
}

void write_byte_data_at(byte reg, byte data) {
  Wire.beginTransmission(ToF_ADDR);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

void write_word_data_at(byte reg, uint16_t data) {
  byte b0 = (data & 0xFF);
  byte b1 = ((data >> 8) && 0xFF);

  Wire.beginTransmission(ToF_ADDR);
  Wire.write(reg);
  Wire.write(b0);
  Wire.write(b1);
  Wire.endTransmission();
}

byte read_byte_data() {
  Wire.requestFrom(ToF_ADDR, 1);
  while (Wire.available() < 1) delay(1);
  byte b = Wire.read();
  return b;
}

byte read_byte_data_at(byte reg) {
  //write_byte_data((byte)0x00);
  write_byte_data(reg);
  Wire.requestFrom(ToF_ADDR, 1);
  while (Wire.available() < 1) delay(1);
  byte b = Wire.read();
  return b;
}

uint16_t read_word_data_at(byte reg) {
  write_byte_data(reg);
  Wire.requestFrom(ToF_ADDR, 2);
  while (Wire.available() < 2) delay(1);
  gbuf[0] = Wire.read();
  gbuf[1] = Wire.read();
  return bswap(gbuf);
}

void read_block_data_at(byte reg, int sz) {
  int i = 0;
  write_byte_data(reg);
  Wire.requestFrom(ToF_ADDR, sz);
  for (i = 0; i < sz; i++) {
    while (Wire.available() < 1) delay(1);
    gbuf[i] = Wire.read();
  }
}

void Send_iic(uint8_t Register, uint8_t Speed) {
  Wire.beginTransmission(ROVER_ADDRESS);
  Wire.write(Register);
  Wire.write(Speed);
  Wire.endTransmission();
}
