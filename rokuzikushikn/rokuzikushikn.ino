#include <Wire.h>

#define FIL 0.9         //フィルタ
#define AVE 100         //測定回数
#define GYRO 30         //ドリフト許容値

int16_t emp[6] = {};
int16_t mx,my,mz;
int data = 0;           //生データ格納 1から順に加速度xyz角速度xyz
long datasum = 0;       //合計データ格納　平均算出用
int th = 0;             //閾値
long integral = 0;      //積分値
int degree;             //回転角度
int rock;               //ロック機構により導かれた初期値
char input;

void setup() {
  Wire.begin();  Serial.begin(115200);  Wire.begin(0x68);

  //加速度、角加速度側の設定（スレーブアドレスは0x68）
  Wire.beginTransmission(0x68);  Wire.write(0x6B);  Wire.write(0x00);  Wire.endTransmission();
  Wire.beginTransmission(0x68);  Wire.write(0x1C);  Wire.write(0x10);  Wire.endTransmission();
  Wire.beginTransmission(0x68);  Wire.write(0x1B);  Wire.write(0x08);  Wire.endTransmission();
  Wire.beginTransmission(0x68);  Wire.write(0x1A);  Wire.write(0x05);  Wire.endTransmission();

  //地磁気側の設定（スレーブアドレスは0x0c）
  Wire.beginTransmission(0x68);  Wire.write(0x37);  Wire.write(0x02);  Wire.endTransmission();
  Wire.beginTransmission(0x0C);  Wire.write(0x0A);  Wire.write(0x16);  Wire.endTransmission();


  Serial.print("*****************\nCalibrating now\nplease turn once and press 'f'\n");

  while (1) {
    reading();
    datasum = 0;
    input = Serial.read();
    if (input == 'f') {
      rock = integral / 360;
      Serial.print("Ready!");
      Serial.println("\n*******\n");
      Serial.println(rock);
      Serial.println(integral);
      integral = 0;
      delay(500);
      break;
    }
  }
}
void loop() {
  reading();
  geomagRead();
  degree = abs(integral / rock);
  Serial.print("Gyro(Z):");
  Serial.println(degree);
  Serial.print("geoMag(X):");
  Serial.println(mx);
  Serial.print("geoMag(Y):");
  Serial.println(my);
  Serial.print("geoMag(Z):");
  Serial.println(mz);
  datasum = 0;
}
void reading() {
  //z軸読み取り
  for (int i = 0; i < AVE; i++) {
    Wire.beginTransmission(0x68);  Wire.write(0x3B);  Wire.endTransmission();    Wire.requestFrom(0x68, 14);    
    while (Wire.available() < 14);
    emp[0] = Wire.read() << 8 | Wire.read();
    emp[1] = Wire.read() << 8 | Wire.read();
    emp[2] = Wire.read() << 8 | Wire.read();
    emp[3] = Wire.read() << 8 | Wire.read();
    emp[4] = Wire.read() << 8 | Wire.read();
    emp[5] = Wire.read() << 8 | Wire.read();
    data = Wire.read() << 8 | Wire.read();

    datasum += data;
  }
  datasum /= AVE;
  
  if (th == 0) {
    th = datasum;
  }
  if (abs(datasum - th) > GYRO) { //誤差が小さい場合はスルー ドリフト対策
    integral += datasum - th;
  }
}

void geomagRead(){
  Wire.beginTransmission(0x0C);
  Wire.write(0x02);
  Wire.endTransmission();
  Wire.requestFrom(0x0C, 1);

  uint8_t ST1 = Wire.read();
  if (ST1 & 0x01) {
    Wire.beginTransmission(0x0C);
    Wire.write(0x03);
    Wire.endTransmission();
    Wire.requestFrom(0x0C, 7);
    uint8_t i = 0;
    uint8_t buf[7];
    while (Wire.available()) {
      buf[i++] = Wire.read();
    }
    if (!(buf[6] & 0x08)) {
      mx = ((int16_t)buf[1] << 8) | buf[0];
      my = ((int16_t)buf[3] << 8) | buf[2];
      mz = ((int16_t)buf[5] << 8) | buf[4];
    }
  }
}
