#include "altimetro.h"
#include "IMU.h"
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <LittleFS.h>
#include <WiFi.h>
#include <ArduinoJson.h>

const char* ssid = "SensorIMU";
const char* password = "123456789";

const IPAddress apIP = IPAddress(192, 168, 4, 1);
const IPAddress mask = IPAddress(255, 255, 255 ,0);

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
               void *arg, uint8_t *data, size_t len);

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  while(!Serial) delay(10);

  //WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, mask);
  WiFi.softAP(ssid, password);

  Serial.println("Servidor AP ativo!");
  Serial.print("SSID: "); Serial.println(ssid);
  Serial.print("IP do servidor: "); Serial.println(WiFi.softAPIP());

  if(!LittleFS.begin(false)){
    Serial.println("Erro ao montar LittleFS");
    return;
  }

  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html");
  });

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.begin();
  Serial.println("Servidor e WebSocket iniciados.");
  
  initMS5611();
  calibMS5611();

  initBNO055();
  configBNO055();
  delay(1000);
}

void loop() {
  ws.cleanupClients();
  
  leituraBNO055();
  leituraMS5611();

  // Monta JSON com roll, pitch, yaw e altitude
  sensors_event_t orientationData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  float roll = orientationData.orientation.x;
  float pitch = orientationData.orientation.y;
  float yaw = orientationData.orientation.z;

  float altitude = altimetro.getAltitude(media);

  StaticJsonDocument<128> doc;
  doc["roll"] = roll;
  doc["pitch"] = pitch;
  doc["yaw"] = yaw;
  doc["altitude"] = altitude;
  String json;
  serializeJson(doc, json);

  ws.textAll(json);

  ws.cleanupClients();

  delay(500);
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
               void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("Cliente WS conectado: %u\n", client->id());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("Cliente WS desconectado: %u\n", client->id());
      break;
    case WS_EVT_DATA:
    {
      AwsFrameInfo *info = (AwsFrameInfo*)arg;
      if (info->opcode == WS_TEXT) {
        
        String msg = String((char*)data).substring(0, len);
        Serial.printf("Recebido texto WS: %s\n", msg.c_str());
        
      } else if (info->opcode == WS_BINARY) {
        Serial.printf("Recebido binario WS len=%d\n", len);
      }
      break;
    }
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}