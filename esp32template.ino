// App libraries
#include "perifericos.h"
#include "wifiapp.h"

void setup(){
  // Start the Serial Monitor
  Serial.begin(115200);
  perifericosIniciar();
  wifi_setup();
  finishSetupLEDbuildin();
}

void loop(){
  wifi_refresh();
}