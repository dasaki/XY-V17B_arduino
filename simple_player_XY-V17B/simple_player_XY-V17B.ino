/*********************************************************************
  David Sanz Kirbis

  December 2019

  Use a voltage level tranlator between 5v Arduino's data pins and
  the module's 3.3v pins, or at least a voltage divider for the 
  modules IO1/RX's pin

  Arduino                                    XY-V17B           SPEAKER

                                               DACR ------------   +
                                               DACL ------------   -
    5V --------------------------------------- VDD
    GND -------------------------------------- GND
RX  2 --- 1k resistor ------------------------ IO0/TX
TX  3 --- 1k resistor ---.--- 2k2 resistor --- GND
                          \------------------- IO1/RX
    3.3v ------------ 10k resistor ----------- CON3/BUSY
    GND  ------------ 10k resistor ----------- CON2
    GND  ------------ 10k resistor ----------- CON1
  
*********************************************************************/

#include "XY-V17B.h"
 
uint8_t numTracks  = 0;      // Number of tracks
uint8_t nextTrack  = 1; 
  
void setup() {
  Serial.begin(115200);
  Serial.println("STARTING...");
  initMP3(2, 3); // Arduino's RX and TX pins
  setMP3Drive(SD_DRIVE);
  setMP3Volume(30); // 0-30
  numTracks = detectMP3Tracks();  
}


void loop() {
  if ( getMP3Status() == IS_STOPPED) {
    playMP3Track(nextTrack);
    nextTrack++;
    if (nextTrack > numTracks) nextTrack = 1;
  }
  printMP3PlaySeconds();
  delay(500);
}
