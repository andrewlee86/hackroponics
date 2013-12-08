#include <OneWire.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <Ethernet.h>

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
OneWire ds(2); //Temperature chip i/o on digital pin 2
LiquidCrystal_I2C lcd(0x20,20,4);  // set the LCD address to 0x20 for a 20 chars and 4 line display
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,0,100);
EthernetServer server(80);

void setup() {
  Serial.begin(9600); // open serial port, set the baud rate to 9600 bps
  sensorSetup();
  lcdSetup();
  webServerSetup();
}

void sensorSetup() {
  pinMode(URPower, OUTPUT);
  digitalWrite(URPower, HIGH); // Set to High
  delay(200);
}

void lcdSetup() {
  lcd.init();
  lcd.backlight();
  printSplash(5000);
  lcd.clear();
}

void webServerSetup() {
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  EthernetClient client = server.available();
}

void loop() {
  int height = getDistance();
  float temp = getTemp();
  int lux = getLux();
  float ph = getPH();
  
  printToLCD(height, temp, lux, ph);
  
  printToWebServer(height, temp, lux, ph);
}

void printToLCD(int height, float temp, int lux, float ph) {
  lcd.clear();
  printWaterLevel(0, height);
  printTemperature(1, temp);
  printLux(2, lux);
  printPH(3, ph);
  delay(1500);
}

void printToWebServer(int height, float temp, int lux, float ph) {
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    
    while (client.connected()) {      
      
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 1");  // refresh the page automatically every 1 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.print("Water level: ");
            client.print(height);
            client.println("<br />");       
            client.print("Temperature: ");
            client.print(temp);
            client.println("<br />");       
            client.print("Light: ");
            client.print(lux);
            client.println("<br />");       
            client.print("pH: ");
            client.print(ph);
          client.println("<br />");       
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
      
    }
    
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
  else{
    Serial.println("client not avaialble");
  }
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
  lcd.print("       v0.3       ");
  lcd.printByte(6);
  lcd.setCursor(0, 3);
  for(int i = 0;i < 20; i++)  lcd.printByte(6);
  delay(msDelay);
}

void printWaterLevel(int row, int height) {
  lcd.setCursor(0, row);
  lcd.print("Water: ");
  lcd.print(height);
  lcd.print(" cm");
}

void printTemperature(int row, float temp) {
  lcd.setCursor(0, row);
  lcd.print("Temp: ");
  lcd.print(temp, 1);  // show 1 decimal place
  lcd.print(" C");  
}

void printLux(int row, int lux) {
  lcd.setCursor(0, row);
  lcd.print("Light: ");
  lcd.print(lux);
  lcd.print(" lux");
}

void printPH(int row, float ph) {
  lcd.setCursor(0, row);
  lcd.print("PH: ");
  lcd.print(ph, 1);  // show 1 decimal place
}

int getDistance() {
  int USValue = 0;
  uint8_t DMcmd[4] = { 0x22, 0x00, 0x00, 0x22 }; //distance measure command
  
  //Sending distance measure command :  0x22, 0x00, 0x00, 0x22 ;
  for (int i=0; i<4; i++) {
    Serial.write(DMcmd[i]);
  }
  
  delay(40); //delay for 75 ms
  if (Serial.available() > 0) {
    int header=Serial.read(); //0x22
    int highbyte=Serial.read();
    int lowbyte=Serial.read();
    int sum=Serial.read();//sum
    
    if (header == 0x22) {
      if (highbyte==255) {
        USValue = 65525;  //if highbyte == 255 , the reading is invalid.
      } else {
        USValue = highbyte * 255 + lowbyte;
      }
      
      return USValue;
    } else {
      while(Serial.available())  byte bufferClear = Serial.read();
    }
  }
  
  return USValue;
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

int getLux() {
  analogRead(2);   //connect grayscale sensor to Analog 2
}

float getPH() {
  return 0.0;
}

