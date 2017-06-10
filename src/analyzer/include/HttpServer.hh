// -*- C++ -*-

#ifndef HTTP_SERVER_HH
#define HTTP_SERVER_HH

class TCanvas;
class THttpServer;
class TMacro;
class TObject;

//______________________________________________________________________________
class HttpServer
{
public:
  static HttpServer& GetInstance( void );
  ~HttpServer( void );

private:
  HttpServer( void );
  HttpServer( const HttpServer& );
  HttpServer& operator =( const HttpServer& );

private:
  THttpServer           *m_server;
  Int_t                  m_port;
  std::vector<TCanvas*>  m_canvas;

public:
  void Open( void );
  void Register( TObject *obj );
  void Register( TList *list, TList *parent=0 );
  void Register( TMacro *macro );
  void Register( TString dir, TString command );
  void SetPort( Int_t port ){ m_port = port; }

};

//______________________________________________________________________________
inline HttpServer&
HttpServer::GetInstance( void )
{
  static HttpServer g_instance;
  return g_instance;
}

#endif
