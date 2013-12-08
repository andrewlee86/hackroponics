#include <OneWire.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// # Connection:
// #       Pin12 (Arduino) -> Pin 1 VCC (URM V3.2)
// #       GND (Arduino)   -> Pin 2 GND (URM V3.2)
// #       Pin 0 (Arduino) -> Pin 9 TXD (URM V3.2)
// #       Pin 1 (Arduino) -> Pin 8 RXD (URM V3.2)
// #

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

int URPower = 12; // Ultrasound power pin
int USValue = 0;
uint8_t DMcmd[4] = { 0x22, 0x00, 0x00, 0x22 }; //distance measure command

OneWire ds(2); //Temperature chip i/o on digital pin 2
LiquidCrystal_I2C lcd(0x20,20,4);  // set the LCD address to 0x20 for a 20 chars and 4 line display

void setup() {
  Serial.begin(9600); // open serial port, set the baud rate to 9600 bps
  sensorSetup();
  lcd.init();
  lcd.backlight();
  printSplash(5000);
  lcd.clear();
}

void sensorSetup() {
  pinMode(URPower, OUTPUT);
  digitalWrite(URPower, HIGH); // Set to High
  delay(200); 
}

void loop() {
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

// FIXME merge this
void sensorLoop() {
  //Sending distance measure command :  0x22, 0x00, 0x00, 0x22 ;
  for(int i=0; i<4; i++) {
    Serial.write(DMcmd[i]);
  }
  
  delay(40); //delay for 75 ms
  unsigned long timer = millis();
  while(millis() - timer < 30) {
    if (Serial.available()>0) {
      int header=Serial.read(); //0x22
      int highbyte=Serial.read();
      int lowbyte=Serial.read();
      int sum=Serial.read();//sum

      if (header == 0x22) {
        if (highbyte==255) {
          USValue=65525;  //if highbyte =255 , the reading is invalid.
        } else {
          USValue = highbyte*255+lowbyte;
        }
        
        Serial.print("Distance=");
        Serial.println(USValue);
      } else {
        while(Serial.available())  byte bufferClear = Serial.read();
        break;
      }
    }
  }
  
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

//returns the temperature from one DS18S20 in DEG Celsius
float getTemp() {
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

