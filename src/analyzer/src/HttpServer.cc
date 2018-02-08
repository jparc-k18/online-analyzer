// -*- C++ -*-

#include <iostream>

#include <TCanvas.h>
#include <TH1.h>
#include <THttpServer.h>
#include <TKey.h>
#include <TMacro.h>
#include <TObject.h>
#include <TSystem.h>

#include "FuncName.hh"
#include "HttpServer.hh"

// ClassImp(HttpServer);

//______________________________________________________________________________
HttpServer::HttpServer( void )
  : //TObject(),
    m_server(0),
    m_port(8080)
{
  //  DEBUG_PRINT;
}

//______________________________________________________________________________
HttpServer::~HttpServer( void )
{
  //  DEBUG_PRINT;
}

//______________________________________________________________________________
void
HttpServer::Open( void )
{
  m_server = new THttpServer(Form("http:%d?loopback?thrds=5", m_port));
  m_server->Restrict("/", "allow=all");
  m_server->SetReadOnly(kTRUE);
  std::cout << "#D HttpServer::Open()" << std::endl
	    << "   Port : " << m_port << std::endl;
}

//______________________________________________________________________________
void
HttpServer::Register( TObject *obj )
{
  if( !obj ||
      TString(obj->ClassName()).Contains("TCanvas") )
    return;

  if( TString(obj->ClassName()).Contains("TMacro") )
    Register((TMacro*)obj);

  m_server->Register( "/", obj );
}

//______________________________________________________________________________
void
HttpServer::Register( TList *list, TList *parent )
{
  if( !list ) return;

  std::cout << "List : " << list->GetName() << std::endl;

  TString parent_dir = ( parent ) ? parent->GetName() : "";

  TIter itr(list);
  TObject *obj = 0;
  while( ( obj=itr.Next() ) ){
    TString class_name(obj->ClassName());
    std::cout << "  Obj : " << obj->GetName() << std::endl;
    if( class_name.Contains("TList") ){
      Register((TList*)obj, list);
    }
    if( class_name.Contains("TH1") ||
	class_name.Contains("TH2") ||
	class_name.Contains("TH3") ){
      m_server->Register("/"+parent_dir+"/"+list->GetName(), obj);
    }
  }
}

//______________________________________________________________________________
void
HttpServer::Register( TMacro *macro )
{
  std::cerr << "#W HttpServer::Register(TMacro) is not supported"
  	    << std::endl;
  // if( !macro ) return;
  // macro->Load();
  // m_server->RegisterCommand( Form("/%s", macro->GetName()),
  // 			     Form("%s()", macro->GetName()) );
  // std::cout << "  Macro : " << macro->GetName() << std::endl;
  // m_server->Register("/", macro);
}

//______________________________________________________________________________
void
HttpServer::Register( TString dir, TString command )
{
  m_server->RegisterCommand( dir, command );
}
