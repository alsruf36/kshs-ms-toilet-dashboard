#include<Wire.h>
#include <SoftwareSerial.h>

#define TARGET_ROUND 10

SoftwareSerial HM10(2,3); // RX, TX

const int MPU_ADDR = 0x68;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
double angleAcX, angleAcY, angleAcZ;
int count = 0;
int angle = 0;
double initX = 0;
double angleX = 0;
const double RADIAN_TO_DEGREE = 180 / 3.14159;

void setup() {
  initSensor();
  Serial.begin(9600);
  HM10.begin(9600);
  pinMode(8,OUTPUT);
  delay(200);
  initX = getAngleX();  // 초기 값; 센서를 고정시켰다고 가정할 경우 0도로 설정
}



void loop() {
  angleX = 0;

  for(int i=0;i<3;i++) {  // 가끔 튀는 값이 있어 평균을 사용하였음.

    angleX += getAngleX();  

    delay(20);

  }

  angleX = angleX /3;
  delay(50);
  if (angleX*getAngleX()<0)
  {
    if(angle > 0){
      count++;
      angle = 0;
    }
    else angle++;
    Serial.print(count);
    Serial.print(" + ");
    Serial.println(angle);
    
    HM10.print(count);
    HM10.print(" + ");
    HM10.println(angle);
  }
  
  if (count%TARGET_ROUND == 0 && count != 0) tone(8,450,2000);
}

double getAngleX() {
  getData();

  angleAcY = atan(-AcX / sqrt(pow(AcY, 2) + pow(AcZ, 2)));
  angleAcY *= RADIAN_TO_DEGREE;

  angleAcX = atan(AcY / sqrt(pow(AcX, 2) + pow(AcZ, 2)));
  angleAcX *= RADIAN_TO_DEGREE;

  return angleAcX;

}

void initSensor() {
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
}

void getData() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true);
  
  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  Tmp = Wire.read() << 8 | Wire.read();
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();
} 
