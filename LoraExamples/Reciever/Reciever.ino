#include <Wire.h>
#include <SPI.h>
#include <LoRa.h> 
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSans9pt7b.h>
#include "myFonts.h"

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET    -1  // Reset pin (-1 if using default reset pin)
#define I2C_SDA       21  // Change if you're using different pins
#define I2C_SCL       22  // Change if you're using different pins
// Pin definitions
#define CONFIG_MOSI 27
#define CONFIG_MISO 19
#define CONFIG_CLK  5
#define CONFIG_NSS  18
#define CONFIG_RST  23
#define CONFIG_DIO0 26

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
String LoraMsg="";
int rssi=0;
bool states[4]={0,0,0,0};
int pinsSw[4]={2,15,13,12};

void setup() {
    Serial.begin(115200);
    pinMode(25,OUTPUT);
    pinMode(2,OUTPUT);
    pinMode(15,OUTPUT);
    pinMode(13,OUTPUT);
    pinMode(12,OUTPUT);

    digitalWrite(2,1);
    digitalWrite(13,1);
    digitalWrite(15,1);
    digitalWrite(12,1);

    Wire.begin(I2C_SDA, I2C_SCL);

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;); // Don't proceed, loop forever
    }
    display.display(); // Show initial buffer content
    setBrightness(70);

  
  //START spi
  SPI.begin(CONFIG_CLK, CONFIG_MISO, CONFIG_MOSI, CONFIG_NSS);
  // Initialize LoRa with custom pins
  LoRa.setPins(CONFIG_NSS, CONFIG_RST, CONFIG_DIO0);

  // Start LoRa communication
  if (!LoRa.begin(868E6)) { // Frequency set to 868 MHz
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setSpreadingFactor(8);      // SF7
  LoRa.setSignalBandwidth(125E3); // 125 kHz
  LoRa.setCodingRate4(5);         // 4/5
  LoRa.setSyncWord(120); 
  LoRa.setTxPower(20, PA_OUTPUT_PA_BOOST_PIN);
  Serial.println("LoRa Receiver initialized successfully.");
 
  drawInit();
}

void drawInit()
{
    display.clearDisplay();
    //display.drawRect(0, 0, 128, 64, 1);
    display.setCursor(0, 4);
    display.setTextColor(1);
    
    display.setFont(&DejaVu_Sans_Mono_Bold_12);
    display.printf("RECEIVER");
    display.setFont(NULL);
    
    display.fillRect(68, 0, 60, 2, 1);
    

    for(int i=68;i<128;i++)
    if(i%2==0){
    display.drawPixel(i,4,1);
    display.drawPixel(i,32,1);
    }

    for(int i=4;i<32;i++)
    if(i%2==0){
    display.drawPixel(68,i,1);
    display.drawPixel(127,i,1);
    }

    display.setTextColor(1);
    display.setCursor(72, 8);
    
    display.printf("LAST MSG ");

    display.setTextColor(1);
    display.setCursor(72, 20);
    display.print(LoraMsg);

    
    display.fillRect(0, 14, 62, 18, 1);
    display.setTextColor(0);
    display.setCursor(3, 21);
    display.print("RSSI:");

    display.fillRect(32, 19, 28, 11, 0);
    display.setTextColor(1);
    display.setCursor(34, 21);
    display.print(String(rssi));


    
    for(int i=0;i<4;i++)
    {
    if(states[i]) 
    display.fillRoundRect(i*33, 36, 28, 28, 4, 1);
    else
    display.drawRoundRect(i*33, 36, 28, 28, 4, 1);;

    display.setFont(NULL);
    display.setTextColor(!states[i]);
    display.setCursor(4+(i*33), 40);
    display.print("SW"+String(i));  
    display.setFont(&DejaVu_Sans_Mono_Bold_12);
    display.setCursor(12+(i*33), 59);
    display.print(states[i]);  
    display.drawRoundRect(i*33, 36, 28, 28, 4, 1);
    
    }
    display.setFont(NULL);
    display.display();
}

char c;
void loop()
{
  // Try to parse a received packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
     
     Serial.print("Received packet: ");
     LoraMsg="";
    // Read packet content
    while (LoRa.available()) {
      LoraMsg = LoRa.readString();
      Serial.print(LoraMsg);
    }

    if(LoraMsg.length()==5)
    {
    digitalWrite(25,1);
    c=LoraMsg.charAt(4);
    int n = c - '0'; 
    states[n]=!states[n];
    digitalWrite(pinsSw[n],!states[n]);
    Serial.println();
    Serial.println(LoraMsg.length());
    // Print RSSI (Signal Strength)
    Serial.print("RSSI: ");
    Serial.println(LoRa.packetRssi());
    rssi=LoRa.packetRssi();

    // Print SNR (Signal-to-Noise Ratio)
    Serial.print("SNR: ");
    Serial.println(LoRa.packetSnr());
    //digitalWrite(25,0);
    drawInit();
    }
  }
}

// CNG 1

void setBrightness(uint8_t contrast) {
  display.ssd1306_command(SSD1306_SETCONTRAST);  // Command to set contrast
  display.ssd1306_command(contrast);            // Contrast value (0-255)
}
