#include <RadioLib.h>
#include <U8g2lib.h>

// OLED PIN DEFINITION
#define SDA_PIN 17
#define SCL_PIN 18
#define RESET_PIN 21

#define BTN_PIN 0

// LoRa PIN DEFINITON
#define NSS 8
#define DIO1 14
#define RESET 12
#define BUSY 13

// LoRa Settings
#define FREQUENCY 868.0
#define BANDWIDTH 125.0
#define SPREADING_FACTOR 9
#define CODING_RATE 7
#define OUTPUT_POWER 22
#define PREAMBLE_LEN 8
#define GAIN 1.6

// Define SX1262 Pins and create Module
SX1262 radio = new Module(NSS, DIO1, RESET, BUSY);

// Define Display with Pins
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, RESET_PIN, SCL_PIN, SDA_PIN);

// Define LoRaWANNode
LoRaWANNode node(&radio, &EU868);

// Define Log Buffer for later
#define U8LOG_WIDTH 25
#define U8LOG_HEIGHT 8
uint8_t u8log_buffer[U8LOG_WIDTH*U8LOG_HEIGHT];
U8G2LOG u8g2log;


void Oled_println(String);
void Oled_print(String);

void setFlag(void);

bool TTN;

void setup() {
  Serial.begin(9600); //for debugging

  //initialize the Display
  u8g2.setBusClock(100e6);                                      // Set I2C Bus speed
  u8g2.begin();                                                 // Start I2C Communication
  u8g2.setFont(u8g2_font_5x7_tr);                               // Set a small Font

  //initialize scrolling Log
  u8g2log.begin(u8g2, U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer);
  u8g2log.setLineHeightOffset(0);
  u8g2log.setRedrawMode(0);

  pinMode(BTN_PIN, INPUT);

  node.wipe();
  node.restore();
  
  /*if(digitalRead(BTN_PIN)){
    Oled_println("LoRa");
    TTN = false;
  }else{
    Oled_println("TTN");
    TTN = true;
  }*/
  TTN = true;
  
  if(!TTN){
    //initialize the LoRa IC
    Oled_print("[SX1262] Initializing ... ");
    int state = radio.begin(FREQUENCY, BANDWIDTH, SPREADING_FACTOR, CODING_RATE, RADIOLIB_SX126X_SYNC_WORD_PRIVATE, OUTPUT_POWER, PREAMBLE_LEN, GAIN, false);

    if(state == RADIOLIB_ERR_NONE) {
      Oled_println("success!");
    } else {
      Oled_print("failed, code ");
      Oled_println(String(state));
      while(true);
    }

    //Set interrupt for recieved Flag
    radio.setDio1Action(setFlag);

    //Start listening to Messages
    Oled_print("Trying to listen ... ");
    state = radio.startReceive();
    if(state == RADIOLIB_ERR_NONE){
      Oled_println("success!");
    }else{
      Oled_print("Error code ");
      Oled_println(String(state));
    }
  }else{
    Oled_print("[SX1262] Initializing... ");
    int state = radio.begin(868.1, 125.0, 9, 7, RADIOLIB_SX126X_SYNC_WORD_PRIVATE, 22, 8, 1.6, false);
    if(state == RADIOLIB_ERR_NONE) {
      Oled_println("success!");
    } else {
      Oled_print("failed, code ");
      Oled_println(String(state));
      while(true);
    }

    // application identifier - pre-LoRaWAN 1.1.0, this was called appEUI
    // when adding new end device in TTN, you will have to enter this number
    // you can pick any number you want, but it has to be unique
    uint64_t joinEUI = 0x6969696969696969;

    // device identifier - this number can be anything
    // when adding new end device in TTN, you can generate this number,
    // or you can set any value you want, provided it is also unique
    uint64_t devEUI = 0x70B3D57ED0063DDF;

    // select some encryption keys which will be used to secure the communication
    // there are two of them - network key and application key
    // because LoRaWAN uses AES-128, the key MUST be 16 bytes (or characters) long

    // network key is the ASCII string "topSecretKey1234"
    //uint8_t nwkKey[] = { 0x02, 0x44, 0x49, 0x2d, 0x3d, 0xfa, 0xd5, 0x81, 0xeb, 0x97, 0x09, 0x4c, 0x45, 0xd6, 0xa9, 0xef};
    //uint8_t nwkKey[] = { 0xef, 0xa9, 0xd6, 0x45, 0x4c, 0x09, 0x97, 0xeb, 0x81, 0xd5, 0xfa, 0x3d, 0x2d, 0x49, 0x44, 0x02};
    uint8_t nwkKey[] = {0x9E, 0x0A, 0x6B, 0xE6, 0xA8, 0x4C, 0x64, 0xC1, 0x40, 0xAD, 0x30, 0x20, 0x32, 0x62, 0xE7, 0x93};
    

    // application key is the ASCII string "aDifferentKeyABC"
    //uint8_t appKey[] = { 0x4e, 0x40, 0x12, 0x07, 0x09, 0xa4, 0x1a, 0xec, 0xe7, 0xc9, 0x91, 0x4d, 0x39, 0xb4, 0x5a, 0x8c};
    //uint8_t appKey[] = { 0x8c, 0x5a, 0xb4, 0x39, 0x4d, 0x91, 0xc9, 0xe7, 0xec, 0x1a, 0xa4, 0x09, 0x07, 0x12, 0x40, 0x4e};
    uint8_t appKey[] = {0xB3, 0x58, 0x53, 0x7E, 0xA4, 0x68, 0x46, 0xC4, 0x8D, 0x75, 0x68, 0x89, 0x28, 0xE8, 0x8A, 0x78};

    node.setTxPower(22);
    node.setDatarate(0);
    Oled_print("[LoRaWAN] Attempting over-the-air activation... ");
    state = node.beginOTAA(joinEUI, devEUI, nwkKey, appKey);
    if(state == RADIOLIB_ERR_NONE) {
      Oled_println("success!");
    } else {
      Oled_print("failed, code ");
      Oled_println(String(state));
      while(true);
    }
    node.saveSession();

    Oled_print("Testing Session ");
    if(node.isJoined()){
      Oled_println("Joined!");
    }else{
      Oled_println("nNot Joined!");
    }
  }
}

//set recieve Flag to nothing recieved
volatile bool receivedFlag = false;

// disable interrupt when it's not needed
volatile bool enableInterrupt = true;

void setFlag(void) {
  // check if the interrupt is enabled
  if(!enableInterrupt) {
    return;
  }

  // we got a packet, set the flag
  receivedFlag = true;
}

int count = 0;

void loop() {
  if(!TTN){
    // have we recieved a message?
    if(receivedFlag){

      // Reset recieve Flag and disable interrupts
      enableInterrupt = false;
      receivedFlag = false;


      // Read the data
      String data;
      int state = radio.readData(data);
      if(state == RADIOLIB_ERR_NONE){
        Serial.println("success recieving:");
      }else{
        Oled_print("Error code recieving ");
        Oled_println(String(state));
      }

      // Print out the Data + RSSI
      Oled_println(String(String(static_cast<uint32_t>(-1*radio.getRSSI())))+":"+data);

      if(data=="Ping!"){
        int state = radio.transmit("Pong!");
        if(state == RADIOLIB_ERR_NONE){
          Serial.println("Success!");
        }else{
          Oled_print("Error, code ");
          Oled_println(String(state));
          delay(2000);
        }
      }

      // Start recieving again and enable interrupts
      enableInterrupt = true;
      radio.startReceive();
    }
  }else{
    node.setDatarate(0);
    // send uplink to port 10
    Oled_println("[LoRaWAN] Sending uplink packet ... ");
    String strUp = "Hello World!" + String(count++);
    String strDown;
    int n = 0;
    int state = node.sendReceive(strUp, 10, strDown);
    if(state == RADIOLIB_ERR_NONE) {
      Oled_println("received a downlink!");

      // print data of the packet (if there are any)
      Oled_println("[LoRaWAN] Data:\t\t");
      if(strDown.length() > 0) {
        Oled_println(strDown);
      } else {
        Oled_println("<MAC commands only>");
      }

      // print RSSI (Received Signal Strength Indicator)
      Oled_print("[LoRaWAN] RSSI:");
      Oled_print(String(radio.getRSSI()));
      Oled_println(" dBm");

      // print SNR (Signal-to-Noise Ratio)
      Oled_print("[LoRaWAN] SNR:");
      Oled_print(String(radio.getSNR()));
      Oled_println(" dB");

      // print frequency error
      Oled_print("[LoRaWAN] Frequency error:");
      Oled_print(String(radio.getFrequencyError()));
      Oled_println(" Hz");
    
    } else if(state == RADIOLIB_ERR_RX_TIMEOUT) {
      Oled_println("no downlink!");
    
    } else {
      Oled_print("failed, code ");
      Oled_println(String(state));
    }
    node.saveSession();
    // wait before sending another packet
    delay(10000);
  }
}

void Oled_println(String new_msg){
  Serial.println(new_msg);
  u8g2log.print(new_msg+"\n");
}

void Oled_print(String new_msg){
  Serial.print(new_msg);
  u8g2log.print(new_msg);
}
