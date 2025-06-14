#include <RPLidar.h>

//LiDARインスタンスの設定
RPLidar lidar;

//レーザーの下限・上限の距離の閾値
const int Dist_Lower_Threshold = 100;
const int Dist_Upper_Threshold = 1500;

//LiDARの検知角度設定
float Angle_Settings[6] = { 60, 72, 84, 96, 108, 120 };

//LiDARの角度誤差の関域
float Angle_Threshold = 1;

//レーザーの遮蔽検知の配列
int String_State[6] = { 0 };
int String_State_Past[6] = { 0 };
int stableCounter[6] = { 0 };
unsigned long noteDetectedAt[6] = { 0 };

//LEDの設定
const int ledPin = 13;
unsigned long ledOnUntil = 0;
const unsigned long LED_ON_DURATION = 200;

//ノートが続く最小値(sec)
const unsigned long NOTE_HOLD_TIME = 150;

//LiDARの検知回数の閾値　
const int STABLE_COUNT_THRESHOLD = 2;

//スイッチの状態設定
bool SW_State;

//レーザーを遮った回数を格納（最大12）
int trigger_num[6] = { 0 };
const int trigger_max[6] = { 12, 12, 12, 12, 12, 12 };

//音階の番号を格納(16進数)
byte note_no[6][12] = {
  { 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F },
  { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B },
  { 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47 },
  { 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53 },
  { 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F },
  { 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B }
};

void setup() {
  //LEDの出力
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ledPin, OUTPUT);

  pinMode(9, INPUT);    // トグルスイッチのPIN
  pinMode(11, OUTPUT);  // MOSFETのPIN
  
  lidar.begin(Serial1);//LiDARのセットアップ

  SerialUSB.begin(115200);//シリアルモニタの通信速度

  //LiDARに回転開始の信号を送る(startscan関数でもいいかも？)
  Serial1.write(0xA5);
  Serial1.write(0x20);
  Serial1.flush();

  digitalWrite(11, LOW);//MOSFETはデフォルトでoff

  SW_State = digitalRead(9);//トグルスイッチの状態検知
}

void loop() {
  if (IS_OK(lidar.waitPoint())) {
    //もしLiDARが回っているなら下記を実行

    //角度と距離を格納
    float distance = lidar.getCurrentPoint().distance;
    float angle = lidar.getCurrentPoint().angle;

    /***
    指定したLiDARの検知角度(Angle_Settings[6] ± Angle_Threshold)かつ
    レーザーの下限・上限の距離の閾値(Dist_Lower_Threshold以上Dist_Upper_Threshold以内)かつ
    LiDARの検知回数が閾値（STABLE_COUNT_THRESHOLD）以上なら
    String_Stateを1にし、LEDを点灯
    ***/
    for (int i = 0; i < 6; i++) {
      if (angle >= Angle_Settings[i] - Angle_Threshold && angle <= Angle_Settings[i] + Angle_Threshold) {
        if (distance >= Dist_Lower_Threshold && distance <= Dist_Upper_Threshold) {
          stableCounter[i]++;
          noteDetectedAt[i] = millis();
          if (stableCounter[i] >= STABLE_COUNT_THRESHOLD) {
            String_State[i] = 1;
            ledOnUntil = millis() + LED_ON_DURATION;
          }
        } else {
          if (millis() - noteDetectedAt[i] > NOTE_HOLD_TIME) {
            String_State[i] = 0;
          }
          stableCounter[i] = 0;
        }
      }
    }
  }

  // LED点灯制御
  digitalWrite(ledPin, (millis() <= ledOnUntil) ? HIGH : LOW);

  //String_Stateが1のときmidi信号を出力　音階が1音上がる
  for (int i = 0; i < 6; i++) {
    if (String_State[i] != String_State_Past[i] && String_State[i] == 1) {
      String_State_Past[i] = String_State[i];
      SerialUSB.write(0x90);
      SerialUSB.write(note_no[i][trigger_num[i]]);
      SerialUSB.write(0x64);
    } else if (String_State[i] != String_State_Past[i] && String_State[i] == 0) {
      String_State_Past[i] = String_State[i];
      SerialUSB.write(0x80);
      SerialUSB.write(note_no[i][trigger_num[i]]);
      SerialUSB.write(0x64);
      trigger_num[i]++;
      if (trigger_num[i] >= trigger_max[i]) {
        trigger_num[i] = 0;
      }
    }
  }

  // MOSFET制御
  SW_State = digitalRead(9);
  if (SW_State == LOW) {
    digitalWrite(11, HIGH);
  } else {
    digitalWrite(11, LOW);
    for(int i = 0; i < 6;i++){
      trigger_num[i] = 0;
    }
  }
}
