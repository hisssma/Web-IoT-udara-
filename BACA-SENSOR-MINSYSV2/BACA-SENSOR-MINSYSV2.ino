#include <Wire.h>
#include <MQUnifiedsensor.h>
#include <DHT.h>

#define placa "Arduino UNO"
#define Voltage_Resolution 5 // Reference voltage: 5V
#define ADC_Bit_Resolution 10 // Resolution for Arduino UNO/MEGA/NANO
#define RatioMQ135CleanAir 3.6 // RS/R0 = 3.6 in clean air for MQ135
#define RatioMQ7CleanAir 27.5 // RS/R0 = 27.5 in clean air for MQ7
#define DHTPIN 2
#define DHTTYPE DHT11

MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, A1, "MQ-135"); // MQ135 sensor on A1
MQUnifiedsensor MQ7(placa, Voltage_Resolution, ADC_Bit_Resolution, A2, "MQ-7"); // MQ7 sensor on A2
DHT dht(DHTPIN, DHTTYPE);

float CO2 = 0;
float CO = 0;

void setup() {
  Serial.begin(9600);
  dht.begin();

  // Initialize MQ135
  MQ135.setRegressionMethod(1);
  MQ135.init();
  Serial.print("Calibrating MQ135...");
  float calcR0 = 0;
  for (int i = 1; i <= 10; i++) {
    MQ135.update();
    calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
    Serial.print(".");
  }
  MQ135.setR0(calcR0 / 10);
  Serial.println(" Done!");
  
  if (calcR0 == 0 || isinf(calcR0)) {
    Serial.println("MQ135 Calibration Error. Check wiring.");
    while (1);
  }

  // Initialize MQ7
  MQ7.setRegressionMethod(1);
  MQ7.init();
  Serial.print("Calibrating MQ7...");
  float calcR0_7 = 0;
  for (int i = 1; i <= 10; i++) {
    MQ7.update();
    calcR0_7 += MQ7.calibrate(RatioMQ7CleanAir);
    Serial.print(".");
  }
  MQ7.setR0(calcR0_7 / 10);
  Serial.println(" Done!");
  
  if (calcR0_7 == 0 || isinf(calcR0_7)) {
    Serial.println("MQ7 Calibration Error. Check wiring.");
    while (1);
  }
}

void loop() {
  // MQ135 CO2 reading
  MQ135.update();
  MQ135.setA(110.47);
  MQ135.setB(-2.862);
  CO2 = MQ135.readSensor() + 400; // CO2 in ppm

  // MQ7 CO reading
  MQ7.update();
  MQ7.setA(99.042);
  MQ7.setB(-1.518);
  CO = MQ7.readSensor(); // CO in ppm
  CO = CO * 1.145; // Convert ppm to µg/m³

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("DHT is Error");
  } 

  sendDataToESP(CO, CO2, temperature);

  // Serial.print("Data CO MQ7: ");
  // Serial.print(CO); 
  // Serial.println(" µg/m³");
  // Serial.println("------------------------------------------");
  // delay(1000);

  // Serial.print("Data CO2 MQ135: ");
  // Serial.print(CO2); 
  // Serial.println(" ppm");
  // Serial.println("------------------------------------------");
  // delay(1000);

  // Serial.print("Temperature: ");
  // Serial.print(temperature);
  // Serial.println(" *C");
  // delay(2000);
}

void sendDataToESP(float mq7, float mq135, float temp) {
  String sendData = String(mq7) + "_" + String(mq135) + "_" + String(temp);
  Serial.println(sendData);
  delay(500);
}