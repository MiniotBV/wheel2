int knoppen[8];




INTERVAL knoppenInt(1000, MILLIS);

void knoppenUpdate(){
  if(knoppenInt.loop()){
    for(int i = 0; i < 8; i++){
    
      knoppen[i] = digitalRead(displayUIT);

      // delayMicroseconds(1);
      digitalWrite(displayKLOK, 1);
      // delayMicroseconds(1);
      digitalWrite(displayKLOK, 0);
    }


    Serial.print("knoppen= ");

    for(int i = 0; i < 8; i++){
      Serial.print(knoppen[i]);
      Serial.print(' ');
    }

    Serial.println();





  }
}