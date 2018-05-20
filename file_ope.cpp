/*
 * file_ope.cpp
 */
#include "user_define.h"

#include <SPI.h>
#include <SD.h>

//File root;

#define XMDM_PKT_LEN  132   // SOH+1+1+128+1 = 132
#define LEN128  128
#define TIMEOUT_LIMIT 30000

//#define DEBUG_LOG_LEN  134
#define DEBUG_LOG_LEN  1

//UCHAR DEBUG_LOG[DEBUG_LOG_LEN];   //DEBUG
//int CNT=0;


void init_sd(){
  //for (CNT=0; CNT<DEBUG_LOG_LEN; CNT++) DEBUG_LOG[CNT]=0;   //DEBUG
  
  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  //CNT=0;
}

void showFiles(){
  File root = SD.open("/");
  if (root){
    printDirectory(root, 1);
    root.close();
  } else {
    Serial.println("SD open failed");
  }
}

void printDirectory(File dir, int numTabs) {
  dir.seek(0);
  //dir.rewindDirectory();
  
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      dir.rewindDirectory();
      Serial.println("ok");
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  } // while(true)
}




/* file upload -- xmodem receive and file out */
void upload_file(char *fname){
  File myFile;
  byte buf[140];
  int j,w;
  UCHAR cin;
  BOOL run,done,retry;
  run = FALSE;
  done = FALSE;
  byte ret, err_cnt=0, errkind=0;
  UCHAR blockno;
  const char mes[] PROGMEM ="please start xmodem sending (waiting 16s)...";
  
  Serial.print("filename:");
  Serial.println(fname);

  myFile = SD.open(fname, FILE_WRITE);
  if (myFile){
    run = TRUE;
    Serial.println(mes);
  }
  delay(16000);
  uart_sendchar(NAK);  // start x-modem receive

  blockno = 1;
  while (run){
    j = 0;
    retry = FALSE;
    while (j < XMDM_PKT_LEN){
      w=0;
      while (!uart_getchar(&cin)){
        w++;
        if (w > TIMEOUT_LIMIT){
          Serial.println(F("---time out, aborted."));
          break; //return;
        }
      }
      buf[j] = cin;
      if (j == 0){
        if (cin != SOH){  retry = TRUE;  errkind=1; }
      }
      if (j == 1){
        if (cin != blockno){  retry = TRUE;  errkind=2; }
      }
      if (j == 2){
        if (cin != (UCHAR)(~blockno)){  retry = TRUE;  errkind=3; }
      }
      if ((j == 0) && (cin == EOT)){    // end of receive
         uart_sendchar(ACK); 
         done = TRUE;
         run = FALSE;
         break;  
      } 
      //DEBUG_LOG[CNT] = cin;  if (CNT<DEBUG_LOG_LEN) CNT++;   //DEBUG
      j++;
    } // while(j<132)
    if (retry){
      uart_sendchar(NAK);
      err_cnt++;
      if (err_cnt > 5){
        Serial.print(F("many error! last error kind="));
        Serial.println(errkind, DEC);
        Serial.print(" block#=0x");
        Serial.println(blockno, HEX);
        Serial.print("~block#=0x");
        Serial.println((UCHAR)(~blockno), HEX);
        break;
      }
    }
    else {        // packet is ok
      ret = myFile.write(&buf[3],LEN128);
      if (ret == LEN128){
        uart_sendchar(ACK);
        blockno++; 
      } else {
        Serial.print(F("file write error!\r\n"));
        Serial.print("ret=");
        Serial.println(ret, DEC);
        break;
      }
    }
     /**/
    
  } // while(run)
  
  if (done) 
    Serial.println(F("EOT received!"));
  Serial.println("done!");
  /* DEBUG
 Serial.print("done="); Serial.println(done, DEC);
  Serial.print(" j="); Serial.println(j, DEC);
  Serial.println(" buf[]="); 
  for (int k=0; k<8; k++){
    Serial.print(k, DEC); Serial.print("->0x");
    Serial.println(buf[k],HEX); 
  } */
  myFile.close();
}

/* hex dump file *  //
void dump_file(char *fname){
  File myFile;
  byte buf[16];
  UINT adr=0,j;
  UCHAR cin;

  
  Serial.print("Dump:");
  Serial.print(fname);
  Serial.print("\r\n");
  if (SD.exists(fname)){
    myFile = SD.open(fname, FILE_READ);
    if (myFile){
      while (myFile.available())  {
        ret = myFile.read(buf,16);
        Serial.print(adr, HEX);
        Serial.print("\t: ");
        for (j=0; j<ret; j++){
           Serial.print(buf[j], HEX); Serial.print(' ');
        }
        Serial.print(" ");
        for (j=0; j<ret; j++){
          cin = ((buf[j] >= ' ')&&(buf[j] <= '~')) ? buf[j] : '.'; 
          Serial.print((char)cin);
        }
        Serial.println(" ");
        adr += 16;
        if (uart_getchar(&cin)){    // cancel by ESC key
          if (cin == ESC){
            Serial.println("Break");
            break;
          }
        }
      }//while
    } else {
      Serial.println("cannot access");
    }
    myFile.close();
    Serial.println("done.");
  }
  else {
    Serial.println("Error file does not exists ");
  }  
}
*/


void delete_file(char *fname){
  File myFile;
  Serial.print(F("Delete filename:"));
  Serial.print(fname);
  Serial.print("\r\n");
  if (SD.exists(fname)){
    SD.remove(fname);
    Serial.println("done.");
  }
  else {
    Serial.println(F("Error file does not exists "));
  }
 
}



/*
void debug_log(){
  int j;
  Serial.println("DEBUG LOG---------");
  for (j=0; j<DEBUG_LOG_LEN; j++){
    Serial.print(j);
    Serial.print("-> 0x");
    Serial.println(DEBUG_LOG[j], HEX);
  }
  Serial.println("DEBUG LOG------END");
}

*/



/* file making test */
void make_new_file(char *fname){
  File myFile;
  byte buf[] = "ok123";
  byte k;
  
  Serial.print("filename:");
  Serial.print(fname);
  Serial.print("\r\n");
  
  myFile = SD.open(fname, FILE_WRITE);
  if (myFile){
    myFile.write(buf,5);
    myFile.close();
    Serial.println("done.");
  } else {
    Serial.println("Error opening ");
    Serial.println(fname);
  }
  
}
/**/

