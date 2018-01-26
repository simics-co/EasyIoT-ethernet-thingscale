Easy IoT Development Kit (Ethernet)
====

「Easy IoT Development Kit (Ethernet)」は、[EnOcean Library for Arduino](https://github.com/simics-co/EnOcean)を使って、
お手軽IoTシリーズのセンサー情報をIoTクラウド：[ThingScale](https://thingscale.io/)へグラフ表示することができます。

## Description
本スケッチは、以下のEEP(EnOcean Equipment Profiles)に対応しています。

* F6-02-04
* D5-00-01
* A5-02-05
* A5-02-30
* A5-07-01
* A5-09-04

## Demo

EnOcean Shield (TCM410J)で受信した無線データ(センサー情報)を、[ThingScale](https://thingscale.io/)へグラフ表示します

![TemperatureDemo](images/TemperatureDemo.png "TemperatureDemo")<br>
![OccupancyDemo](images/OccupancyDemo.png "OccupancyDemo")

## Requirement

* お手軽IoT開発キット(Arduino Uno R3 + DolphinV4(GWC) Shield + Ethernet Shield 2)を用意してください
* Arduino IDE 1.7.2以降のバージョンを使用してください
* [EnOcean Library for Arduino](https://github.com/simics-co/EnOcean)をインストールしてください
* [Arduino Client for MQTT](https://github.com/knolleary/pubsubclient)をインストールしてください<br>
  ※インストール手順は、「[EnOcean Library for Arduino](https://github.com/simics-co/EnOcean)」のREADME.md：Install 1～4を参考にしてください

## Usage

1. 「EasyIoT-ethernet-thingscale.ino」を実行します
2. スケッチ内の以下の項目を変更します<br>
　![ChangeItems](images/ChangeItems.png "ChangeItems")
   * ① お手持ちのセンサーのIDに書き換えます
   * ② Ethernet Shield 2のMACアドレスに書き換えます
   * ③ 接続するルーターのIPアドレスが固定の場合は、そのIPアドレスに書き換えます（通常、変更する必要はありません）
   * ④ ThingScaleのUsername(ID)に書き換えます
   * ⑤ ThingScaleのManagement Console -> Admin Profile内にある「Device Token」に書き換えます

※スケッチをマイコンボードに書き込むときは、EnOcean ShieldのSW3を「Prog」側にしてください。通常はSW3を「Normal」側にしてください。

## Install

1. [GitHub repository](https://github.com/simics-co/EasyIoT-ethernet-thingscale)から[EasyIoT-ethernet-thingscale-master.zip](https://github.com/simics-co/EasyIoT-ethernet-thingscale/archive/master.zip)をダウンロードします
2. 圧縮ファイルを展開し、フォルダ名を「EasyIoT-ethernet-thingscale」へ変更します

## Contribution

1. Forkする ( https://github.com/simics-co/EnOcean#fork-destination-box )
2. ローカル上でbranchを作る (git checkout -b my-new-feature)
3. 変更した内容をcommitする (git commit -am 'Add some feature')
4. pushする (git push origin my-new-feature)
5. Pull Requestを作成する

## Licence

   Copyright 2016-2018 LoonaiFactory

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

## Author

[LoonaiFactory](https://github.com/loonaifactory)
