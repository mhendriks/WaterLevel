void FSBegin(){

  if (LittleFS.begin(false)) Serial.println(F("File System Mount succesfull"));
  else Serial.println(F("!!!! File System Mount failed"));  

  Serial.print("->Index file ");Serial.println(LittleFS.exists("/index.html")?"available":"not available"); 
  
}
