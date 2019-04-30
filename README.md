# IIJMachinistClient
ESP32(ESP-WROOM-32) Arduino IDE用  
IIJのMachinistにデータ送信するためのライブラリです。

# version
version 0.1

# ライセンス  
License MIT

# 使い方
* このライブラリを Arduino IDE にインクルードしてください。
* Arduino IDE からスケッチ例を参照してください。

# 注意事項

* このライブラリは *IIJ非公式* です。このライブラリに関する問い合わせはIIJに行わないでください。
* ESP32(ESP-WROOM-32)でのみ動作確認しています。
* ESP8266(ESP-WROOM-02)では動きません（時間があったらあとで対応させます）

# 事前に作業しておくこと

* [Machinist](https://app.machinist.iij.jp/login) にアカウント登録しておきます
* [Getting Started](https://machinist.iij.jp/getting-started/) のページを一読しましょう。
* Machinist にログインし、APIキー を取得しておきます。
* Machinist の UI より、Agentを作成しておきます。

# IIJMachinistClient ライブラリ説明

## コンストラクタ

* IIJMachinistClient(String apiKey)  
  コンストラクタ。
  * 引数:
    * apiKey: Machinistから取得した APIキー をセットします

* IIJMachinistClient(char* apiKey)  
  コンストラクタ。
  * 引数:
    * apiKey: Machinistから取得した APIキー をセットします

## public関数

* void init(void)  
  初期化処理 ntpより時刻を取得して ESP32 の内蔵時計にセットします。  
  WiFi接続後、begin関数内で1回だけ実行してください。

* int post(const String &json)  
  MachinistにJSONをPOSTします。
  * 引数
    * json: POSTするJSON文字列
  * 戻り値
    * POSTした結果のhttpステータスコード

* post(const String &agent, const String &nspace, const String &name, const float value)  
  MachinistにデータをPOSTします。
  * 引数
    * agent: Machinistで登録されている Agent の文字列
    * nspace: Machinistに送る namespace の文字列
    * name: Machinistに送る name の文字列
    * value: Machinistに送る値
  * 戻り値
    * POSTした結果のhttpステータスコード

* int post(const String &agent, const String &nspace, const String &name, const float value, const String &tagName, const String &tagValue, const long timestamp = -1L)  
  MachinistにデータをPOSTします。
  * 引数
    * agent: Machinistで登録されている Agent の文字列
    * nspace: Machinistに送る namespace の文字列
    * name: Machinistに送る name の文字列
    * value: Machinistに送る値
    * tagName: Machinistに送る Tag の名称
    * tagValue: Machinistに送る Tag の値
    * timestamp: Machinistに送る timestamp (unixtime値)
  * 戻り値
    * POSTした結果のhttpステータスコード

* int post(const char *agent, const char *nspace, const char *name, const float value, const char *tagName = "", const char *tagValue = "", const long timestamp = -1L)  
  MachinistにデータをPOSTします。
  * 引数
    * agent: Machinistで登録されている Agent の文字列
    * nspace: Machinistに送る namespace の文字列
    * name: Machinistに送る name の文字列
    * value: Machinistに送る値
    * tagName: Machinistに送る Tag の名称
    * tagValue: Machinistに送る Tag の値
    * timestamp: Machinistに送る timestamp (unixtime値)
  * 戻り値
    * POSTした結果のhttpステータスコード

* void setDebugSerial(Stream &debug)  
  引数にSerialオブジェクトを渡すと、そのSerialにデバッグ用の出力を行うようになります。  

# リンク
* IIJ Machinist  
  https://machinist.iij.jp/

* Machinist Getting Started  
  https://machinist.iij.jp/getting-started/


# history
* ver. 0.1: 初回公開

# メモ
