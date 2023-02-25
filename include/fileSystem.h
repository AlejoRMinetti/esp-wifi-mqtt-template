#include <SPIFFS.h>

bool sendFile(WiFiClient client, String path) { 
    // Abre el archivo en SPIFFS 
    File file = SPIFFS.open(path, "r");
    if (!file) { 
        // Si el archivo no existe, envía un mensaje de error 
        client.println("HTTP/1.1 404 Not Found");
        client.println();
        client.println("File not found");
        return false;
    } // Envía el encabezado HTTP 
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println();
    // Envía el contenido del archivo al cliente 
    while (file.available()) { 
        client.write(file.read());
        // Serial.print(file.read());
    }
    file.close();
    return true;
} 


bool FileSysInit() { // Inicializa SPIFFS
    if (!SPIFFS.begin()) { 
        Serial.println("Error al inicializar SPIFFS.");
        return false;
    }
    return true; 
 } 