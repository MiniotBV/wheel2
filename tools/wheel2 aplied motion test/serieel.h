


bool golven = false;



Interval serieelInt(10000, MICROS);
// Interval serieelInt(5000, MICROS);

void serieelFunc(){
  if(serieelInt.loop()){
    if(golven){
      
      
      Serial.print(pos,3);
      Serial.print(", ");

      Serial.print(faseVerschuiving,3);
      Serial.print(", ");

      Serial.print(snelheid,3);
      Serial.print(", ");

      Serial.print(sin(pos),3);
      // Serial.print(", ");


      Serial.println();




    }




    while(Serial.available() > 0){
      char letter = Serial.read();
      
      //------------------------------------------------------------------------SERIEEL PLATEAU MOTOR
      if(letter == 'G'){    //gol aan
        golven = true;
      }
      else if(letter == 'g'){    //golven uit
        golven = false;
      }
  
      else if(letter == 'f'){    //set karP
        faseVerschuiving = Serial.parseFloat();
        Serial.println("fase: " + String(faseVerschuiving));
      }
      else if(letter == 'S'){    //set P
        snelheid = Serial.parseFloat();
        Serial.println("snelheid: " + String(snelheid));
      }
      else if(letter == 's'){    //set P
        targetSnelheid = Serial.parseFloat();
        Serial.println("target: " + String(targetSnelheid));
      }
      else if(letter == 'a'){    //set P
        accel = Serial.parseFloat();
        Serial.println("accel: " + String(accel));
      }
      else if(letter == 'k'){    //set P
        kk = Serial.parseFloat();
        Serial.println("kk: " + String(kk));
      }
      else if(letter == '?'){    //help
        Serial.println("help--------------");   
        Serial.println("snelheid: " + String(snelheid));
        Serial.println("fase: " + String(faseVerschuiving));
        
        Serial.println();
      }

    
    }
  }
}



