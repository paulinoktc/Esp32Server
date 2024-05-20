
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
//sensor de temperatura------------
#include <OneWire.h>
#include <DallasTemperature.h>
//Dysplay--------------------------
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//--------------------------------

// OLED FeatherWing buttons map to different pins depending on board.
// The I2C (Wire) bus may also be different.
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


const int oneWireBus = 4;//pin
OneWire oneWire(oneWireBus);
DallasTemperature sensors (&oneWire);

//Snsor de vibracion
int arrayVibrate[20];
int hzs=0;
int suma=0;
int Pin_Sensor = 5; 


//const char* serverUrl = "http://192.168.0.100/serverx.php";
const char* serverUrl = "http://192.168.150.70/serverx.php";

WebServer server(80);

void setup() {
  Serial.begin(115200);
//  const char* ssid     = "MERCUSYS_5EBE";
//  const char* password = "57931289";

  //const char* ssid     = "CPZ14";
  //const char* password = "12345678";


  const char* ssid     = "Redmi";
  const char* password = "12345678";

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.display();

  display.clearDisplay();
  display.display();

  Serial.println("Desconectamos antes de conectar el WiFi");
  WiFi.disconnect();
  Serial.print("Conectando a  ");
  Serial.println(ssid);
  //Conectamos el esp a la red wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //Intentamos conectarnos a la red
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  //Si logramos conectarnos mostramos la ip a la que nos conectamos
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  //Si entramos a la raiz mostramos las opciones
  server.on("/", []() {
    String content="";
    content += "<html>";
    content += "<div><a href=\"encender\">Encender</a></div>";
    content += "<div><a href=\"apagar\">Apagar</a></div>";
    content += "</html>";
    server.send(200, "text/html", content);
  });

  pinMode(Pin_Sensor , INPUT);
  server.begin();
}

void loop() {
  //lipiar la pantalla
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
 
/*
  if(!digitalRead(BUTTON_A)) display.print("A");
  if(!digitalRead(BUTTON_B)) display.print("B");
  if(!digitalRead(BUTTON_C)) display.print("C");
 */
   display.setTextSize(1);
   display.setCursor(0,0);
     display.print("..M E M O S C..");
  /*sensor*/
  sensors.begin();
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  int vibrate = digitalRead(Pin_Sensor);

  int sensor1=1;
  int sensor2=2;
  

  //int valor_entero = random(0, 1000); // Generar un número aleatorio entre 0 y 999
  //float valor = valor_entero / 100.0; // Dividir el número entero para obtener un número decimal
  Serial.print(temperatureC);
  Serial.println("°C");
  
  Serial.print(vibrate);
 
  arrayVibrate[hzs]=vibrate;


  if(hzs==20){

    hzs=0;
    for (int i = 0; i < 20; i++) {
      suma += arrayVibrate[i];
    }
    Serial.print(suma);
    Serial.println("Hz");
    

    // Construir el cuerpo de la solicitud HTTP
  String data = "sensor=" + String(sensor1) + "&valor=" + String(temperatureC) + "&sensor2=" + String(sensor2)+ "&valor2=" + String(suma);
   display.setCursor(0, 8);
   display.print(suma);
   display.print("Hz");

   display.setCursor(0, 16);
   display.print(temperatureC);
   display.print("°C");

   
  suma=0;

  HTTPClient http;
  http.begin(serverUrl); // Especifica la URL del script PHP en el servidor
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");


  int httpResponseCode = http.POST(data); // Envía los datos al servidor

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
  } else {
    Serial.print("Error en la solicitud HTTP: ");
    Serial.println(httpResponseCode);
  }

  http.end(); // Libera los recursos



  server.handleClient();

  display.display(); // actually display all of the above
  
  delay(100);
  yield();
  
  }
  hzs=hzs+1;
}

