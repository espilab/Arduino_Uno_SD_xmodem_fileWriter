/*
  user_define.h
*/
#include     "Arduino.h"
#include <SD.h>

#define	UINT      unsigned int
#define	USHORT    unsigned short int
#define	SHORT     signed short int
#define	UCHAR     unsigned char
#define	BOOL      unsigned char


#define	TRUE	0x01
#define	FALSE	0x00

#define	CR	((char)0x0d)
#define	LF	((char)0x0a)
#define	BEL	((char)0x07)
#define	BS	((char)0x08)
#define ESC ((char)0x1b)

#define SOH ((char)0x01)
#define ACK ((char)0x06)
#define NAK ((char)0x15)
#define STX ((char)0x02)
#define EOT ((char)0x04)
#define CAN ((char)0x18)



//---------- uart.c functions ----------
int recv_len(void);
int uart_sendstr(char *cp);
int uart_sendchar(UCHAR c);
BOOL uart_getchar(UCHAR *c);
void init_uart(void);


//---------- file_ope.cpp functions ----------
void init_sd(void);
void showFiles(void);
void printDirectory(File, int);
void upload_file(char*);
void delete_file(char*);
void dump_file(char*);
void make_new_file(char*);  //DEBUG

//---------- session.c functions ----------
void init_session(void);
