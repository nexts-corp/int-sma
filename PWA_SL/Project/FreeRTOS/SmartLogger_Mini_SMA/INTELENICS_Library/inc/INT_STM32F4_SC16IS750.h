/* SC16IS750 I2C or SPI to UART bridge 
 *   v0.1 WH, Nov 2013, Sparkfun WiFly Shield code library alpha 0 used as example, Added I2C I/F and many more methods.
 *                      https://forum.sparkfun.com/viewtopic.php?f=13&t=21846
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef _SC16IS750_H
#define _SC16IS750_H
 
// #include "Stream.h"
//#include <SerialBase.h>
#include "stdint.h"

#include <stdio.h>
#include <stdlib.h>
#include <String.h>
#include <stdbool.h>
 
//I2C Slaveaddresses                      A1  A0 
#define SC16IS750_SA0            0x90  /* VDD VDD */
#define SC16IS750_SA1            0x92  /* VDD VSS */
#define SC16IS750_SA2            0x94  /* VDD SCL */
#define SC16IS750_SA3            0x95  /* VDD SDA */
#define SC16IS750_SA4            0x98  /* VSS VDD */
#define SC16IS750_SA5            0x9A  /* VSS VSS */
#define SC16IS750_SA6            0x9C  /* VSS SCL */
#define SC16IS750_SA7            0x9E  /* VSS SDA */
#define SC16IS750_SA8            0xA0  /* SCL VDD */
#define SC16IS750_SA9            0xA2  /* SCL VSS */
#define SC16IS750_SA10           0xA4  /* SCL SCL */
#define SC16IS750_SA11           0xA6  /* SCL SDA */
#define SC16IS750_SA12           0xA8  /* SDA VDD */
#define SC16IS750_SA13           0xAA  /* SDA VSS */
#define SC16IS750_SA14           0xAC  /* SDA SCL */
#define SC16IS750_SA15           0xAE  /* SDA SDA */
 
//Default I2C Slaveaddress
#define SC16IS750_DEFAULT_ADDR   SC16IS750_SA5

/** See datasheet section 7.8 for configuring the
  * "Programmable baud rate generator"
  */
#define SC16IS750_XTAL_FREQ              14745600UL /* On-board crystal (New mid-2010 Version) */
#define SC16IS750_PRESCALER_1                   1   /* Default prescaler after reset           */
#define SC16IS750_PRESCALER_4                   4   /* Selectable by setting MCR[7]            */
#define SC16IS750_PRESCALER                      SC16IS750_PRESCALER_1  
#define SC16IS750_BAUDRATE_DIVISOR(baud)       ((SC16IS750_XTAL_FREQ/SC16IS750_PRESCALER)/(baud*16UL))

//Default baudrate
#define SC16IS750_DEFAULT_BAUDRATE           9600

/** See section 8.3 of the datasheet for definitions
  * of bits in the FIFO Control Register (FCR)
  */
#define FCR_RX_IRQ_60                 (3 << 6)
#define FCR_RX_IRQ_56                 (2 << 6)
#define FCR_RX_IRQ_16                 (1 << 6)
#define FCR_RX_IRQ_8                  (0 << 6)
//TX Level only accessible when EFR[4] is set
#define FCR_TX_IRQ_56                 (3 << 4)
#define FCR_TX_IRQ_32                 (2 << 4)
#define FCR_TX_IRQ_16                 (1 << 4)
#define FCR_TX_IRQ_8                  (0 << 4)
//#define FCR_RESERVED                  (1 << 3)
#define FCR_TX_FIFO_RST               (1 << 2)
#define FCR_RX_FIFO_RST               (1 << 1)
#define FCR_ENABLE_FIFO               (1 << 0)

//FIFO size
#define SC16IS750_FIFO_RX                  64
#define SC16IS750_FIFO_TX                  64

/** See section 8.4 of the datasheet for definitions
  * of bits in the Line Control Register (LCR)
  */
#define LCR_BITS5                      0x00
#define LCR_BITS6                      0x01
#define LCR_BITS7                      0x02
#define LCR_BITS8                      0x03

#define LCR_BITS1                      0x00
#define LCR_BITS2                      0x04

#define LCR_NONE                       0x00
#define LCR_ODD                        0x08
#define LCR_EVEN                       0x18
#define LCR_FORCED1                    0x28
#define LCR_FORCED0                    0x38

#define LCR_BRK_ENA                    0x40
#define LCR_BRK_DIS                    0x00

#define LCR_ENABLE_DIV                 0x80
#define LCR_DISABLE_DIV                0x00

#define LCR_ENABLE_ENHANCED_FUNCTIONS (0xBF)


/** See section 8.5 of the datasheet for definitions
  * of bits in the Line status register (LSR)
  */
#define LSR_DR   (0x01) /* Data ready in RX FIFO                       */
#define LSR_OE   (0x02) /* Overrun error                               */
#define LSR_PE   (0x04) /* Parity error                                */
#define LSR_FE   (0x08) /* Framing error                               */
#define LSR_BI   (0x10) /* Break interrupt                             */
#define LSR_THRE (0x20) /* Transmitter holding register (FIFO empty)   */
#define LSR_TEMT (0x40) /* Transmitter empty (FIFO and TSR both empty) */
#define LSR_FFE  (0x80) /* At least one PE, FE or BI in FIFO           */


/** See section 8.6 of the datasheet for definitions
  * of bits in the Modem control register (MCR)
  */
#define MCR_MDTR                      (1 << 0) /* Data Terminal Ready pin control. */
#define MCR_MRTS                      (1 << 1) /* Request to Send pin control when not in Auto RTS mode.*/
//MCR[2] only accessible when EFR[4] is set
#define MCR_ENABLE_TCR_TLR            (1 << 2)
#define MCR_ENABLE_LOOPBACK           (1 << 4)
//MCR[7:5] only accessible when EFR[4] is set
#define MCR_ENABLE_XON_ANY_CHAR       (1 << 5)
#define MCR_ENABLE_IRDA               (1 << 6)
#define MCR_PRESCALE_1                (0 << 7)
#define MCR_PRESCALE_4                (1 << 7)


/** See section 8.7 of the datasheet for definitions
  * of bits in the Modem status register (MSR)
  */
#define MSR_DCTS                      (1 << 0) /* Delta CTS - CTS Changed State      */
#define MSR_DDSR                      (1 << 1) /* Delta DSR - DSR Changed State      */
#define MSR_DDI                       (1 << 2) /* Delta DI  - DI  Changed State      */
#define MSR_DCD                       (1 << 3) /* Delta CD  - CD  Changed State      */
#define MSR_CTS                       (1 << 4) /* CTS State - Complement of NCTS pin */
//MSR[7:5] only accessible when GPIO[7:4] are set as modem pin
#define MSR_DSR                       (1 << 5) /* DSR State - Complement of NDSR pin */
#define MSR_RI                        (1 << 6) /* RI State  - Complement of  NRI pin */
#define MSR_CD                        (1 << 7) /* CD State  - Complement of  NCD pin */


/** See section 8.8 of the datasheet for definitions
  * of bits in the Interrupt enable register (IER)
  */
#define IER_ERHRI (0x01) /* Enable received data available interrupt            */
#define IER_ETHRI (0x02) /* Enable transmitter holding register empty interrupt */
#define IER_ELSI  (0x04) /* Enable receiver line status interrupt               */
#define IER_EMSI  (0x08) /* Enable modem status interrupt                       */
//IER[7:5] only accessible when EFR[4] is set
#define IER_SLEEP (0x10) /* Enable sleep mode                                   */
#define IER_XOFFI (0x20) /* Enable XOFF interrupt                               */
#define IER_RTSI  (0x40) /* Enable RTS interrupt                                */
#define IER_CTSI  (0x80) /* Enable CTS interrupt                                */


/** See section 8.9 of the datasheet for definitions
  * of bits in the Interrupt identification register (IIR)
  * Bit 0 is set to 0 if an IRQ is pending.
  * Bits 1..5 are used to identify the IRQ source.
  */
#define IIR_IRQ_NOT_PENDING             (0x01)  /* IRQ Not Pending              */
#define IIR_TX_EMPTY                    (0x02)  /* THR Interrupt                */
#define IIR_RX_DATA                     (0x04)  /* RHR Interrupt                */
#define IIR_RX_ERROR                    (0x06)  /* Line Status Error Interrupt  */
#define IIR_RX_TIMEOUT                  (0x0B)  /* RX Timeout Interrupt         */
#define IIR_RX_XOFF                     (0x10)  /* RX XOff Interrupt            */
#define IIR_DCTS_DRTS                   (0x20)  /* Delta CTS or RTS Interrupt   */
#define IIR_DIO                         (0x30)  /* Delta GPIO pin Interrupt     */

#define IIR_BITS_USED                   (0x07)


/** See section 8.10 of the datasheet for definitions
  * of bits in the Enhanced Features Register (EFR)
  */
#define EFR_ENABLE_CTS                  (1 << 7)
#define EFR_ENABLE_RTS                  (1 << 6)
#define EFR_ENABLE_XOFF2_CHAR_DETECT    (1 << 5)
#define EFR_ENABLE_ENHANCED_FUNCTIONS   (1 << 4)
// EFR[3:0] are used to define Software Flow Control mode
// See section 7.3
#define EFR_DISABLE_TX_FLOW_CTRL        (0x0 << 2)
#define EFR_TX_XON2_XOFF2               (0x1 << 2)
#define EFR_TX_XON1_XOFF1               (0x2 << 2)
#define EFR_TX_XON2_1_XOFF2_1           (0x3 << 2)

#define EFR_DISABLE_RX_FLOW_CTRL        (0x0 << 0)
#define EFR_RX_XON2_XOFF2               (0x1 << 0)
#define EFR_RX_XON1_XOFF1               (0x2 << 0)
#define EFR_RX_XON2_1_XOFF2_1           (0x3 << 0)

#define EFR_TX_XON2_XOFF2_RX_FLOW       (0x1 << 2) | (0x3 << 0)
#define EFR_TX_XON1_XOFF1_RX_FLOW       (0x2 << 2) | (0x3 << 0)
#define EFR_TX_XON2_1_XOFF2_1_RX_FLOW   (0x3 << 2) | (0x3 << 0)

/** See section 8.12 of the datasheet for definitions
  * of bits in the Transmission Control Register (TCR)
  * These levels control when RTS is asserted or de-asserted and auto RTS is enabled. Note that XON/XOFF is not supported in this lib.
  *   Trigger level to halt transmission to the device   : 0..15 (meaning 0-60 with a granularity of 4) 
  *     RTS is de-asserted when RX FIFO is above the set trigger level (i.e. buffer is getting full)  
  *   Trigger level to resume transmission to the device : 0..15 (meaning 0-60 with a granularity of 4) 
  *     RTS is asserted again when RX FIFO drops below the set trigger level (i.e. buffer has room again)    
  */
#define TCR_HALT_DEFAULT                (0x0E)
#define TCR_RESUME_DEFAULT              (0x08)  
 
/** See section 8.12 of the datasheet for definitions
  * Note: The device will stop transmissions from the TX FIFO when CTS is de-asserted by external receiver and 
  *       auto CTS is enabled. Note that XON/XOFF is not supported in this lib.
  */


/** See section 7.5 and 8.13 of the datasheet for definitions
  * of bits in the Trigger Level Register (TLR) control when an IRQ is generated.
  *   Trigger level for TX interrupt: 0..15 (meaning 0-60 with a granularity of 4) 
  *     IRQ when TX FIFO is above the set trigger level (i.e. buffer is getting full)
  *   Trigger level for RX interrupt: 0..15 (meaning 0-60 with a granularity of 4) 
  *     IRQ when RX FIFO is above the set trigger level (i.e. data is waiting to be read)
  */
#define TLR_TX_DEFAULT                  (0x0E)
#define TLR_RX_DEFAULT                  (0x04)  

/**
  * See section 8.19 of the datasheet for definitions
  * of bits in the IO Control register (IOC)
  * 
  * Bit 0 is set to 0 to enable latch of IO inputs.
  * Bit 1 is set to enable GPIO[7-4] as /RI, /CD, /DTR, /DST.
  * Bit 2 is set to enable software reset.
  */
#define IOC_ENA_LATCH                   (0x01)
#define IOC_ENA_MODEM                   (0x02) /* Set GPIO[7:4] pins to modem functions */
#define IOC_SW_RST                      (0x04) 


/**
  * See section 8.20 of the datasheet for definitions
  * of bits in the Extra Features Control register (EFCR)
  * 
  */
#define EFCR_ENA_RS485                  (0x01)  
#define EFCR_DIS_RX                     (0x02)    
#define EFCR_DIS_TX                     (0x04)    
#define EFCR_ENA_TX_RTS                 (0x10)    
#define EFCR_INV_RTS_RS485              (0x20)    
#define EFCR_ENA_IRDA                   (0x80)    
 
// See Chapter 11 of datasheet
#define SPI_READ_MODE_FLAG              (0x80)
 
//  SC16IS750 Register definitions (shifted to align)
typedef enum { 
/*
 * 16750 addresses. Registers accessed when LCR[7] = 0.
 */   
		RHR         = 0x00 << 3, /* Rx buffer register     - Read access  */
		THR         = 0x00 << 3, /* Tx holding register    - Write access */
		IER         = 0x01 << 3, /* Interrupt enable reg   - RD/WR access */

/*
 * 16750 addresses. Registers accessed when LCR[7] = 1.
 */       
		DLL         = 0x00 << 3, /* Divisor latch (LSB)    - RD/WR access */
		DLH         = 0x01 << 3, /* Divisor latch (MSB)    - RD/WR access */

/*
 * 16750 addresses. IIR/FCR is accessed when LCR[7:0] <> 0xBF.
 *                  Bit 5 of the FCR register is accessed when LCR[7] = 1.
 */       
		IIR         = 0x02 << 3, /* Interrupt id. register - Read only    */
		FCR         = 0x02 << 3, /* FIFO control register  - Write only   */
/*
 * 16750 addresses. EFR is accessed when LCR[7:0] = 0xBF.
 */       
		EFR         = 0x02 << 3, /* Enhanced features reg  - RD/WR access */     

/*
 * 16750 addresses.
 */       
		LCR         = 0x03 << 3, /* Line control register  - RD/WR access */
/*
 * 16750 addresses. MCR/LSR is accessed when LCR[7:0] <> 0xBF.
 *                  Bit 7 of the MCR register is accessed when EFR[4] = 1.
 */       
		MCR         = 0x04 << 3, /* Modem control register - RD/WR access */
		LSR         = 0x05 << 3, /* Line status register   - Read only    */

/*
 * 16750 addresses. MSR/SPR is accessed when LCR[7:0] <> 0xBF.
 *                  MSR, SPR register is accessed when EFR[1]=0 and MCR[2]=0.
 */       
		MSR         = 0x06 << 3, /* Modem status register  - Read only    */
		SPR         = 0x07 << 3, /* Scratchpad register    - RD/WR access */
/*
 * 16750 addresses. TCR/TLR is accessed when LCR[7:0] <> 0xBF.
 *                  TCR, TLR register is accessed when EFR[1]=1 and MCR[2]=1.
 */       
		TCR         = 0x06 << 3, /* Transmission control register - RD/WR access */
		TLR         = 0x07 << 3, /* Trigger level register        - RD/WR access */

/*
 * 16750 addresses. XON, XOFF is accessed when LCR[7:0] = 0xBF.
 */       
		XON1        = 0x04 << 3, /* XON1 register          - RD/WR access */
		XON2        = 0x05 << 3, /* XON2 register          - RD/WR access */
		XOFF1       = 0x06 << 3, /* XOFF1 register         - RD/WR access */
		XOFF2       = 0x07 << 3, /* XOFF2 register         - RD/WR access */

/*
 * 16750 addresses.
 */       
		TXLVL       = 0x08 << 3, /* TX FIFO Level register - Read only    */
		RXLVL       = 0x09 << 3, /* RX FIFO Level register - Read only    */
		IODIR       = 0x0A << 3, /* IO Pin Direction reg   - RD/WR access */
		IOSTATE     = 0x0B << 3, /* IO Pin State reg       - Read only    */
		IOINTENA    = 0x0C << 3, /* IO Interrupt Enable    - RD/WR access */
//        reserved    = 0x0D << 3,
		IOCTRL      = 0x0E << 3, /* IO Control register    - RD/WR access */
		EFCR        = 0x0F << 3, /* Extra features reg     - RD/WR access */

}RegisterName ;
 
typedef enum {
	None,
	Odd,
	Even,
	Forced1,
	Forced0
}Parity;

// This enum used to be part of SerialBase class (access via SerialBase.h).
//  It seems not be supported anymore. The enums for Parity have moved to Serial now..  
typedef enum {
		Disabled = 0,
		RTS,
		CTS,
		RTSCTS
}Flow;
 
  
// SC16IS750 configuration register values
// Several configuration registers are write-only. Need to save values to allow restoring.
typedef struct {
	char baudrate;
  char dataformat;  
  char flowctrl;  
  char fifoformat;
  char fifoenable; 
}SC16IS750_cfg;

int SC16IS750_readable( void );
int SC16IS750_readableCount( void );
int SC16IS750_writable( void ); 
int SC16IS750_writableCount( void );
void SC16IS750_writeString( const char *str );
void SC16IS750_writeBytes(const char *data, int len);
int SC16IS750_readString(char *data, int len);
void SC16IS750_baud(int baudrate );   
void SC16IS750_format(int bits, Parity parity, int stop_bits);
void SC16IS750_send_break( void );
void SC16IS750_set_break(bool enable);
// void set_flow_control(Flow type=Disabled, PinName flow1=NC, PinName flow2=NC);
// void set_flow_triggers(int resume = TCR_RESUME_DEFAULT, int halt = TCR_HALT_DEFAULT);
void SC16IS750_set_modem_control(void);
bool SC16IS750_connected(void);
void SC16IS750_set_fifo_control(void);
void SC16IS750_flush(void);
void SC16IS750_ioSetDirection(unsigned char bits);
void SC16IS750_ioSetState(unsigned char bits);
unsigned char SC16IS750_ioGetState(void);
void SC16IS750_swReset(void);
void SC16IS750_init(void);


void SC16IS750_I2C_writeRegister(RegisterName register_address, char data );
char SC16IS750_I2C_readRegister(RegisterName register_address );
void SC16IS750_I2C_writeDataBlock (const char *data, int len );
 
#endif  //  _SC16IS750_H
/*********** Portions COPYRIGHT 2014 Intelenics. Co., Ltd.*****END OF FILE****/
