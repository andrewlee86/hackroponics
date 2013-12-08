
// Jayconsystems.com
// Based on the excellent guide at:
// www.scribd.com/doc/88533821/Arduino-Et-Internet-A-Quick-Start-Guide

#include <SPI.h>
#include <Ethernet.h>
#include "Smtp_Service.h"

const unsigned int SMTP_PORT = 587;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte my_ip[] = {192, 168, 0, 100};
byte gateway[] = {192, 168, 0, 1};
byte smtp_server[] = { 132, 245, 193, 137};


SmtpService smtp_service(smtp_server, SMTP_PORT);

char incString[250];

String domain =   "npathway.com";
String login =    "Z2FicmllbEBucGF0aHdheS5jb20=";
String password = "dnpYYmQ0dm1lQTRNZXk=";


Email email;

void setup()
{
   Ethernet.begin(mac, my_ip, gateway);
   Serial.begin(9600);
   Serial.setTimeout(500000);
   delay(1000);   
   
   email.setDomain(domain);
   email.setLogin(login);
   email.setPassword(password);
   
   email.setFrom("gabriel@npathway.com");
   email.setTo("gab.paquin@outlook.com");
   email.setCc("");
   email.setSubject("Jayconsystems is amazing!");
   email.setBody("It really is =)");

}



void loop()
{

  if (Serial.available() > 0) {
    byte inByte = Serial.read(); 
    if (inByte == 'S')
    {
      smtp_service.send_email(email);  
    }
  
  delay(1000);
  }
  else{
    Serial.println("client disconnected");
  }
}

