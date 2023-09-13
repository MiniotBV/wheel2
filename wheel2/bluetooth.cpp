#include "log.h"
#include "bluetooth.h"
#include "pins.h"

Bluetooth::Bluetooth(Shared& shared, Carriage& carriage, Plateau& plateau) :
  _shared(shared),
  _carriage(carriage),
  _plateau(plateau),
  _interval(200, TM_MILLIS),
  _checkBeforeStartInterval(2000, TM_MILLIS) {
} // Bluetooth()

void Bluetooth::init() {
  LOG_DEBUG("bluetooth.cpp", "[init]");
  Serial2.setRX(BT_RXD_PIN);
  Serial2.setTX(BT_TXD_PIN);
  Serial2.setPollingMode(true);
  Serial2.setFIFOSize(1024);
  Serial2.begin(115200);
  // Serial2.begin(9600);

  write("AT+");
} // init()

void Bluetooth::func() {
  if (millis() < 1000) {
    return;
  }

  if (_initTodo) {
    _initTodo = false;
    init();
    return;
  }

  // if(_checkBeforeStartInterval.once()){
  //   write("AT+");
  // }

  if (_interval.tick()) {
    while (Serial2.available() > 0) {
      char c = Serial2.read();
      _buffer += c;
    }

    if (_buffer != "") {
      LOG_DEBUG("bluetooth.cpp", "[func] BT IN:" + _buffer);
      _buffer = "";
    }

    // while (Serial2.available() > 0) {
    //   char c = Serial2.read();
    //   if (c == '\n' || c == '\r' ) {
    //     // if (c == '\n') {
    //     //   Serial.print("<nl>");
    //     // }
    //     // if (c == '\r') {
    //     //   Serial.print("<cr>");
    //     // }
    //     if (_buffer != "") {
    //       encode();
    //     }

    //     _buffer = "";
    //   } else {
    //     _buffer += c;
    //   }
    // }
  }
} // func()

void Bluetooth::write(String command) {
  Serial2.print(command + "\r\n");

  // LOG_DEBUG("bluetooth.cpp", "[write] BT OUT:" + command);
  if (debug) {
    Serial.println("BT OUT:" + command);
  }
} // write()

void Bluetooth::encode() {
  // LOG_DEBUG("bluetooth.cpp", "[encode] BT IN:" + _buffer);
  if (debug) {
    Serial.println("BT IN:" + _buffer);
  }

  if (_buffer.startsWith("income_opid:")) {
    _buffer.replace("income_opid:", "");
    _buffer.trim();

    if (_buffer.startsWith(BT_BUTTON_IN)) {
      _buffer.remove(0, 1);  // remove 'in' from buffer
      LOG_DEBUG("bluetooth.cpp", "[encode] KNOP_IN:" + _buffer);

      // if (_buffer == BT_PLAY) {
      //   if (_shared.state == S_PAUSE || _shared.state == S_PLAYING) { // maybe remove S_PLAYING?
      //     _carriage.pause();
      //   } else if(_shared.state == S_HOME) {
      //     _plateau.play();
      //   }
      // }

      if (_buffer == BT_PLAY) {
        if (_shared.state == S_PAUSE) { 
          _carriage.pause();
        } else if (_shared.state == S_HOME) {
          _plateau.play();
        }
      } else if (_buffer == BT_PAUSE) {
        if (_shared.state == S_PAUSE || _shared.state == S_PLAYING) { // maybe remove S_PLAYING?
          _carriage.pause();
        }
      } else if (_buffer == BT_NEXT_TRACK) {
        if (_shared.state == S_PLAYING || _shared.state == S_PAUSE || _shared.state == S_GOTO_TRACK) {
          _carriage.gotoNextTrack();
        }
      } else if (_buffer == BT_PREV_TRACK) {
        if (_shared.state == S_PLAYING || _shared.state == S_PAUSE || _shared.state == S_GOTO_TRACK) {
          _carriage.gotoPreviousTrack();
        }
      }
    } else if (_buffer.startsWith(BT_BUTTON_OUT)) {
      _buffer.remove(0, 1);  // remove 'in' from buffer
      LOG_DEBUG("bluetooth.cpp", "[encode] BUTTON_OUT:" + _buffer);
    } else {
      LOG_DEBUG("bluetooth.cpp", "[encode] BUTTON_UNKNOWN:" + _buffer);
    }
  }

  if (_buffer.startsWith("OK+")) {
    if (millis() < 4000) {
      _wirelessVersion = true;  // ff checken of er wel een bluetooth module is aangesloten
    }
  }
} // encode()
