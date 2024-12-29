#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// pin aktuator
const int relayPompa = D7;
const int relayKipas = D8;
const int buzzer = D6;
const int dhtLed = D0;
const int ledDanger = D1;
const int ledWarning = D2;
const int ledSafety = D3;

double batasDht = 28.0;
double batasCO = 2.9;
double batasCO2 = 600;

ESP8266WebServer server(80);

const char* ssid = "outfit kece";
const char* password = "123456789";

bool stateKipas = false;
bool statePompa = false;

void setup () {
  // Serial.begin(115200);
  Serial.begin(9600);

  pinMode(relayPompa, OUTPUT);
  pinMode(relayKipas, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(dhtLed, OUTPUT);
  pinMode(ledDanger, OUTPUT);
  pinMode(ledWarning, OUTPUT);
  pinMode(ledSafety, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(relayPompa, LOW);
  digitalWrite(relayKipas, LOW);
  digitalWrite(buzzer, LOW);
  digitalWrite(dhtLed, LOW);
  digitalWrite(ledDanger, LOW);
  digitalWrite(ledWarning, LOW);
  digitalWrite(ledSafety, LOW);

  WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      digitalWrite(LED_BUILTIN, LOW);
      delay(1000);
      Serial.println("Menghubungkan ke WiFi...");
    }

  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("");
  Serial.print("WiFi Terhubung! ");
  Serial.println(ssid);
  Serial.print("IP Address:");
  Serial.println(WiFi.localIP());

  server.on("/kontrolPompa", HTTP_GET, getRelayPompa);
  server.on("/kontrolFan", HTTP_GET, getRelayKipas);


  server.on("/kontrolPompa", HTTP_POST, setRelayPompa);
  server.on("/kontrolFan", HTTP_POST, setRelayKipas);

  server.on("/sendDataArduino", HTTP_GET, kirimDataWeb);

  server.begin();
  Serial.print("Server is Running....");
}


// untuk memparsing data dari '_'
void split(String data, char delimiter, String parts[], int maxParts) {
  int currentIndex = 0;
  int startIndex = 0;

  for(int i = 0; i < data.length(); i++) {
    if(data[i] == delimiter) {
      if(currentIndex < maxParts) {
        parts[currentIndex] = data.substring(startIndex, i);
        currentIndex++;
        startIndex = i + 1;
      }
    }
  }

  if(currentIndex < maxParts) {
    parts[currentIndex] = data.substring(startIndex);
  }
}

void loop() {
  server.handleClient();
  String dataArduino = " ";

  while(Serial.available() > 0) {
    // Baca data dari arduino
    dataArduino += char(Serial.read());
  }

  Serial.print("Data Kiriman arduino: "); //data sek raw atau masih dalamm format 00_00_000\n
  dataArduino.trim();
  Serial.println(dataArduino);
  delay(500);

  if(dataArduino.length() > 0) {
    const int banyaknyaData = 3;
    String arrayParts[banyaknyaData];

    split(dataArduino, '_', arrayParts, banyaknyaData);

    float nilaiCO = arrayParts[0].toFloat();
    float nilaiCO2 = arrayParts[1].toFloat();
    float nilaiDHT = arrayParts[2].toFloat();

    // // Debug output
    // Serial.println("Nilai CO: " + String(nilaiCO));
    // Serial.println("Nilai CO2: " + String(nilaiCO2));
    // Serial.println("Nilai temperature: " + String(nilaiDHT));

    // KONTROL OTOMATIS (lededSafety Stand By)
    // if(nilaiDHT > batasDht && nilaiCO < batasCO && nilaiCO2 < batasCO2){
    //   digitalWrite(ledSafety, HIGH);
    // } else {
    //   digitalWrite(ledSafety, LOW);
    // }

    // if (nilaiDHT < batasDht) {
    //   digitalWrite(dhtLed, HIGH);
    // } else {
    //   digitalWrite(dhtLed, LOW);
    // }

    // if(nilaiCO > batasCO) {
    //   digitalWrite(relayKipas, HIGH);
    //   digitalWrite(ledDanger, HIGH); 
    // } else {
    //   digitalWrite(relayKipas, LOW);
    //   digitalWrite(ledDanger, LOW);

    // }

    // if(nilaiCO2 > batasCO2) {
    //   digitalWrite(relayPompa, HIGH);
    //   digitalWrite(ledWarning, HIGH);
    // } else {
    //   digitalWrite(relayPompa, LOW);
    //   digitalWrite(ledWarning, LOW);
    // }

    // //buzzer emergency
    // if(nilaiCO > 5.00){
    //   digitalWrite(buzzer, HIGH);
    // } else {
    //   digitalWrite(buzzer, LOW);
    // }
  }
   delay(1000);

}

// void untuk kirim data ke web untuk monitoring
void kirimDataWeb() {
  String datas = Serial.readStringUntil('\n');
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", datas);
}

// Untuk Monitoring Sensor ke Halaman WEB
void getRelayKipas() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", stateKipas ? "NYALA" : "MATI");
}

void getRelayPompa() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", statePompa ? "NYALA" : "MATI");
}

void setRelayKipas() {
  stateKipas = !stateKipas;
  digitalWrite(relayKipas, stateKipas ? HIGH : LOW);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", stateKipas ? "NYALA" : "MATI");
}

void setRelayPompa() {
  statePompa = !statePompa;
  digitalWrite(relayPompa, statePompa ? HIGH : LOW);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", statePompa ? "NYALA" : "MATI");
}
