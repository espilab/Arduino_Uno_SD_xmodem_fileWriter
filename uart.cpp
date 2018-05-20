/*
	uart.cpp
*/
#include "user_define.h"
#include <string.h>

/* ---------- Global vars ---------- */
//#define UARTBUFSIZE 10
//UCHAR	RXBUF[UARTBUFSIZE];
//UCHAR	TXBUF[UARTBUFSIZE];
/*
UCHAR	*RX_WPTR, *RX_RPTR;
USHORT	RCV_LEN;
UCHAR	RXBYTE;
*/



/* ---------- UART access functions ---------- */
void test1(void)
{
	/*char a[]="ABC\r\n";
/*	UART6_SendData(&a, 5);	*/

}

int uart_sendstr(char *txstr)
{
  Serial.print(txstr);
}

int uart_sendchar(UCHAR c)
{
  Serial.print((char)c);
}

//int uart_recvSet()	/* setting receive buffer */
//{ }

/* get a received char from receive buffer */
BOOL uart_getchar(UCHAR *c)
{
  BOOL f;
  f = FALSE;
  if (Serial.available() > 0){
    f = TRUE;
    *c = (UCHAR)(Serial.read());
  } 
  return f;
}
int recv_len()
{
	return Serial.available();
}

void init_uart()
{
  Serial.begin(115200);
}


