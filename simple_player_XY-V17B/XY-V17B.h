/**********************************************
By David Sanz Kirbis

December 2019

Using definitions and some code from Peter Barsznic Sept 2018:
https://github.com/electron1979/Arduino_XY-V17B_mp3_wav_player/blob/master/XY-V17B.ino

http://www.icstation.com/mini-player-module-audio-voice-board-8bit-uart-contorl-support-card-card-p-13279.html
http://attach01.oss-us-west-1.aliyuncs.com/IC/Datasheet/13288.pdf

5.1>. Communication format
  Adopt full duplex serial port communication;
  Baud rate 9600, data bits 8, stop bit 1, check bit N.
  Start code - command type - data length (n) - data 1- data n- and test (SM)
  Command code: fixed to 0xAA.
  Command type: used to distinguish the type of command.
  Data length: the number of bytes of data in an command.
  Data: the relevant data in the command, when the length of data is 1, means
  that there is only CMD and no data bits.
  Test: it is low 8 bits of the sum of all the bytes. that is, When the starting code
  and the data are added, take out low 8 bits.
  Data format: sent data or command, high 8-bit data is in front, low 8-bit is in
  the back.

5.2>. Communication protocol
  The following is a data definition for the return and identification of the chip.
  A. Playing State definition: the system is on the stop state when power on.
  00(stop) 01(play) 02(pause)
  B. Disk character definition: it is stopped after the switch disk.
  USB:00 SD:01 FLASH:02 NO_DEVICE: FF.
  C. Volume: the volume is 31grades, 0-30.The default is 20grade.
  D. Play mode: the default is the single stop when power on.
  Cycle for all songs (00) : play the whole songs in sequence and play it
  after the play.
  Single cycle (01) : play the current song all the time.
  Single stop (02) : Only play current song once and then stop.
  Random play (03) : random play.
  Directory loop (04) : play the songs in the current folder in order, and
  then play them after the play. The directory does not contain subdirectory.
  Directory random (05): random play in the current folder, and the
  directory does not contain subdirectory.
  Directory order play (06) : play the songs in the current folder in order
  and stop after the play. The directory does not contain subdirectory.
  Sequential play (07) : play the whole songs in order and stop after it is
  played.
  E. EQ definition: the default EQ is NORMAL(00).
  NORMAL(00) POP(01) ROCK(02) JAZZ(03) CLASSIC(04)
  F. Composition play definition: combination play is combined by filename. The
  file requirements are stored under the "XY" file. You can change the name of the
  file you want to combine to two bytes, which is generally recommended as a
  number. Such as: 01. Mp3, 02. Mp3.

  *****Currently only written to select from 255 tracks*****
 */ 
 
 // Play selected track examples
  byte play01[] =     {0xAA, 0x07, 0x02, 0x00, 0x01, 0xB4};
  byte play02[] =     {0xAA, 0x07, 0x02, 0x00, 0x02, 0xB5};

  // Control Commands
  byte play[] =     {0xAA, 0x02, 0x00, 0xAC};
  byte pause[] =    {0xAA, 0x03, 0x00, 0xAD};
  byte stop[] =     {0xAA, 0x04, 0x00, 0xAE};
  byte previous[] = {0xAA, 0x05, 0x00, 0xAF};
  byte next[] =     {0xAA, 0x06, 0x00, 0xB0};
  byte volPlus[] =  {0xAA, 0x14, 0x00, 0xBE};
  byte volMinus[] = {0xAA, 0x15, 0x00, 0xBF};
  byte prevFile[] = {0xAA, 0x0E, 0x00, 0xB8};
  byte nextFile[] = {0xAA, 0x0F, 0x00, 0xB9};
  byte stopPlay[] = {0xAA, 0x10, 0x00, 0xBA};

  // Setting Commands
  byte setVolXX[] =             {0xAA, 0x13, 0x01,0x00,0x00};         // {0xAA, 0x13, 0x01, VOL, SM}            VOL:    0x00-0xFF (0-30)
  byte setLoopMode[] =          {0xAA, 0x18, 0x01};                   // {0xAA, 0x18, 0x01, LM, SM}             LM:     0x00-0x07
  byte setCycleTimes[] =        {0xAA, 0x19, 0x02};                   // {0xAA, 0x19, 0x02, H, L, SM}           L&H:    0x00-0xFF
  byte setEQ[] =                {0xAA, 0x1A, 0x01};                   // {0xAA, 0x1A, 0x01, EQ, SM}             EQ:     0x00-0x04
  byte specifySongXX[] =        {0xAA, 0x07, 0x02, 0x00, 0x00, 0x00}; // {0xAA, 0x07, 0x02, H, L, SM}           L&H:    0x00-0xFF
  byte specifyPath[] =          {0xAA, 0x08};                         // {0xAA, 0x08, Length, Drive, Path, SM}  L,D&P:  0x00-0xFF
  byte switchDriveXX[] =        {0xAA, 0x0B, 0x01, 0x00, 0x00};       // {0xAA, 0x0B, 0x01, Drive, SM}          L&H:    0x00-0xFF
  byte specifySongInterplay[] = {0xAA, 0x16, 0x03};                   // {0xAA, 0x16, 0x03, H, L, SM}           L&H:    0x00-0xFF
  byte specifyPathInterplay[] = {0xAA, 0x17};                         // {0xAA, 0x17, Length, Drive, Path, SM}  L,D&P:  0x00-0xFF
  byte selectSong[] =           {0xAA, 0x1F, 0x02};                   // {0xAA, 0x1F, 0x02, H, L, SM}           L&H:    0x00-0xFF (Don't play)

  // Query Commands
  // Command                   // Command Code            // Return
  byte playStatus[] =          {0xAA, 0x01, 0x00, 0xAB};  // AA, 01 01, play status, SM
  byte currentOnlineDrive[] =  {0xAA, 0x09, 0x00, 0xB3};  // AA, 09 01, drive, SM
  byte currentPlayDrive[] =    {0xAA, 0x0A, 0x00, 0xB4};  // AA, 0A 01, drive, SM
  byte NumberOfSongs[] =       {0xAA, 0x0C, 0x00, 0xB6};  // AA, 0C 02, S.N.H S.N.L SM
  byte currentSong[] =         {0xAA, 0x0D, 0x00, 0xB7};  // AA, 0D 02, S.N.H S.N.L SM
  byte folderDirectorySong[] = {0xAA, 0x11, 0x00, 0xBB};  // AA, 11 02, S.N.H S.N.L SM
  byte folderNumberOfSongs[] = {0xAA, 0x12, 0x00, 0xBC};  // AA, 12 02, S.N.H S.N.L SM

// Global vars
uint8_t SD_DRIVE   =     0x01;
uint8_t USB_DRIVE  =     0x00;
uint8_t IS_STOPPED  =    0x00;
uint8_t IS_PLAYING  =    0x01;
uint8_t IS_PAUSED   =    0x02;


#include <SoftwareSerial.h>

SoftwareSerial *SerialMP3 = NULL;

#define DEBUG_SERIAL

uint8_t currentVolume = 0;
uint8_t numOfTracks   = 0;
long playStartMillis  = 0;


void initMP3(int rxPin, int txPin) {
  SerialMP3 = new SoftwareSerial(rxPin, txPin);
  SerialMP3->begin(9600);
}
/***************************************************
 *             GET NUMBER OF TRACKS
 ***************************************************/
uint8_t detectMP3Tracks() {
  uint8_t noTracks[6]   = {0};    // used to build Number of tracks (noOfTracks)
  uint8_t indx          = 0;      // used to read Serial noOfTracks

 // Detect & print number of tracks ///////////////////////////
  SerialMP3->write(NumberOfSongs, sizeof(NumberOfSongs));
  SerialMP3->flush();
  while (!SerialMP3->available())  {
#ifdef DEBUG_SERIAL  
    Serial.print('.');
#endif
    delay(100);
  }
  Serial.println("");
  while (SerialMP3->available()) {
    noTracks[indx] = SerialMP3->read();
    indx++;
    delay(100);
  }
  uint8_t numberOfTracks = ( (256*noTracks[3]) + (noTracks[4]));
#ifdef DEBUG_SERIAL  
  Serial.print("No. of songs: ");
  Serial.println(numberOfTracks);
#endif
return(numberOfTracks);
}
/***************************************************
 *                SET DRIVE ( SD, USB, etc. )
 ***************************************************/
void setMP3Drive(uint8_t driveNum)
{
    switchDriveXX[3] = driveNum;  
    switchDriveXX[4] = ( switchDriveXX[0] + switchDriveXX[1] + switchDriveXX[2] + switchDriveXX[3] );
    SerialMP3->write(switchDriveXX, sizeof(switchDriveXX));
}
/***************************************************
 *                  PLAY TRACK
 *                  ( 0 - 255 )
 ***************************************************/
void playMP3Track(uint8_t trackNumber)
{
    specifySongXX[4] = trackNumber;                                                      
    specifySongXX[5] = uint8_t( specifySongXX[0] + specifySongXX[1] + specifySongXX[2] + specifySongXX[3] + specifySongXX[4] );  
    SerialMP3->write(specifySongXX, sizeof(specifySongXX));
    playStartMillis = millis();
#ifdef DEBUG_SERIAL  
    Serial.print("Playing track ");
    Serial.println(specifySongXX[4]); 
#endif
}

/***************************************************
 *                  GET STATUS
 ***************************************************/
uint8_t getMP3Status()
{
  uint8_t pStatus[5]   = {0};    // used to build Number of tracks (noOfTracks)
  uint8_t indx          = 0;      // used to read Serial noOfTracks

  SerialMP3->write(playStatus, sizeof(playStatus));
  SerialMP3->flush();
  while (!SerialMP3->available())  {
    delay(100);
  }
  while (SerialMP3->available()) {
    pStatus[indx] = SerialMP3->read();
    indx++;
    delay(100);
  }
  return(pStatus[3]);
}
/***************************************************
 *                 SET VOLUME
 ***************************************************/
void setMP3Volume(uint8_t newVol) {
    if (newVol > 30) newVol = 30;
    setVolXX[3] = newVol;                                                      
    setVolXX[4] = ( setVolXX[0] + setVolXX[1] + setVolXX[2] + setVolXX[3] );  
    SerialMP3->write(setVolXX, sizeof(setVolXX));  
    currentVolume = newVol; 
#ifdef DEBUG_SERIAL  
    Serial.print("New volume ");
    Serial.println(setVolXX[3]); 
#endif
}
/***************************************************
 *  get millis passed since last track started playing
 ***************************************************/
long getMP3PlayingMillis() {
     return(millis() - playStartMillis);
}
/***************************************************
 *  prints track's minutes and seconds elapsed playing 
 ***************************************************/
long printMP3PlaySeconds() {
  long totalseconds = getMP3PlayingMillis()/1000;
  long minutes = totalseconds / 60;
  uint8_t seconds = totalseconds % 60;
  
  if (minutes < 10) Serial.print('0');
  Serial.print(minutes);
  Serial.print("m");
  if (seconds < 10) Serial.print('0');
  Serial.print(seconds);
  Serial.println("s");
  return(totalseconds);
}
