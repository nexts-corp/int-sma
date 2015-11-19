#include <mega2560.h>  
#include <stdio.h>           
#include <string.h>
#include <ctype.h>         
#include <delay.h>
#include "uart.h"                
#include "io.h"
#include "ethcomm.h"
                 
/*============================================================*/
char flag_PackageTerminate3 = 0;

flash unsigned char charHeader = '$';
flash unsigned char charTerminate = '#';

bit                 flag_oled_ack      = 0;

unsigned char       rxPackageIndex3;
unsigned char       rxPackage3[64];
unsigned char       rxPackageIndexCount3;
/*============================================================*/
// USART0 Receiver buffer
char rx_buffer0[RX_BUFFER_SIZE0];

unsigned int rx_wr_index0,rx_rd_index0,rx_counter0;

// This flag is set on USART0 Receiver buffer overflow
bit rx_buffer_overflow0;
                                                     
unsigned char       configRd[CONF_READ_BUFFER];      // for reading config
int                 configLen=0;

char                config_start    = 0;
char                config_ready    = 0;    // 1 = failed, 2 = success
char                config_end      = 0;
char                config_response = 0;

// USART0 Receiver interrupt service routine
interrupt [USART0_RXC] void usart0_rx_isr(void)
{
    char status,data;
    status=UCSR0A;
    data=UDR0;        
    
    if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
    {                                       
        if(ETH_current_mode != MODE_CONF)
        {              
            //putchar3(data); 
            rx_buffer0[rx_wr_index0++]  =   data;                        
            
            if (rx_wr_index0 == RX_BUFFER_SIZE0)
            {           
                rx_wr_index0=0;          
            }
            if (++rx_counter0 == RX_BUFFER_SIZE0)
            {
                rx_counter0=0;
                rx_buffer_overflow0=1;  
                send_uart(3,"\r\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n" );
                send_uart(3,    "!!!!!!! Eth Buffer Overflow !!!!!!!!!"     );
                send_uart(3,"\r\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n" );
            }    
        }
        else{     
            putchar3(data);          
            
            if (configLen == CONF_READ_BUFFER)
            {           
                configLen=0;          
            }        
            
            if((data == '<') && (config_start == 0)){
                config_start            = 1;   
                config_response         = 0;      
                config_ready            = 0;
            }   
            else if((data == 0x0D) && (config_end == 0) && (config_start == 1) && (config_response != 0)){           
                config_end  = 0x0D;
            }
            else if((data == 0x0A) && (config_end == 0x0D) && (config_start == 1) && (config_response != 0)){   
                config_end              = 0x0A;
                config_ready            = 1;
            }
            else if((data == 'E') && (config_start == 1) && (config_response == 0)){   
                config_response         = 'E';    
            }
            else if((data == 'S') && (config_start == 1) && (config_response == 0)){  
                config_response         = 'S';       
            }
            else if((config_start == 1) && (config_response != 0)){   
                configRd[configLen++]   =   data;  
            }     
        }
    }
}

#ifndef _DEBUG_TERMINAL_IO_
// Get a character from the USART0 Receiver buffer
#define _ALTERNATE_GETCHAR_
                
#pragma used+
char getchar(void)
{
    char data;
    while (rx_counter0==0);
    data=rx_buffer0[rx_rd_index0++];
 //   #if RX_BUFFER_SIZE0 != 256
    if (rx_rd_index0 == RX_BUFFER_SIZE0)
        rx_rd_index0=0;
//    #endif
    #asm("cli")
    --rx_counter0;
    #asm("sei")
    return data;
}
#pragma used-    
// Write a character to the USART0 Transmitter
#pragma used+
void putchar0(char c)
{
    while ((UCSR0A & DATA_REGISTER_EMPTY)==0);
    UDR0=c;
}
#pragma used-
#endif
   
int read_uart0(char *readbuf, int maxread,int interchar)
{                          
    char data;  
    int len=0;                        
    
    while (rx_counter0 > 0)
    {   
        data=rx_buffer0[rx_rd_index0++];
        if (rx_rd_index0 == RX_BUFFER_SIZE0){
            rx_rd_index0=0;                              
        }
        UCSR0B &= 0x7F;          // -- disable uart0 interrupt -- //
        
        --rx_counter0;                 
        
        UCSR0B |= 0x80;          // -- enable uart0 interrupt -- //
        
        memcpy(readbuf++,&data,1);       
        len++;

        if(len == maxread){
            break;
        }        
        delay_ms(interchar);
    }
    return len;
}     

int write_uart0(char writebuf[], int len)
{
    int i;
    for(i=0;i<len;i++){
        putchar0(writebuf[i]);           
    }           
    return i;
}                 
/*============================================================*/
// USART1 Receiver interrupt service routine
interrupt [USART1_RXC] void usart1_rx_isr(void)
{
    char status,data;
    status=UCSR1A;
    data=UDR1;
    
    if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
    {
        if(data);
    }                        
}    

/*============================================================*/
// USART2 Receiver interrupt service routine
interrupt [USART2_RXC] void usart2_rx_isr(void)
{
    char status,data;
    status=UCSR2A;
    data=UDR2;   
    if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
    {
        if(data=='K'){
            flag_oled_ack = 1;   
        }                       
        else{
            //putchar3(data);
        }
    }      
}

// Write a character to the USART2 Transmitter
#pragma used+
void putchar2(char c)
{
    while ((UCSR2A & DATA_REGISTER_EMPTY)==0);
    UDR2=c;
}
#pragma used-


/*============================================================*/
// USART3 Receiver interrupt service routine
interrupt [USART3_RXC] void usart3_rx_isr(void)
{
    char status,data;
    status=UCSR3A;
    data=UDR3;     
    //putchar3(data);        
    //putchar0(data);
    if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
    {                         
        putchar3(data);      
//        putchar0(data);
        // Detect Package Header
        if(data == charHeader)
        {
            rxPackageIndex3 = 0;
        }
        
        rxPackage3[rxPackageIndex3++] = data;         
        
        if(rxPackageIndex3 >= 64) rxPackageIndex3 = 0;      
        
        if(data == charTerminate)
        {
            rxPackageIndexCount3 = rxPackageIndex3-1;
            rxPackageIndex3 = 0;
            flag_PackageTerminate3 = 1;
        }
    }
}

/*============================================================*/
// Write a character to the USART3 Transmitter
#pragma used+
void putchar3(char c)
{
    while ((UCSR3A & DATA_REGISTER_EMPTY)==0);
    UDR3=c;
}
#pragma used-

/*============================================================*/
void send_uart(char port,char *buffer)
{
    char i = 0;
    
    switch (port)
    {
        case 0: puts(buffer);
                break;

        case 1: while(buffer[i] != 0)    
                {
                    //putchar1(buffer[i]);
                    i++;
                }
                break;

        case 2: while(buffer[i] != 0)    
                {
                    putchar2(buffer[i]);
                    i++;
                }
                break;

        case 3: while(buffer[i] != 0)    
                {
                    putchar3(buffer[i]);
                    i++;
                }
                break;
    }    
}

/*============================================================*/
void init_uart(void)
{
//    // USART0 initialization
//    // Communication Parameters: 8 Data, 1 Stop, No Parity
//    // USART0 Receiver: On
//    // USART0 Transmitter: On
//    // USART0 Mode: Asynchronous
//    // USART0 Baud Rate: 38400
//    UCSR0A=0x00;
//    UCSR0B=0x98;
//    UCSR0C=0x06;
//    UBRR0H=0x00;
//    UBRR0L=0x0C;

    // USART0 initialization
    // Communication Parameters: 8 Data, 1 Stop, No Parity
    // USART0 Receiver: On
    // USART0 Transmitter: On
    // USART0 Mode: Asynchronous
    // USART0 Baud Rate: 9600
    UCSR0A=0x00;
    UCSR0B=0x98;
    UCSR0C=0x06;
    UBRR0H=0x00;
    UBRR0L=0x33;

    // USART1 initialization
    // Communication Parameters: 8 Data, 1 Stop, No Parity
    // USART1 Receiver: On
    // USART1 Transmitter: On
    // USART1 Mode: Asynchronous
    // USART1 Baud Rate: 38400
    UCSR1A=0x00;
    UCSR1B=0x98;
    UCSR1C=0x06;
    UBRR1H=0x00;
    UBRR1L=0x0C;

//    // USART2 initialization
//    // Communication Parameters: 8 Data, 1 Stop, No Parity
//    // USART2 Receiver: On
//    // USART2 Transmitter: On
//    // USART2 Mode: Asynchronous
//    // USART2 Baud Rate: 115200 (Double Speed Mode)
//    UCSR2A=0x02;
//    UCSR2B=0x98;
//    UCSR2C=0x06;
//    UBRR2H=0x00;
//    UBRR2L=0x08;

    // USART2 initialization
    // Communication Parameters: 8 Data, 1 Stop, No Parity
    // USART2 Receiver: On
    // USART2 Transmitter: On
    // USART2 Mode: Asynchronous
    // USART2 Baud Rate: 38400
    UCSR2A=0x00;
    UCSR2B=0x98;
    UCSR2C=0x06;
    UBRR2H=0x00;
    UBRR2L=0x0C;

    // USART3 initialization
    // Communication Parameters: 8 Data, 1 Stop, No Parity
    // USART3 Receiver: On
    // USART3 Transmitter: On
    // USART3 Mode: Asynchronous
    // USART3 Baud Rate: 38400
    UCSR3A=0x00;
    UCSR3B=0x98;
    UCSR3C=0x06;
    UBRR3H=0x00;
    UBRR3L=0x0C;
}

/*============================================================*/

