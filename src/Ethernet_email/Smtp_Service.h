#ifndef __SMTP_SERVICE__H_
#define __SMTP_SERVICE__H_

#include "email.h"


class SmtpService
{
  byte*    _smtp_server;
  unsigned int _port;
  
  void read_response(Client& client)
  {
    delay(5000);
    while(client.available())
    {
      const char c = client.read();
      Serial.print(c);
    }
  }
  
  void send_line(Client& client, String line)
  {
    const unsigned int MAX_LINE = 256;
    char buffer[MAX_LINE];
    line.toCharArray(buffer, MAX_LINE);
    Serial.println(buffer);
    client.println(buffer);
    read_response(client);
  }
  
  public:
  
  SmtpService(
  byte*   smtp_server,
  const unsigned int port) : _smtp_server(smtp_server),
                             _port(port) {}
                             
  void send_email (const Email& email)
  {
    EthernetClient client;
    Serial.print("Connecting...");
    
    if(!client.connect(_smtp_server, _port))
    {
      Serial.println("connection failed.");
    }
    else
    {
      Serial.println("Connected.");
      read_response(client);
      send_line(client, String("ehlo ") + email.getDomain());
      send_line(client, String("AUTH LOGIN"));
      send_line(client, email.getLogin());
      send_line(client, email.getPassword());
      send_line(client, String("mail from: <") + email.getFrom() + String(">"));
      send_line(client, String("rcpt to: <") + email.getTo() + String(">"));
      send_line(client, String("rcpt to: <") + email.getCc() + String(">"));
      send_line(client, String("data"));
      send_line(client, String("from: ") + email.getFrom());
      send_line(client, String("to: ") + email.getTo());
      send_line(client, String("subject: ") + email.getSubject());
      send_line(client, String(""));
      send_line(client, email.getBody());
      send_line(client, String("."));
      send_line(client, String("quit"));
      client.println("Disconnecting.");
      client.stop();
    }
  }
};        
    
#endif
