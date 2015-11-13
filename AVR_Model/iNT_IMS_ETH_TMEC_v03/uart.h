#ifndef UART_H
#define UART_H

#define RXB8 1
#define TXB8 0

#define UPE 2

#define DOR 3

#define FE 4

#define UDRE 5

#define RXC 7

#define FRAMING_ERROR           (1<<FE)
#define PARITY_ERROR            (1<<UPE)
#define DATA_OVERRUN            (1<<DOR)
#define DATA_REGISTER_EMPTY     (1<<UDRE)
#define RX_COMPLETE             (1<<RXC)
                                      
extern bit              flag_oled_ack;
extern char             flag_PackageTerminate3;
extern unsigned char    rxPackage3[];



//------------- USART0 Receiver buffer ----------------------//
#define RX_BUFFER_SIZE0 500
extern char rx_buffer0[];
//#if RX_BUFFER_SIZE0 <= 256
//extern unsigned char rx_wr_index0,rx_rd_index0,rx_counter0;
//#else
extern unsigned int rx_wr_index0,rx_rd_index0,rx_counter0;
//#endif                               
// This flag is set on USART0 Receiver buffer overflow
extern bit rx_buffer_overflow0;              
                  
#define CONF_READ_BUFFER    325
extern unsigned char       configRd[];      // for reading config
extern int                 configLen;                   

extern char                config_start;
extern char                config_ready;    // 1 = failed, 2 = success
extern char                config_end;
extern char                config_response;
           
//------------- USART1 Receiver buffer ----------------------//  
//#define RX_BUFFER_SIZE1 255
//extern char rx_buffer1[];
//#if RX_BUFFER_SIZE1 <= 256
//extern unsigned char rx_wr_index1,rx_rd_index1,rx_counter1;
//#else
//extern unsigned int rx_wr_index1,rx_rd_index1,rx_counter1;
//#endif
//// This flag is set on USART1 Receiver buffer overflow
//extern bit rx_buffer_overflow1; 
//------------- USART2 Receiver buffer ----------------------//  

//------------- USART3 Receiver buffer ----------------------//       
//#define RX_BUFFER_SIZE3 8
//extern char rx_buffer3[];
//#if RX_BUFFER_SIZE3 <= 256
//extern unsigned char rx_wr_index3,rx_rd_index3,rx_counter3;
//#else
//extern unsigned int rx_wr_index3,rx_rd_index3,rx_counter3;
//#endif
// This flag is set on USART3 Receiver buffer overflow
//extern bit rx_buffer_overflow3;

                               
char getchar(void);    
int read_uart0(char *readbuf, int maxread,int interchar); 
int write_uart0(char writebuf[], int len);
                              
void putchar0(char c);
void putchar2(char c);  
void putchar3(char c);

void send_uart(char port,char *buffer);
void init_uart(void);
#endif
