#include <U8g2lib.h>
#include <TinyGPS.h>

// OLED PIN DEFINITION
#define SDA_PIN 17
#define SCL_PIN 18
#define RESET_PIN 21

#define BTN_PIN 0

// Define Log Buffer for later
#define U8LOG_WIDTH 25
#define U8LOG_HEIGHT 8
uint8_t u8log_buffer[U8LOG_WIDTH*U8LOG_HEIGHT];
U8G2LOG u8g2log;

// Define Display with Pins
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, RESET_PIN, SCL_PIN, SDA_PIN);

// Define GPS Module
TinyGPS gps;

void Oled_println(String);
void Oled_print(String);


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


  Serial2.begin(9600, 134217756U, 2, 3);
  /*delay(1000);

  for(int i=0; i<1000; i++){
    Serial2.println("a");
  }*/

}


void loop(){
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (Serial2.available()){
      char c = Serial2.read();
      Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      Oled_print(String(c));
    }
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
