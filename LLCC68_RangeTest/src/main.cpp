#include <Arduino.h>
#include <RadioLib.h>

#define BUSY D1
#define DIO1 D0

#define LORA_RST D2
#define LORA_CS D8

#define FREQUENCY 868.0
#define BANDWIDTH 125.0
#define SPREADING_FACTOR 9
#define CODING_RATE 7
#define OUTPUT_POWER 22
#define PREAMBLE_LEN 8
#define GAIN 0

LLCC68 radio = new Module(LORA_CS, DIO1, LORA_RST, BUSY);


void setup() {
  Serial.begin(115200);
  Serial.print("[LLCC68] Initializing ... ");
  int state = radio.begin(FREQUENCY, BANDWIDTH, SPREADING_FACTOR, CODING_RATE, RADIOLIB_SX126X_SYNC_WORD_PRIVATE, OUTPUT_POWER, PREAMBLE_LEN, GAIN, false);
  if (state == RADIOLIB_ERR_NONE){
    Serial.println(F("success!"));
  }else{
    Serial.print(F("failed, code "));
    Serial.println(state);
    while(true);
  }
}


void loop() {
  Serial.println("Transmitting");
  int state = radio.transmit("Ping!");
  if(state == RADIOLIB_ERR_NONE){
    Serial.println("Success!");
    radio.startReceive();
    delay(1000);
  }else{
    Serial.print("Error, code ");
    Serial.println(String(state));
  }
  String str;
  state = radio.readData(str);
  if(state == RADIOLIB_ERR_NONE){
    // Print out the Data + RSSI
    Serial.println(str);
    Serial.println("RSSI:"+String(radio.getRSSI()));

    if(str=="Pong!"){
      Serial.print("Pong!");
      state = radio.transmit("PingPong complete!");
      if(state == RADIOLIB_ERR_NONE){
        Serial.println("Success!");
        radio.startReceive();
        delay(3000);
      }else{
        Serial.print("Error, code ");
        Serial.println(String(state));
        delay(2000);
      }
    }
  }
}
