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
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define SEALEVELPRESSURE_HPA (1013.25)

#define ssid "Livebox-A8B0"
#define password "titof*101094/g33k"


Adafruit_BMP280 bme;
char message[20]; //taille max de 20 caractère pour le message, pour l'exemple

// variables
float temperature = 0;

// http server
ESP8266WebServer server ( 80 );

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

  connectwifi();
  initserver();
}

void initserver() {
  server.on ( "/", handleRoot );
  server.begin();
  Serial.println ( "HTTP server started" );
}

String apitemp() {
  String json = "{ \"temperature\" : ";
  json += temperature;
  json += " }";
  return json;
}

void handleRoot() {
  server.send(200, "application/json", apitemp());
}

void loop() {
  server.handleClient();
  //tft.fillScreen(ST77XX_BLACK);
  tft.invertDisplay(false);
  temperature = bme.readTemperature();
  sprintf(message, " %.2fC", temperature);
  testdrawtext(message,ST77XX_WHITE);
  //Serial.println(message);
  delay(500);
}

void connectwifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
     delay(500);
     Serial.print("*");
  }
  
  Serial.println("");
  Serial.println("WiFi connection Successful");
  Serial.print("The IP Address of ESP8266 Module is: ");
  Serial.println(WiFi.localIP());// Print the IP address
}


void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(10, 5);
  tft.setTextColor(ST77XX_BLACK, color);
  tft.setTextWrap(true);
  tft.print(text);
}
