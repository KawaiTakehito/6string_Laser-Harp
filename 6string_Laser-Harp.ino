#include <MIDIUSB.h>
//光センサーのピン定義
const int sensor[6] = {A0, A1, A2, A3, A4, A5};

//各センサーの閾値　（デフォルトは200）
int sensor_th[6] = {200,200,200,200,200,200};

//各センサーの状態を格納する配列
bool sensor_state_now[6] = {0, 0, 0, 0, 0, 0};
//各センサーの過去の状態を格納する配列
bool sensor_state_old[6] = {0, 0, 0, 0, 0, 0};

//レーザを遮った数を格納
int trigger_num[6] = {0, 0, 0, 0, 0, 0,};
//trigger_numの上限
const int trigger_max[6] = {12, 12, 12, 12, 12, 12};

//音階の番号を格納 C2~C7までの6オクターブ　×　C~Bまでの12音階の配列
int note_no[6][12] = {
                        {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47},
                        {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59},
                        {60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71},
                        {72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83},
                        {84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95},
                        {96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107}
                        };

//デバッグ用の機能（シリアルモニタを使うもの）のON/OFF切り替え
bool SerialSW = true;
bool SensorMonitor = false;

void setup() {
  Serial.begin(9600);

  //内蔵LEDの出力
  pinMode(LED_BUILTIN, OUTPUT);

  /**
  6つの光センサーの値を20回取得し平均を取る
  それらの値に175をを足したものを各センサーの閾値とする
  **/
  int valueofLight;
  float sum[6] = {0,0,0,0,0,0};
  for(int i=0;i<6;i++){
    for(int j=0;j<20;j++){
      valueofLight = analogRead(sensor[i]);
      if(SerialSW){
        Serial.print("A");
        Serial.print(i);
        Serial.print(" ");
        Serial.print(j);
        Serial.print("回目 ");
        Serial.println(valueofLight);
      }
      sum[i] += (float)valueofLight;
    }
    delay(100);
  }
  for(int i = 0;i<6;i++){
    sum[i] /= 20;
    sum[i] += 175;
    sensor_th[i] = sum[i];
    if(SerialSW){
      Serial.print("sensor_th[");
      Serial.print(i);
      Serial.print("]");
      Serial.print(":");
      Serial.println(round(sum[i]));
    }
  }
}

void loop() {
  //レーザーを遮った(各センサーの値が閾値未満)ならsensor_state_nowを1に、それ以外なら0に
  for(int string=0; string<6; string++){
    if(analogRead(sensor[string]) < sensor_th[string]){
      sensor_state_now[string] = 1;
    }
    else {
      sensor_state_now[string] = 0;
    }
  }

  /***
  sensor_state_nowが1のときmidi信号を出力
  trigger_numを1増やす（音階が1音上がる）
  trigger_maxを超えるとtrigger_numは0に戻る
  ***/
  for(int string=0; string<6; string++){
    if(sensor_state_now[string] != sensor_state_old[string]){
      int num = trigger_num[string];
     
      noteOn(0, note_no[string][num], 100 * sensor_state_now[string]);
      MidiUSB.flush();
      
      if(sensor_state_now[string] == 0){
        trigger_num[string]++;
        if(trigger_num[string] >= trigger_max[string]){
          trigger_num[string] = 0;
        }
        delay(50);
      }
    }
  sensor_state_old[string] = sensor_state_now[string];
  }

  /**
  C0~F0のmidi信号を入力するとchoicestringをそれぞれ0~6に変更する
  C1~B1のmidi信号を入力するとtrigger_num[choicestring]をそれぞれ0~12に変更する
  **/
  int choicestring = 0;
  midiEventPacket_t rx;
  do {
    rx = MidiUSB.read();
    if ((rx.byte3 != 0)) {
      if(rx.byte2 >= 24 && rx.byte2 <= 35){
        trigger_num[choicestring] = rx.byte2 - 24;
      }
      switch(rx.byte2){
        case 12:
          choicestring = 0;
          break;
        case 13:
          choicestring = 1;
          break;
        case 14:
          choicestring = 2;
          break;
        case 15:
          choicestring = 3;
          break;
        case 16:
          choicestring = 4;
          break;
        case 17:
          choicestring = 5;
          break;
      }
    }
  } while (rx.header != 0);

  //センサーが取得した値を表示(デバッグ用)
  if(SensorMonitor){
    for(int i=0;i<6;i++){
      int SensorMonitor = analogRead(sensor[i]);
      Serial.print("A");
      Serial.print(i);
      Serial.print(" ");
      Serial.println(SensorMonitor);
    }
    delay(500);
  }
}

//以下MIDIUSBライブラリ付属の関数
void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}
