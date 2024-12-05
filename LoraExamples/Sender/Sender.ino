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
String status="WAITING...";
String loraMsg="";
int rssi=0;
int lastOne=5;

int pinsSw[4]={2,15,13,12};
int xpos[4]={90,110,90,110};
int ypos[4]={36,36,52,52};
int deb=0;

void setup() {
    pinMode(25,OUTPUT);
    pinMode(2,INPUT_PULLUP);
    pinMode(15,INPUT_PULLUP);
    pinMode(13,INPUT_PULLUP);
    pinMode(12,INPUT_PULLUP);
    Serial.begin(115200);
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
    display.printf("SENDER");
    display.setCursor(0, 46);
    display.print(status);
    display.setCursor(0, 60);
    display.print(loraMsg);
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

    for(int i=0;i<4;i++)
    if(i==lastOne)
    display.fillRect(xpos[i], ypos[i], 16, 12, 1);
    else
    display.drawRect(xpos[i], ypos[i], 16, 12, 1);

    for(int i=0;i<86;i++)
    if(i%2==0)
    display.drawPixel(i,62,1);
    else
    display.drawPixel(i,63,1);
    
    display.setTextColor(1);
    display.setCursor(72, 8);
    display.printf("sWORD 120");
    display.setCursor(72, 21);
    display.printf("125kHz");

    display.fillRect(0, 14, 62, 18, 1);
    display.setTextColor(0);
    display.setCursor(3, 21);
    display.print("SF8 pLen8");

    display.setFont(NULL);
    display.display();
}

char c;
void loop()
{
  
  for(int i=0;i<4;i++)
  if(digitalRead(pinsSw[i])==0)
  {
    if(deb==0)
    {
      deb=1;
      digitalWrite(25,1);
      LoRa.beginPacket();
      lastOne=i;
      loraMsg="CNG "+String(lastOne);
      LoRa.print(loraMsg);
      LoRa.endPacket();
      status="SENDING...";
      drawInit();
      Serial.println("Message sent!");
      delay(150);
      digitalWrite(25,0);
      status="WAITING...";
      loraMsg="";
      drawInit();
    }
  }else{deb=0;}
  
}


void setBrightness(uint8_t contrast) {
  display.ssd1306_command(SSD1306_SETCONTRAST);  // Command to set contrast
  display.ssd1306_command(contrast);            // Contrast value (0-255)
}