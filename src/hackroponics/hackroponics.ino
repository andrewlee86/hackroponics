#include <OneWire.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

int DS18S20_Pin = 2; //DS18S20 Signal pin on digital 2
OneWire ds(DS18S20_Pin); //Temperature chip i/o
LiquidCrystal_I2C lcd(0x20,20,4);  // set the LCD address to 0x20 for a 20 chars and 4 line display

void setup()
{
  Serial.begin(9600); // open serial port, set the baud rate to 9600 bps
  lcd.init();
  lcd.backlight();
  printSplash(5000);
  lcd.clear();
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

void printSplash(int msDelay) {
  uint8_t bell[8]  = {0x4,0xe,0xe,0xe,0x1f,0x0,0x4};
  uint8_t note[8]  = {0x2,0x3,0x2,0xe,0x1e,0xc,0x0};
  uint8_t clock[8] = {0x0,0xe,0x15,0x17,0x11,0xe,0x0};
  uint8_t heart[8] = {0x0,0xa,0x1f,0x1f,0xe,0x4,0x0};
  uint8_t duck[8]  = {0x0,0xc,0x1d,0xf,0xf,0x6,0x0};
  uint8_t check[8] = {0x0,0x1,0x3,0x16,0x1c,0x8,0x0};
  uint8_t cross[8] = {0x0,0x1b,0xe,0x4,0xe,0x1b,0x0};
  uint8_t retarrow[8] = {	0x1,0x1,0x5,0x9,0x1f,0x8,0x4};
  
  lcd.createChar(0, bell);
  lcd.createChar(1, note);
  lcd.createChar(2, clock);
  lcd.createChar(3, heart);
  lcd.createChar(4, duck);
  lcd.createChar(5, check);
  lcd.createChar(6, cross);
  lcd.createChar(7, retarrow);
  lcd.home();
  
  lcd.setCursor(0, 0);
  for(int i = 0;i < 20; i++)  lcd.printByte(6);
  lcd.setCursor(0, 1);
  lcd.printByte(6);
  lcd.print("   Hackroponics   ");
  lcd.printByte(6);
  lcd.setCursor(0, 2);
  lcd.printByte(6);
  lcd.print("       v0.1       ");
  lcd.printByte(6);
  lcd.setCursor(0, 3);
  for(int i = 0;i < 20; i++)  lcd.printByte(6);
  delay(msDelay);
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

