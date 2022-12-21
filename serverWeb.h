#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include "fileSystem.h"

WiFiServer server(80);
// Set these to your desired credentials.
const char *ssid = "AP ESP32";
const char *password = "123456";

void serverInit(){
    // You can remove the password parameter if you want the AP to be open.
  // WiFi.softAP(ssid, password);
  WiFi.softAP(ssid);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();
}

////////////////////// WIFI config /////////////////////////////

bool saveWiFiConfiguration(String ssid, String password) { 
    // Abre el archivo de configuración en la memoria flash
    File file = SPIFFS.open("/wifi.cfg", "w");
    if (!file) { 
        Serial.println("Error al abrir el archivo de configuración del Wi-Fi.");
        return false;
    } // Escribe el SSID y la contraseña en el archivo 
    file.println(ssid);
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
  WIFI_CONFIG,
  SUCCESS,
  ERROR
};

void sendPage(WiFiClient client, Page page) {
  // Send the appropriate response based on the requested page
  switch (page) {
    case INDEX:
      sendFile(client, "./data/index.html");
      break;
    case WIFI_CONFIG:
      sendFile(client, "./data/wificonfig.html");
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

enum Page paginaActual = INDEX;

////////////////// process all requests ///////////////////////////
void processHTMLRequest() {
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
                sendPage(client,paginaActual);
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
    }   
}