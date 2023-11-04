# ESP32 IOT Project

This is an IOT project developed with an ESP32 microcontroller and sensors.
The objective of this project is to collect sensor data from the microcontroller and to store it to a cloud database (InfluxDB). The collected data is then sent to an online visualisation platform Grafana for better presentation of the collected data. 

## Requirements

 Software: 

 * Arduino IDE: https://www.arduino.cc/en/software
 * InfluxDB: https://www.influxdata.com/
 * Grafana: https://grafana.com/

 Hardware: 

 1. ESP32 microcontroller
 2. DHT11 Temperature and humidity sensor 
 3. Sound sensor 
 4. Other sensors
    

## Hardware wiring 

   DHT11:
   
   * GND -> GND 
   * VCC -> VCC
   * DOUT -> GPIO4
   


 Sound sensor:
  
   * GND -> GND 
   * VCC -> VCC
   * OUT -> GPIO18



## Installation

1. Clone the git repository
2. Upload the code to the microcontroller via Arduino IDE

## Credits

Prof. dr. Ivan Mezei

