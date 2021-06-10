#include<Wire.h>
#define FIL 0.9   //フィルタ
#define AVE 100   //測定回数
#define GYRO 30   //ドリフト許容値

int emp[6] = {};  //6軸のデータを格納するリスト
int data = 0;   //データ格納を行う指標で、1から順にx,y,z,θ,φ,ψを示す
long datasum = 0;   //合計データの格納を担う、平均算出時に使う
int th = 0;     //閾値
void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
