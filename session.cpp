/*
	session.cpp
*/
#include "user_define.h"
#include <string.h>
#include <ctype.h>

/* ---------- Global vars ---------- */
#define CMDLINE_BUFSIZE 16

// var. into sessiom() function
//UCHAR	CMDLINE[CMDLINE_BUFSIZE];



/* hex string to int (up to 4digit)*/
UINT hex2val(UCHAR *str){
  UINT ret,m,p;
  p=0;
  ret=0;
  while ((str[p]!=0)&&(p<5)){ p++; }
  if (p>=5){ return 0; }
  m=1;
  while (p>0){
    p--;
    if (toupper(str[p]) >= 'A'){
      ret += m * ((toupper(str[p])-'A') + 10);
    } else {
      ret += m * (str[p]-'0');
    }
    m = m << 4;
  }
  return ret;
}
/* int to hex string (up to 4digit)*/
void hexstr(UINT val, char *str){
  char *hextbl="0123456789ABCDEF?";
  str[0] = hextbl[val >> 12];
  str[1] = hextbl[(val >> 8) & 0x000f];
  str[2] = hextbl[(val >> 4) & 0x000f];
  str[3] = hextbl[val & 0x000f];
  str[4] = 0;
}
/* ---------- Terminal session functions ---------- */
void show_help()
{
  const char cp[] PROGMEM =
		" l            : list files in SD card\r\n"
		" u <filename> : upload file\r\n"
		" d <filename> : delete file\r\n"
		//" r            : rename file\r\n"
    " 0            : initialize\r\n"
    " ?            : show help\r\n"
    //" x <filename> : hex dump file\r\n"
    //" @            : debug log\r\n"
		"\r\n";
   uart_sendstr(cp);
}

/* --- get one command line --- */
/*   loop in this function until enter key pressed
     return: string length
*/
SHORT cmd_line(UCHAR *cmdlineptr)
{
	UCHAR cin;
	USHORT len;
	BOOL enterkey;
	USHORT cptr = 0;

	enterkey = FALSE;
	while (!enterkey){
		while(!(recv_len() > 0)){		/* wait for anykey loop */
			//check_switch();		/* check push sw event (it's not related this session) */
                      //digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
		}
		//uart_sendstr("(enter-pressed)\r\n");
	
		if (uart_getchar(&cin)){
			if (cin == BS){
				if (cptr >0){
					uart_sendchar(BS);
					uart_sendchar(' ');
					uart_sendchar(BS);
					cptr--;
				} else {
					uart_sendchar(BEL);
				}
			}
			else if (cin == CR){
				uart_sendchar(CR);
				uart_sendchar(LF);
				cmdlineptr[cptr] = 0;
				len = cptr;
				cptr = 0;
				enterkey = TRUE;
			}
			else {
				if (cptr < (CMDLINE_BUFSIZE-1)){
					uart_sendchar(cin);
					cmdlineptr[cptr++] = cin;
				} else {
					uart_sendchar(BEL);
				}
			}
		}
	} /* while(.. */
	return len;
}

/* decode and process command */
void process_command(UCHAR *cmdline, SHORT len)
{
	UINT adr,j;
	char str[16];
  char *cp;
	BOOL quit;
	UCHAR data;
        int stat;



	switch (cmdline[0]){
		case '?':
			show_help();
			break;
		case 'l':
        showFiles();
			  break;

    case 'u':
        cp = cmdline;
        cp += 2;
        j = 0;
        while ((*cp != ' ')&&(*cp != 0)&&(*cp != CR)&&(j < 16)){
          str[j] = *cp;
          cp++;
          j++;
        }
        str[j] = 0;
        upload_file(str);
        break;
    case 'd':
        cp = cmdline;
        cp += 2;
        j = 0;
        while ((*cp != ' ')&&(*cp != 0)&&(*cp != CR)&&(j < 16)){
          str[j] = *cp;
          cp++;
          j++;
        }
        str[j] = 0;
        delete_file(str);
        break;


		case '0':
			init_sd();
			break;

   case 'x':
        cp = cmdline;
        cp += 2;
        j = 0;
        while ((*cp != ' ')&&(*cp != 0)&&(*cp != CR)&&(j < 16)){
          str[j] = *cp;
          cp++;
          j++;
        }
        str[j] = 0;
        //dump_file(str);
        break;
         
    case 'k':

    case '@':   // DEBUG make new file
        cp = cmdline;
        cp += 2;
        j = 0;
        while ((*cp != ' ')&&(*cp != 0)&&(*cp != CR)&&(j < 16)){
          str[j] = *cp;
          cp++;
          j++;
        }
        Serial.print("j="); Serial.println(j,DEC); 
        str[j] = 0;
        make_new_file(str);
        break;


      
		dafault:
			break;
	}
}
/* Session --- loop infinite here */
void session()
{
	SHORT len;
  UCHAR  cmdline_str[CMDLINE_BUFSIZE];
  init_sd();
	
	while (1){
		//digitalWrite(13, LOW);
                uart_sendchar('>');
		len = cmd_line(cmdline_str);
		process_command(cmdline_str, len);
	}

}

void init_session(void)
{
	const char cp[] PROGMEM ="Arduino SD card shield file manager terminal 0.3  2018-4 by espilab\r\n>";
	uart_sendstr(cp);

	session();

}
