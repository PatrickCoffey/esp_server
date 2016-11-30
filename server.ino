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
// #include <ESP8266mDNS.h>
#include <FS.h>

/* if self_host is true then it will run its own web server
 * if it is false it will join a wifi netowrk using 
 * the supplied credentials
*/
const bool self_host = true;
const char *ssid = "esp8266";
const char *password = "esp8266esp8266";

ESP8266WebServer server ( 80 );


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
  else if (filename.endsWith(".woff")) return "application/font-woff";
  else if (filename.endsWith(".woff2")) return "application/font-woff2";
  else if (filename.endsWith(".pdf")) return "application/x-pdf";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path) {
  Serial.print ("handling files read on: ");
  Serial.println (path);
  //Serial.print ("file exists: ");
  //Serial.println (SPIFFS.exists(path));
  if (path.endsWith("/"))
    path += "index.htm";
  String contentType = getContentType(path);
  
  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "r");
    if (!file) {
      Serial.println("file open failed");
    }
    //Serial.println ("streaming...");
    size_t sent = server.streamFile(file, contentType);
    //DBG_OUTPUT_PORT.printf("File %s exist\n", file.name());
    //Serial.printf ("File %s exist\n", file.name());
    file.close();
    return true;
  }
  return false;
}

void init_wifi( void ) {
  Serial.println ( "WIFI Setup" );
  Serial.println ( "----------" );
  if (self_host) {
    Serial.println("Configuring access point...");
    Serial.print("my ssid: ");
    Serial.println(ssid);
    Serial.print("my passwd: ");
    Serial.println(password);
    WiFi.softAP(ssid, password);
    WiFi.mode(WIFI_AP);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    Serial.println ( "" );
  } else {
    WiFi.begin ( ssid, password );
    WiFi.mode(WIFI_STA);
    Serial.println ( "" );
    // Wait for connection
    while ( WiFi.status() != WL_CONNECTED ) {
      delay ( 500 );
      Serial.print ( "." );
    }
    Serial.print ( "Connected to " );
    Serial.println ( ssid );
    Serial.print ( "Client IP address: " );
    Serial.println ( WiFi.localIP() );
    Serial.println ( "" );
  }
}

void fs_info ( void ) {
  FSInfo fs_info;
  SPIFFS.info(fs_info);
  Serial.println ( "" );
  Serial.println ( "SPIFFS Info" );
  Serial.println ( "-----------" );
  Serial.print ( "total bytes: " );
  Serial.print ( fs_info.totalBytes );
  Serial.println ( " bytes" );
  Serial.print ( "used bytes: " );
  Serial.print ( fs_info.usedBytes );
  Serial.println ( " bytes" );
  Serial.print ( "bytes left: " );
  Serial.print ( fs_info.totalBytes - fs_info.usedBytes );
  Serial.println ( " bytes" );
  Serial.print ( "block size: " );
  Serial.print ( fs_info.blockSize );
  Serial.println ( " bytes" );
  Serial.print ( "page size: " );
  Serial.print ( fs_info.pageSize );
  Serial.println ( " bytes" );
  Serial.print ( "max open files: " );
  Serial.print ( fs_info.maxOpenFiles );
  Serial.println ( " files" );
  Serial.print ( "max path length: " );
  Serial.print ( fs_info.maxPathLength );
  Serial.println ( " chars" );
  Serial.println ( "" );
}

void fs_list ( void ) {
  Serial.println ( "File Listing" );
  Serial.println ( "------------" );
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    Serial.print(dir.fileName());
    Serial.print( " " );
    File f = dir.openFile("r");
    Serial.print(f.size());
    Serial.println ( " bytes" );
  }
}

void setup ( void ) {
  Serial.begin ( 115200 );
  Serial.println ( "" );
  Serial.println ( "" );

  init_wifi();

  //File System Init
  SPIFFS.begin();
  fs_info();
  fs_list();

  //if ( MDNS.begin ( "esp8266" ) ) {
  //  Serial.println ( "MDNS responder started" );
  //}

  server.onNotFound( []() {
    server.sendHeader("Connection", "close");
    server.sendHeader("Access-Control-Allow-Origin", "*");
    Serial.print ( "checking for: " );
    Serial.println ( server.uri() );
    if (!handleFileRead(server.uri())) {
      server.send(404, "text/html", "<h1>Error: 404</h1>\n\n<h2>File Not Found!</h2>");
    } else {
      Serial.print("Page was served!");
    };
    Serial.println ( "" );
  });
  server.begin();
  Serial.println ( "HTTP server started" );
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
}
