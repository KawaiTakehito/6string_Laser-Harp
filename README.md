# 6string_Laser-Harp
Arduino Microとレーザーモジュール、フォトトランジスタを用いたレーザーハープのソースコード

## 目次
* [概要](#概要)
* [必要なもの](#必要なもの)
* [インストールとセットアップ](#インストールとセットアップ)
* [使い方](#使い方)
* [ピン配置](#ピン配置)
* [ライブラリ](#ライブラリ)
* [参考文献](#参考文献)

## 概要
Arduino MicroのA0～A5ピンをフォトトランジスタに接続し、レーザー光をフォトトランジスタに当てます
レーザー光を遮るとMIDI信号が入力されます
6つのセンサーにはそれぞれ、C2\~B2　C3\~B3　C4\~B4　C5\~B5　C6\~B6　C7\~B7の音が割り当てられており、一回遮るごとにC→C#→D・・・と音階が上がります
* [レーザーハープ全面](images/Laser-Harp2.jpg)
* [レーザーハープ後面](images/Laser-Harp1.jpg)
* [基盤表](images/board1.jpg)
* [基盤裏](images/board2.jpg)

## 必要なもの

### ハードウェア

* Arduino micro
* フォトトランジスタ 560nm NJL7502L
* ブレッドボードまたはユニバーサル基盤
* ジャンパーワイヤー等の配線材
* レーザーハープ筐体

### ソフトウェア

* Arduino IDE 2.3.6
* MIDIUSBライブラリ　1.0.5
* MIDIデバイスを使用できるDAW
* Tx16Wx等のサンプラープラグイン

## インストールとセットアップ

1.  **このリポジトリをクローンまたはダウンロードします**
    ```bash
    git clone [https://github.com/horsebone-entangle/6string_Laser-Harp.git](https://github.com/horsebone-entangle/6string_Laser-Harp.git)
    cd 6string_Laser-Harp-main
    ```

2.  **Arduino IDEを開きます**

3.  **スケッチを開きます**
    `ファイル > 開く` から、ダウンロードしたプロジェクトフォルダ内の `6string_Laser-Harp.ino` ファイルを開きます

4.  **必要なライブラリをインストールします**
    Arduino IDEの `スケッチ > ライブラリをインクルード > ライブラリを管理...` から、MIDIUSBライブラリを検索してインストールします

5.  **ボードとポートを選択します**
    * `ツール > ボード` から、使用しているArduinoボードを選択します
    * `ツール > シリアルポート` から、Arduinoが接続されているポートを選択します

6.  **Arduinoにスケッチをアップロードします**
    Arduino IDEの「アップロード」ボタン（右矢印のアイコン）をクリックします

## 使い方

1.  Arduino microをPCに接続します
2.  お使いのDAWにArduino microをmidiデバイスとして登録します
3.  Tx16Wx等のサンプラープラグインをトラックに挿入し、サンプラーに音源を割り当てます

## ピン配置

以下の表は、Arduino microの使用ピンと部品の一覧です

| Arduinoピン |   　　部 品　　 |  　　備　考   |
| :---------- | :------------- | :----------- |
| A0~A5       | 560nm NJL7502L |200KΩ抵抗を使用|  


## ライブラリ
* [`MIDIUSB`](https://docs.arduino.cc/libraries/midiusb/) - [ArduinoボードをMIDIデバイス化するライブラリ]

## 参考文献
* [Binary Macaron氏](https://www.youtube.com/watch?v=AOoOoMTHRkc&list=PLkjJlIGA7FxJA72lzpV9EdGVzYsoOpRiu)