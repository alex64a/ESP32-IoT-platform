#include <Wire.h>  //Import the required libraries
#include "DHT.h"
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

#define WIFI_SSID "lesswireless"                                                                                   //Network Name
#define WIFI_PASSWORD "**********"                                                                              //Network Password
#define INFLUXDB_URL "https://eu-central-1-1.aws.cloud2.influxdata.com"                                            //InfluxDB v2 server url, e.g. https://eu-central-1-1.aws.cloud2.influxdata.com (Use: InfluxDB UI -> Load Data -> Client Libraries)
#define INFLUXDB_TOKEN "4U6dWd2BYjiHeYf1WGqQ1vUcYLuJVkSzUQVG9e5JVcbBfrHcpwfhlkevrEAEIMncJfYfdU-NJ0mrupXsL2RYtg=="  //InfluxDB v2 server or cloud API token (Use: InfluxDB UI -> Data -> API Tokens -> <select token>)
#define INFLUXDB_ORG "91b1603cf9d889a5"                                                                            //InfluxDB v2 organization id (Use: InfluxDB UI -> User -> About -> Common Ids )
#define INFLUXDB_BUCKET "ESP32_sensor_data"                                                                        //InfluxDB v2 bucket name (Use: InfluxDB UI ->  Data -> Buckets)
#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"                                                                       //InfluxDB v2 timezone

const int SOUND_SENSOR = 18;  //Sound sensor pin

DHT dht(4, DHT11);  //DHT sensor parameters

int temp = 0;  //Variables to store sensor readings
int humid = 0;
int sound = 0;

//two parameters used to display time and humidity every 30 seconds
unsigned long previousMillis = 0;
const long interval = 30000;  // interval at which to print temperature and humidity (in miliseconds)

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);  //InfluxDB client instance with preconfigured InfluxCloud certificate

Point sensor("ESP32");  //Data point

void setup() {
  Serial.begin(115200);  //Start serial communication

  dht.begin();                   //Connect to the DHT Sensor
  pinMode(SOUND_SENSOR, INPUT);  //Connect to the Sound sensor
  WiFi.mode(WIFI_STA);           //Setup wifi connection
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to wifi");  //Connect to WiFi
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  sensor.addTag("device", DEVICE);  //Add tag(s) - repeat as required
  sensor.addTag("SSID", WIFI_SSID);

  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");  //Accurate time is necessary for certificate validation and writing in batches

  if (client.validateConnection())  //Check server connection
  {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}


void loop()  //Loop function
{
  sound = digitalRead(SOUND_SENSOR);  //Record sound
  temp = dht.readTemperature();       //Record temperature
  humid = dht.readHumidity();         //Record humidity

  unsigned long currentMillis = millis();

  sensor.clearFields();  //Clear fields for reusing the point. Tags will remain untouched

  sensor.addField("temperature", temp);  // Store measured value into point
  sensor.addField("humidity", humid);    // Store measured value into point
  sensor.addField("sound", sound);       // Store measured value into point
  if (wifiMulti.run() != WL_CONNECTED)   //Check WiFi connection and reconnect if needed
    Serial.println("Wifi connection lost");

  if (!client.writePoint(sensor))  //Write data point
  {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }

  Serial.print("Sound: ");  //Display sensor readings from the sound sensor
  Serial.println(sound);

  //As our temperature and humidity readings don't change that often, they will be displayed every 30 seconds
  if (currentMillis - previousMillis >= interval) {
    // save the last time data was displayed
    previousMillis = currentMillis;

    Serial.print("Temp: ");  //Display sensor readings (temperature and humidity) from DHT11
    Serial.println(temp);
    Serial.print("Humidity: ");
    Serial.println(humid);
  }
}
