#include "DHT.h"

#include <ESP8266WiFi.h>
#include <InfluxDb.h>

#define D_0 16
#define D_1 5
#define D_2 4
#define D_3 0
#define D_4 2

#define D_5 14
#define D_6 12
#define D_7 13
#define D_8 15

#define DHTPIN   D_4    // what digital pin we're connected to
#define DHTTYPE DHT22  // DHT 22

#define INFLUXDB_HOST "192.168.12.1"
#define WIFI_SSID "OPi"
#define WIFI_PASS "perfokarta"

#define MAX_POST_ERRS 3

DHT dht(DHTPIN,DHTTYPE);
Influxdb influx(INFLUXDB_HOST);

int errCount = 0;

void setup() {
  Serial.begin(9600);
  Serial.println(" ### Hello ###");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  influx.setDb("esp");
  dht.begin();

  Serial.println("Setup done");
}

void loop() {
  
  //READ DHT22
  float humidityData = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float celData = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float fehrData = dht.readTemperature(true);
  // put your main code here, to run repeatedly:
  // Check if any reads failed and exit early (to try again).
  if (isnan(humidityData) || isnan(celData) || isnan(fehrData))
  {
    Serial.println("Failed to read from DHT sensor!");
    delay(100);
    return; //Read again
  }
  else {
    Serial.print("T[" + String(celData)+ "] H[" + String(humidityData)+"] ");
  }

    // Compute heat index in Celsius (isFahreheit = false)
  float hicData = dht.computeHeatIndex(celData, humidityData, false);
  // Compute heat index in Fahrenheit (the default)
  float hifData = dht.computeHeatIndex(fehrData, humidityData);

  InfluxData m1 = measure(celData, humidityData);
  influx.prepare(m1);

  // only with this call all prepared measurements are sent
  if ( true != influx.write() ) {
    ++errCount;
    //return;//pass delay()
  }

  if ( errCount > MAX_POST_ERRS) {
    errCount = 0; //in some cases of reset, variable can save value
    Serial.println("Error with POST Request");
    //reconnect to WiFi AP
    WiFi.reconnect();
  }

  delay(5000);
}

InfluxData measure(float& temperature, float& humidity) {
  InfluxData row("weather");
  row.addTag("device", "esp8266");
  row.addTag("sensor", "dht22");
  row.addTag("location", "office-radov");
  row.addValue("temperature", temperature);
  row.addValue("humidity", humidity);
  return row;
}
