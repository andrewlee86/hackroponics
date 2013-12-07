#include <OneWire.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

int DS18S20_Pin = 2; //DS18S20 Signal pin on digital 2
LiquidCrystal_I2C lcd(0x20,20,4);  // set the LCD address to 0x20 for a 16 chars and 2 line display

//Temperature chip i/o
OneWire ds(DS18S20_Pin);  // on digital pin 2

void setup()
{
  Serial.begin(9600); // open serial port, set the baud rate to 9600 bps
  lcd.init();
  lcd.backlight();
}

void loop()
{
  String height = "-";
  float temp = getTemp();
  int lux = analogRead(2);   //connect grayscale sensor to Analog 2  
  String ph = "-";
  
  //lcd.noBacklight();
  lcd.clear();
  printWaterLevel(0, height);
  printTemperature(1, temp);
  printLux(2, lux);
  printPH(3, ph);  
  delay(2000);
}

void printWaterLevel(int row, String height) {
  lcd.setCursor(0, row);
  lcd.print("Water: ");
  lcd.print(height);
}

void printTemperature(int row, float temp) {
  lcd.setCursor(0, row);
  lcd.print("Temp: ");
  lcd.print(temp, 1);  // show 1 decimal place
  lcd.print("C");  
}

void printLux(int row, int lux) {
  lcd.setCursor(0, row);
  lcd.print("Lux: ");
  lcd.print(lux);
}

void printPH(int row, String ph) {
  lcd.setCursor(0, row);
  lcd.print("PH: ");
  lcd.print(ph);
}

float getTemp(){
  //returns the temperature from one DS18S20 in DEG Celsius

  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end

  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad

  
  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }
  
  ds.reset_search();
  
  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;
  
  return TemperatureSum;
  
}
