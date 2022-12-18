#include "serverWeb.h"

void wifi_setup() {
  // FileSysInit();

  // TODO try read credencial for local wifi and start in AT mode
  
  
  Serial.println("Configuring access point...");
  serverInit();

  Serial.println("Server started");
}


void wifi_refresh() {
  processHTMLRequest();
}
