
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
//sensor de temperatura
#include <OneWire.h>
#include <DallasTemperature.h>
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
  //
  
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

  
  }
  hzs=hzs+1;
}

