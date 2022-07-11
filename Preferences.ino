#include <Preferences.h>

Preferences preferences;

void PrefBegin(){
  preferences.begin("mobile-home", false);

  Vmin = preferences.getUInt("vmin", 300);
  Vmax = preferences.getUInt("vmax", 60);
  Serial.print("Vmax: ");Serial.println(Vmax);
  Serial.print("Vmin: ");Serial.println(Vmin);
}

void ShutDownHandler(){
   preferences.end();
  Serial.println(F("/!\\ SHUTDOWN /!\\"));
}
