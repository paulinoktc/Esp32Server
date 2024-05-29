#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
// Sensor de temperatura
#include <OneWire.h>
#include <DallasTemperature.h>
// Display
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Define los pines de los botones y el bus I2C según el tipo de placa
#if defined(ESP8266)
  #define BUTTON_A  0
  #define BUTTON_B 16
  #define BUTTON_C  2
  #define WIRE Wire
#elif defined(ARDUINO_ADAFRUIT_FEATHER_ESP32C6)
  #define BUTTON_A  7
  #define BUTTON_B  6
  #define BUTTON_C  5
  #define WIRE Wire
#elif defined(ESP32)
  #define BUTTON_A 15
  #define BUTTON_B 32
  #define BUTTON_C 14
  #define WIRE Wire
#elif defined(ARDUINO_STM32_FEATHER)
  #define BUTTON_A PA15
  #define BUTTON_B PC7
  #define BUTTON_C PC5
  #define WIRE Wire
#elif defined(TEENSYDUINO)
  #define BUTTON_A  4
  #define BUTTON_B  3
  #define BUTTON_C  8
  #define WIRE Wire
#elif defined(ARDUINO_FEATHER52832)
  #define BUTTON_A 31
  #define BUTTON_B 30
  #define BUTTON_C 27
  #define WIRE Wire
#elif defined(ARDUINO_ADAFRUIT_FEATHER_RP2040)
  #define BUTTON_A  9
  #define BUTTON_B  8
  #define BUTTON_C  7
  #define WIRE Wire1
#else // 32u4, M0, M4, nrf52840 and 328p
  #define BUTTON_A  9
  #define BUTTON_B  6
  #define BUTTON_C  5
  #define WIRE Wire
#endif

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &WIRE);

const int oneWireBus = 4; // Pin temperatura
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

int arrayVibrate[20];
int hzs = 0;
int suma = 0;
int Pin_Sensor = 5;

const char* serverUrl = "http://192.168.172.70/serverx.php";

WebServer server(80);

void setup() {
  Serial.begin(115200);
  

  const char* ssid = "Redmi";
  const char* password = "12345678";

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  display.clearDisplay();
  display.display();

  Serial.println("Desconectamos antes de conectar el WiFi");
  WiFi.disconnect();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  dysplayInit();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", []() {
    String content = "";
    content += "<html>";
    content += "<div><a href=\"encender\">Encender</a></div>";
    content += "<div><a href=\"apagar\">Apagar</a></div>";
    content += "</html>";
    server.send(200, "text/html", content);
  });

  pinMode(Pin_Sensor, INPUT);
  server.begin();
}

void displayInfo(float temperatureC, int suma) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("..M E M O S C..");
  display.setCursor(0, 8);
  display.print(suma);
  display.print(" Hz");
  display.setCursor(0, 16);
  display.print(temperatureC);
  display.print(" C");

  display.setCursor(0, 32);
  display.print("..M E M O S C..");

  display.display();
}


void dysplayInit() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("..M E M O S C..");
  display.setCursor(0, 8);
  display.print("Conectando....");
  display.display();
}

void loop() {
  
  sensors.begin();
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  int vibrate = digitalRead(Pin_Sensor);

  Serial.print(temperatureC);
  Serial.println("C");
  Serial.print(vibrate);
  Serial.println(" v");

  arrayVibrate[hzs] = vibrate;

  if (hzs >= 20) {
    hzs = 0;
    for (int i = 0; i < 20; i++) {
      suma += arrayVibrate[i];
    }
    suma+10;

    if(suma>=25){
      
    }
    
    Serial.print(suma);
    Serial.println(" Hz");

    String data = "sensor=1&valor=" + String(temperatureC) + "&sensor2=2&valor2=" + String(suma);

    displayInfo(temperatureC, suma);

    suma = 0;

    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int httpResponseCode = http.POST(data);

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
    } else {
      Serial.print("Error en la solicitud HTTP: ");
      Serial.println(httpResponseCode);
    }

    http.end();

    server.handleClient();

    //delay(100);
    yield();
  }

  hzs++;
}



