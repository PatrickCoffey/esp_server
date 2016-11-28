#include <dummy.h>

/*
 * Webserver test
 * --------------
 * This is an example web server implementation
 * currently it serves files straight from the SPIFFS :)
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>

#define DEBUG_WEBSERVER

const char *ssid = "your_ssid";
const char *password = "your_pwd";

ESP8266WebServer server ( 80 );

void root() {
  String html = "";
}

String getContentType(String filename) {
  if (server.hasArg("download")) return "application/octet-stream";
  else if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".json")) return "application/json";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".pdf")) return "application/x-pdf";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path) {
  Serial.print ("handling files read on: ");
  Serial.println (path);
  Serial.print ("file exists: ");
  Serial.println (SPIFFS.exists(path));
  if (path.endsWith("/"))
    path += "index.htm";
  String contentType = getContentType(path);
  
  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "r");
    if (!file) {
      Serial.println("file open failed");
    }
    Serial.println ("streaming...");
    size_t sent = server.streamFile(file, contentType);
    //DBG_OUTPUT_PORT.printf("File %s exist\n", file.name());
    Serial.printf ("File %s exist\n", file.name());
    file.close();
    return true;
  }
  return false;
}

void setup ( void ) {
  Serial.begin ( 115200 );
  WiFi.begin ( ssid, password );
  Serial.println ( "" );

  // Wait for connection
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  Serial.println ( "" );
  Serial.print ( "Connected to " );
  Serial.println ( ssid );
  Serial.print ( "IP address: " );
  Serial.println ( WiFi.localIP() );

  //File System Init
  SPIFFS.begin();

  Serial.println ("opening directory /");
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    Serial.println(dir.fileName());
    File f = dir.openFile("r");
    Serial.println(f.size());
  }

//  if ( MDNS.begin ( "esp8266" ) ) {
//    Serial.println ( "MDNS responder started" );
//  }

//  server.on ( "/inline", []() {
//    server.send ( 200, "text/plain", "this works as well" );
//  } );

  server.on ( "/", root );
  server.onNotFound( []() {
    server.sendHeader("Connection", "close");
    server.sendHeader("Access-Control-Allow-Origin", "*");
    Serial.print ( "checking for: " );
    Serial.println ( server.uri() );
    if (!handleFileRead(server.uri())) {
      server.send(404, "text/plain", "FileNotFound");
    } else {
      //handleFileRead(server.uri());
    };
  });
//  server.onNotFound ( handleNotFound );
  server.begin();
  Serial.println ( "HTTP server started" );
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
}
