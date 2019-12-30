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
#include <WebSocketsServer.h>

#define SEALEVELPRESSURE_HPA (1013.25)

#define ssid "lmlmlmlm"
#define password "klklkl"

IPAddress staticIP(192,168,1,40);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

Adafruit_BMP280 bme;
char message[20]; //taille max de 20 caractère pour le message, pour l'exemple

// variables
float temperature = 0;

// http server
ESP8266WebServer server ( 80 );
WebSocketsServer webSocket(81);

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
  startWebSocket();
}

void initserver() {
  server.on ( "/", handleRoot );
  server.on("/graph", handlegraph );
  server.begin();
  Serial.println ( "HTTP server started" );
}

String apitemp() {
  String json = "{ \"temperature\" : ";
  json += temperature;
  json += " }";
  return json;
}

String graphpage() {
  String html = "<head><script src=\"https://cdn.plot.ly/plotly-latest.min.js\"></script></head><body><div id=\"graph\"></div></body><script>";
  html += "trace = {y: [],  x: [],  mode: 'lines',  line: {color: '#80CAF6'}};Plotly.plot('graph', [trace]);var cnt = 0;websocket = new WebSocket(\"ws://\" + window.location.host + \":81\");";
  html += "websocket.onmessage = function(evt) { trace.y.push(parseFloat(evt.data)); trace.x.push(new Date());  if (trace.x.length > 6000) {    trace.x.shift();    trace.y.shift();  }  Plotly.redraw('graph'); };";
  html += "</script>";
  return html;
}

void handlegraph() {
  server.send(200, "text/html", graphpage());
}

void handleRoot() {
  server.send(200, "application/json", apitemp());
}

void loop() {
  webSocket.loop();   
  server.handleClient();
  //tft.fillScreen(ST77XX_BLACK);
  tft.invertDisplay(false);
  temperature = bme.readTemperature();
  sprintf(message, " %.2f", temperature);
  testdrawtext(message,ST77XX_WHITE);
  //Serial.println(message);
  delay(500);
  webSocket.broadcastTXT(message);
}


void startWebSocket() { 
  webSocket.begin();          
  Serial.println("WebSocket server started.");
}

void connectwifi() {
  WiFi.begin(ssid, password);
  WiFi.config(staticIP, gateway, subnet);
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
