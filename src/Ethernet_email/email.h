#ifndef __EMAIL__H_
#define __EMAIL__H_

class Email
{
  String _domain, _login, _password, _from, _to, _cc, _subject, _body;
  
  public:
  
  Email(
  const String& domain = "",
  const String& login = "",
  const String& password = "",
  const String& from = "",
  const String& to = "",
  const String& cc = "",
  const String& subject = "",
  const String& body = ""
  ) : _domain(domain), _login(login), _password(password), _from(from), _to(to), _cc(cc), _subject(subject), _body(body) {}
   
  const String& getDomain()   const { return _domain;     }
  const String& getLogin()    const { return _login;      }
  const String& getPassword() const { return _password;   }
  const String& getFrom()     const { return _from;       }
  const String& getTo()       const { return _to;         }
  const String& getCc()       const { return _cc;         }
  const String& getSubject()  const { return _subject;    }
  const String& getBody()     const { return _body;       }
  
  void setDomain( const String domain)     { _domain = domain;    }
  void setLogin( const String login)       { _login = login;      }
  void setPassword( const String password) { _password= password; }
  void setFrom( const String from)         { _from = from;        }
  void setTo( const String to)             { _to = to;            }
  void setCc( const String cc)             { _cc = cc;            }
  void setSubject( const String subject)   { _subject = subject;  }
  void setBody( const String body)         { _body = body;        }
};

#endif
