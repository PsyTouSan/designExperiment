#include<Wire.h>
#define FIL 0.9   //フィルタ
#define AVE 100   //測定回数
#define GYRO 30   //ドリフト許容値

int Data[6] = {};  //6軸のデータを格納するリスト
int data = 0;   //データ格納を行う指標で、1から順にx,y,z,θ,φ,ψを示す
long datasum = 0;   //合計データの格納を担う、平均算出時に使う
int th = 0;     //閾値
long integral = 0;  //積分した値を格納（加速度を積分していけば距離（回転角）になる）
int degree;
int rock;
char input;
void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(115200);
  Wire.begin(0x68);
  
  Wire.beginTransmission(0x68); //渡したスレーブアドレス（この場合は、0x68)とのやりとりを開始
  Wire.write(0x6B);  Wire.write(0x00);  //受け取った引数をキューへ送信
  Wire.endTransmission(); //送信が完了する
  
  Wire.beginTransmission(0x68);
  Wire.write(0x1C);  Wire.write(0x10);  
  Wire.endTransmission();
  
  Wire.beginTransmission(0x68);  
  Wire.write(0x1B);  Wire.write(0x08);
  Wire.endTransmission();
  
  Wire.beginTransmission(0x68);  
  Wire.write(0x1A);  Wire.write(0x05); 
  Wire.endTransmission();
  Serial.print("--------------------------¥nPreace press 'f' key")  ;

  while(1){
    reading();
    datasum = 0;
    input = Serial.read();
    if(input == 'f'){
      rock = integral / 360;
      Serial.print("Start");
      Serial.print("¥n--------------------------¥n");
      integral = 0;
      delay(250);
      break;
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
void reading(){
  for(int i = 0;i < AVE;i++){
    Wire.beginTransmission(0x68);
    Wire.write(0x3B);   Wire.endTransmission();
    Wire.requestFrom(0x68,14);  //第一引数はデータを要求するデバイスのアドレス、第二引数は、要求するデータバイト量
    while(Wire.available() < 14);
    Data[0] = Wire.read() << 8 | Wire.read(); //Write.readの値を8ビット左シフトし、元のWrite.read()との論理和を計算
    Data[1] = Wire.read() << 8 | Wire.read();
    Data[2] = Wire.read() << 8 | Wire.read();
    Data[3] = Wire.read() << 8 | Wire.read();
    Data[4] = Wire.read() << 8 | Wire.read();
    Data[5] = Wire.read() << 8 | Wire.read();
    datasum += data;
    }
    datasum /= AVE;
    if(th == 0){
      th = datasum;
    }
    if(abs(datasum - th) > GYRO){
      integral += datasum - th;
    }
}
