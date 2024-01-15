#include <RadioLib.h>

#define BUSY D1
#define DIO1 D0

#define LORA_RST D2
#define LORA_CS D8



static uint8_t ucBackBuffer[1024];
LLCC68 radio = new Module(LORA_CS, DIO1, LORA_RST, BUSY);


LoRaWANNode node(&radio, &EU868);


void setup() {
  Serial.begin(115200);
  int state = radio.begin(868.1, 125.0, 9, 7, RADIOLIB_SX126X_SYNC_WORD_PRIVATE, 17, 8, 0, false);
  if(state == RADIOLIB_ERR_NONE) {
    Serial.println("success!");
  } else {
    Serial.print("failed, code ");
    Serial.println(String(state));
    while(true);
  }

  uint64_t joinEUI = 0x0000000000000000;
  uint64_t devEUI = 0x70B3D57ED0063A79;
  uint8_t nwkKey[] = { 0x02, 0x44, 0x49, 0x2d, 0x3d, 0xfa, 0xd5, 0x81, 0xeb, 0x97, 0x09, 0x4c, 0x45, 0xd6, 0xa9, 0xef};
  uint8_t appKey[] = { 0x4e, 0x40, 0x12, 0x07, 0x09, 0xa4, 0x1a, 0xec, 0xe7, 0xc9, 0x91, 0x4d, 0x39, 0xb4, 0x5a, 0x8c};

  Serial.print("[LoRaWAN] Attempting over-the-air activation ... ");
  state = node.beginOTAA(joinEUI, devEUI, nwkKey, appKey, 9);
  if(state == RADIOLIB_ERR_NONE) {
    Serial.println("success!");
  } else {
    Serial.print("failed, code ");
    Serial.println(String(state));
    while(true);
  }
}

int count = 0;

void loop() {
  // send uplink to port 10
  Serial.println("[LoRaWAN] Sending uplink packet ... ");
  String strUp = "Hello World! #" + String(count++);
  String strDown;
  int state = node.sendReceive(strUp, 10, strDown);
  if(state == RADIOLIB_ERR_NONE) {
    Serial.println("received a downlink!");

    // print data of the packet (if there are any)
    Serial.println("[LoRaWAN] Data:\t\t");
    if(strDown.length() > 0) {
      Serial.print(strDown);
    } else {
      Serial.println("<MAC commands only>");
    }

    // print RSSI (Received Signal Strength Indicator)
    Serial.print("[LoRaWAN] RSSI:");
    Serial.print(String(radio.getRSSI()));
    Serial.println(" dBm");

    // print SNR (Signal-to-Noise Ratio)
    Serial.print("[LoRaWAN] SNR:");
    Serial.print(String(radio.getSNR()));
    Serial.println(" dB");

    // print frequency error
    Serial.print("[LoRaWAN] Frequency error:");
    Serial.print(String(radio.getFrequencyError()));
    Serial.println(" Hz");
  
  } else if(state == RADIOLIB_ERR_RX_TIMEOUT) {
    Serial.println("no downlink!");
  
  } else {
    Serial.print("failed, code ");
    Serial.println(String(state));
  }

  // wait before sending another packet
  delay(30000);
}