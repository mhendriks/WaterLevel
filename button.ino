void IRAM_ATTR iButton_pressed() {
  if ( millis() - Tpressed > 200) { //debounce
    pressed++;
    Tpressed = millis();
  }
//  Serial.println("AUX_BUTTON pressed"); // zo min mogelijk tijd tijdens een interupt gebruiken
}
void setupAuxButton() {
  pinMode(AUX_BUTTON, INPUT);
  attachInterrupt(AUX_BUTTON, iButton_pressed, RISING);
  Serial.println(F("AUX_BUTTON setup completed"));
}

void handleButtonPressed(){
  if ( Tpressed && ((millis() - Tpressed) > 1500 ) ) 
  {
    Serial.printf("Button %d times pressed\n", pressed );
    switch(pressed){
    case 1: Serial.println(F("Set low level"));
            Vmin = (meten[0]+meten[1]+meten[2]) / 3;
            if (Vmin > Vmax) Vdelta = Vmin - Vmax;
            preferences.putUInt("vmin", Vmin);
            DrawSets(true);
            break;
    case 2: ;
            break;
    case 3: Serial.println(F("Set high level"));
            Vmax = (meten[0]+meten[1]+meten[2]) / 3;
            if (Vmax < Vmin) Vdelta = Vmin - Vmax;
            DrawSets(false);
            preferences.putUInt("vmax", Vmax);
            break;
     case 6: Serial.println(F("Reboot"));
             ESP.restart();
            break;
    }
    Tpressed = 0;
    pressed = 0;
    PressedClear();
  };
}
