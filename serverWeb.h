#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include "fileSystem.h"

WiFiServer server(80);
// Set these to your desired credentials.
const char *ssid = "AP ejemplo";
const char *password = "123";


void serverInit(){
    // You can remove the password parameter if you want the AP to be open.
  // WiFi.softAP(ssid, password);
  WiFi.softAP(ssid);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();
}

void send404(WiFiClient client){
    client.print("<h1>ERROR 404 from the ucontroller</h1><br>");
    return;
}

////////////////////// WIFI config /////////////////////////////

void sendConfigurationPage(WiFiClient client) { 
    // Envía el encabezado HTTP y el código HTML de la página 
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println();
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("<head><title>Wi-Fi Configuration</title></head>");
    client.println("<body>");
    client.println("<h1>Configure Wi-Fi</h1>");
    client.println("<form method='get'action='/save'>");
    client.println("<label for='ssid'>SSID:</label>");
    client.println("<input type='text'name='ssid'>");
    client.println("<br>");
    client.println("<label for='password'>Password:</label>");
    client.println("<input type='password'name='password'>");
    client.println("<br>");
    client.println("<input type='submit'value='Save'>");
    client.println("</form>");
    client.println("</body>");
    client.println("</html>");
 } 

bool saveWiFiConfiguration(String ssid, String password) { 
    // Abre el archivo de configuración en la memoria flash
    File file = SPIFFS.open("/wifi.cfg", "w");
    if (!file) { 
        Serial.println("Error al abrir el archivo de configuración del Wi-Fi.");
        return false;
    } // Escribe el SSID y la contraseña en el archivo file.println(ssid);
    file.println(password);
    file.close();
    return true;
 } 

String getValue(String request, String name) { 
    // Obtiene el valor del parámetro 'name' de la solicitud 
    int start = request.indexOf(name + "=");
    if (start == -1) { 
        return "";
    } 
    start += name.length() + 1;
    int end = request.indexOf('&', start);
    if (end == -1) { 
        end = request.length();
    } 
    return request.substring(start, end);
 } 

void sendSuccessPage(WiFiClient client) { // Envía el encabezado HTTP y el código HTML de la página client.println("HTTP/1.1 200 OK");
 client.println("Content-Type: text/html");
 client.println();
 client.println("<!DOCTYPE HTML>");
 client.println("<html>");
 client.println("<head><title>Success</title></head>");
 client.println("<body>");
 client.println("<h1>Success</h1>");
 client.println("<p>Wi-Fi configuration saved successfully!</p>");
 client.println("</body>");
 client.println("</html>");
 } 

void processSaveRequest(WiFiClient client, String request) { 
    // Obtiene el SSID y la contraseña del Wi-Fi del formulario 
    String ssid = getValue(request, "ssid");
    String password = getValue(request, "password");
    // Guarda la configuración del Wi-Fi en la memoria flash del ESP8266
    saveWiFiConfiguration(ssid, password);
    // Envía la página de éxito al cliente
    sendSuccessPage(client);
} 

enum Page {
  INDEX,
  SETUP,
  NETWORK_CONFIG,
  SUCCESS,
  ERROR
};

void sendPage(WiFiClient client, Page page) {
  // Send the appropriate response based on the requested page
  switch (page) {
    case INDEX:
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("");
      client.println("<h1>Welcome to the WiFi Configuration page!</h1>");
      break;
    case SETUP:
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("");
      client.println("<h1>Setup page</h1>");
      break;
    case NETWORK_CONFIG:
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("");
      client.println("<h1>Network Configuration page</h1>");
      break;
    case SUCCESS:
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("");
      client.println("<h1>Success!</h1>");
      break;
    case ERROR:
      client.println("HTTP/1.1 404 Not Found");
      client.println("Content-Type: text/html");
      client.println("");
      client.println("<h1>404 Page Not Found</h1>");
      break;
    }
}


////////////////// process all requests ///////////////////////////
void processHTMLRequest() {
/*
  // Read the request
  String request = client.readStringUntil('\r');
  Serial.println(request);

  // Parse the request
  int pos = request.indexOf(' ');
  String method = request.substring(0, pos);
  String url = request.substring(pos + 1, request.indexOf(' ', pos + 1));

  // Send the appropriate page
  if (url == "/") {
    sendPage(client, INDEX);
  } else if (url == "/setup") {
    sendPage(client, SETUP);
  } else if (url == "/network-config") {
    sendPage(client, NETWORK_CONFIG);
  } else {
    sendPage(client, ERROR);
  }

*/

    WiFiClient client = server.available();   // listen for incoming clients
    if (client) {
        Serial.println("New Client.");           // print a message out the serial port
        String currentLine = "";                // make a String to hold incoming data from the client
        while (client.connected()) {            // loop while the client's connected
        if (client.available()) {             // if there's bytes to read from the client,
            char c = client.read();             // read a byte, then
            Serial.write(c);                    // print it out the serial monitor
            if (c == '\n') {                    // if the byte is a newline character
            // if the current line is blank, you got two newline characters in a row.
            // that's the end of the client HTTP request, so send a response:
            if (currentLine.length() == 0) {
                Serial.println("enviando respuesta...");
                // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                // and a content-type so the client knows what's coming, then a blank line:
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/html");
                client.println();
                client.print("<!DOCTYPE html><html><head><title>ESP32</title></head><body>");
                client.print("<h1>Click <a href=\"/config\">CONFIG</a> to connect to local WiFi.</h1><br>");
                // the content of the HTTP response follows the header:
                client.print("<h1>Click <a href=\"/H\">here</a> to turn OFF the LED.</h1><br>");
                client.print("<h1>Click <a href=\"/L\">here</a> to turn ON the LED.</h1><br>");

                // The HTTP response ends with another blank line:
                client.println();
                // break out of the while loop:
                break;
            } else {
                currentLine = "";
            }
            } else if (c != '\r') {
            currentLine += c;
            }
            // Check to see if the client request was "GET /H" or "GET /L":
            if (currentLine.endsWith("GET /H")) {
            digitalWrite(LED_BUILTIN, HIGH);               // GET /H turns the LED on
            }
            if (currentLine.endsWith("GET /L")) {
            digitalWrite(LED_BUILTIN, LOW);                // GET /L turns the LED off
            }
            if (currentLine.endsWith("GET /config")) {
            sendConfigurationPage(client);
            }
        }
        }
        // close the connection:
        client.stop();
        Serial.println("Client Disconnected.");


        // else if (request.startsWith("/save?")) { 
        //     // Si se solicitó la ruta '/save', procesa la solicitud y
        //     // guarda la configuración del Wi-Fi
        //     processSaveRequest(client, request);
        // }else if (request == "/success") { 
        //     // Si se solicitó la ruta '/success', envía la página de éxito al cliente
        //     sendSuccessPage(client);
        // } else { // Si se solicitó otra página,
        //     // envía un mensaje de error
        //     send404(client);
        // }
    }   
}