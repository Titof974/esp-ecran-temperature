#include <Adafruit_GFX.h>

#include <Adafruit_ST7735.h>
#define ColorBG ST7735_BLACK
#define ColorText ST7735_WHITE
#define TFT_CS         15
#define TFT_DC         2
#define TFT_RST         5
#define TFT_MOSI 13  // Data out
#define TFT_SCLK 14  // Clock out

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BMP280 bme;
char message[20]; //taille max de 20 caractère pour le message, pour l'exemple


void setup(void) {
  Wire.begin(0,4);      
  Serial.begin(9600);

  bool status;

  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  tft.initR(INITR_MINI160x80);  

  Serial.println("Initialized");

  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(3.89);
  tft.setRotation(3);
}

void loop() {
  //tft.fillScreen(ST77XX_BLACK);
  tft.invertDisplay(false);
  sprintf(message, " %.2fC", bme.readTemperature());
  testdrawtext(message,ST77XX_WHITE);
  Serial.println(message);
  delay(500);
}


void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(10, 5);
  tft.setTextColor(ST77XX_BLACK, color);
  tft.setTextWrap(true);
  tft.print(text);
}
