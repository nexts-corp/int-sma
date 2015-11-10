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
#include    "INT_STM32F4_SC16IS750.h"

#include <stdio.h>
#include <stdlib.h>
#include <String.h>
#include <stdbool.h>

#include "main.h"

#include "INT_STM32F4_i2c.h"

#define ENABLE_BULK_TRANSFERS             1
#define BULK_BLOCK_LEN                   16

SC16IS750_cfg _config;

static void SC16IS750_set_flow_control(Flow type);

/*==========================================================================================*/
/**
  * @brief  Initialise internal registers
  * 				Should be in protection section. Public for testing purposes
  * 				If initialisation fails this method does not return. 
  * @param  None
  * @retval None
  */
void SC16IS750_init(void) {
 
  // Initialise SC16IS750
 
  // Software reset, assuming there is no access to the HW Reset pin
  SC16IS750_swReset();
 
  // Set default baudrate (depends on prescaler) and save in _config
  // DLL/DLH  
  SC16IS750_baud( 9600 );
 
  // Set default dataformat and save in _config
  // LCR 
  SC16IS750_format( 8, None, 1 );  
 
  // Set dataflow mode and Enables enhanced functions
  // Save in _config
  // EFR
  SC16IS750_set_flow_control( Disabled ); 
  
 
  // FIFO control, sets TX and RX IRQ trigger levels and enables FIFO and save in _config
  // Note FCR[5:4] only accessible when EFR[4] is set (enhanced functions enable)
  // FCR, TLR
  SC16IS750_set_fifo_control();
  SC16IS750_flush();
 
  // Modem control, sets prescaler, enable TCR and TLR
  // Note MCR[7:4] and MCR[2] only accessible when EFR[4] is set (enhanced functions enable)
//    set_modem_control();
 
  // Set RTS trigger levels
  // Note TCR only accessible when EFR[4] is set (enhanced functions enable) and MCR[2] is set
//   set_flow_triggers();
 
 
  // Set default break condition and save in _config
  // LCR   
  //set_break();
 
  // The UART bridge should now be successfully initialised.
 
  // Test if UART bridge is present and initialised
  if(!SC16IS750_connected()){ 
#if(0)  
    // Lock up if we fail to initialise UART bridge.
    while(1) {};
#else    
    printf("Failed to initialise UART bridge\r\n");    
#endif    
  }
  else {
    printf("Initialised UART bridge!\r\n");      
  }
 
}
/*==========================================================================================*/
/**
  * @brief  Set baudrate of the serial port. 
  * @param  baudrate: baud integer baudrate (4800, 9600 etc).
  * @retval None
  */
void SC16IS750_baud( int baudrate ) {
  unsigned long divisor = SC16IS750_BAUDRATE_DIVISOR(baudrate);
  char lcr_tmp;
  
  _config.baudrate = baudrate;               // Save baudrate
 
  lcr_tmp = SC16IS750_I2C_readRegister(LCR);                            // Read current LCR register
  SC16IS750_I2C_writeRegister(LCR, lcr_tmp | LCR_ENABLE_DIV);           // Enable Divisor registers
  SC16IS750_I2C_writeRegister(DLL, ( divisor       & 0xFF));            //   write divisor LSB
  SC16IS750_I2C_writeRegister(DLH, ((divisor >> 8) & 0xFF));            //   write divisor MSB
  SC16IS750_I2C_writeRegister(LCR, lcr_tmp);                            // Restore LCR register, activate regular RBR, THR and IER registers  
}
 
/*==========================================================================================*/
/**
  * @brief  Set the transmission format used by the serial port. 
  * @param  bits: 			The number of bits in a word (5-8; default = 8).
	*					parity: 		The parity used (None, Odd, Even, Forced1, Forced0).
	*					stop_bits:	The number of stop bits (1 or 2; default = 1). 
  * @retval None
  */
void SC16IS750_format(int bits, Parity parity, int stop_bits) {
  char lcr_tmp = 0x00;
  
  switch (bits) {
    case 5:  lcr_tmp |= LCR_BITS5;
             break;
    case 6:  lcr_tmp |= LCR_BITS6;
             break;
    case 7:  lcr_tmp |= LCR_BITS7;
             break;
    case 8:  lcr_tmp |= LCR_BITS8;
             break;
    default: lcr_tmp |= LCR_BITS8;     
  }
 
  switch (parity) {
    case None:    lcr_tmp |= LCR_NONE;
                          break;
    case Odd:     lcr_tmp |= LCR_ODD;
                          break;
    case Even:    lcr_tmp |= LCR_EVEN;
                          break;
    case Forced1: lcr_tmp |= LCR_FORCED1;
                          break;
    case Forced0: lcr_tmp |= LCR_FORCED0;
                          break;                      
    default:              lcr_tmp |= LCR_NONE;     
  }
 
  switch (stop_bits) {
    case 1:  lcr_tmp |= LCR_BITS1;
             break;
    case 2:  lcr_tmp |= LCR_BITS2;
             break;
    default: lcr_tmp |= LCR_BITS1;     
  }
 
  _config.dataformat = lcr_tmp;      // Save dataformat   
 
  SC16IS750_I2C_writeRegister(LCR, lcr_tmp); // Set LCR register, activate regular RBR, THR and IER registers  
 
}
/*==========================================================================================*/
/**
  * @brief  Generate a break condition on the serial line. 
  * @param  None
  * @retval None
  */
void SC16IS750_send_break(void) {
  /* Wait for 1.5 frames before clearing the break condition
		This will have different effects on our platforms, but should
		ensure that we keep the break active for at least one frame.
		We consider a full frame (1 start bit + 8 data bits bits + 
		1 parity bit + 2 stop bits = 12 bits) for computation.
		One bit time (in us) = 1000000/_baud
		Twelve bits: 12000000/baud delay
		1.5 frames: 18000000/baud delay
	*/
  SC16IS750_set_break(true);
  delay_ms(18000000/_config.baudrate);
  SC16IS750_set_break(false);    
}
/*==========================================================================================*/
/**
  * @brief  Set a break condition on the serial line. 
  * @param  enable: break condition
  * @retval None
  */
void SC16IS750_set_break(bool enable) {
 
  if (enable) {
    _config.dataformat |= LCR_BRK_ENA;      // Save dataformat         
  }
  else {
    _config.dataformat &= ~LCR_BRK_ENA;     // Save dataformat           
  }
 
  SC16IS750_I2C_writeRegister(LCR, _config.dataformat); // Set LCR register
}
/*==========================================================================================*/
/**
  * @brief  Set the flow control type on the serial port
  *  				Added for compatibility with Serial Class.
  *  				SC16IS750 supports only Flow, Pins can not be selected.
  *  				This method sets hardware flow control. SC16IS750 supports XON/XOFF, but this is not implemented. 
  * @param  Flow: type the flow control type (Disabled, RTS, CTS, RTSCTS)
  * @retval None
  */
void SC16IS750_set_flow_control(Flow type) {
  char lcr_tmp; 
  char efr_tmp = 0x00;
  
  // We need to enable flow control to prevent overflow of buffers and
  // lose data when used with fast devices like the WiFly.
  switch (type) {
     case Disabled : 
                     break;
     case RTS:       efr_tmp = EFR_ENABLE_RTS;
                     break;     
     case CTS:       efr_tmp = EFR_ENABLE_CTS;                     
                     break;     
     case RTSCTS:    efr_tmp = EFR_ENABLE_RTS | EFR_ENABLE_CTS;
                     break;
     default:             ;    
 
  }
  //Save flowcontrol mode and enable enhanced functions  
  _config.flowctrl = efr_tmp | EFR_ENABLE_ENHANCED_FUNCTIONS;
 
  lcr_tmp = SC16IS750_I2C_readRegister(LCR);                       // save LRC register
  SC16IS750_I2C_writeRegister(LCR, LCR_ENABLE_ENHANCED_FUNCTIONS); // write magic number 0xBF to enable access to EFR register
  SC16IS750_I2C_writeRegister(EFR, _config.flowctrl);              // set flow and enable enhanced functions
  SC16IS750_I2C_writeRegister(LCR, lcr_tmp);                       // restore LCR register
}  
 
/** Set the RX FIFO flow control levels
  *  This method sets hardware flow control levels. SC16IS750 supports XON/XOFF, but this is not implemented.
  *  Should be called BEFORE Auto RTS is enabled.  
  *
  *  @param resume trigger level to resume transmission (0..15, meaning 0-60 with a granularity of 4)     
  *  @param halt trigger level to resume transmission (0..15, meaning 0-60 with granularity of 4)       
  *  @return none   
  */
// void SC16IS750::set_flow_triggers(int resume, int halt) {
//  
//   // sanity checks
//   halt = halt & 0x0F;
//   resume = resume & 0x0F;  
//   if (halt <= resume) {
//     halt   = TCR_HALT_DEFAULT;
//     resume = TCR_RESUME_DEFAULT;  
//   }
//  
//   // Note: TCR accessible only when EFR[4]=1 and MCR[2]=1
//   this->writeRegister(TCR, (resume << 4) | halt);          // set TCR register
// }
 
 
/** Set the Modem Control register
  *  This method sets prescaler, enables TCR and TLR
  *
  *  @param none 
  *  @return none 
  */
// void SC16IS750::set_modem_control() {
//  
//   //Note MCR[7:4] and MCR[2] only accessible when EFR[4] is set
//   if (SC16IS750_PRESCALER == SC16IS750_PRESCALER_1) { // Default prescaler after reset
//     this->writeRegister(MCR, MCR_PRESCALE_1 | MCR_ENABLE_TCR_TLR);
//   }  
//   else { 
//     this->writeRegister(MCR, MCR_PRESCALE_4 | MCR_ENABLE_TCR_TLR);
//   }
// }  

/*==========================================================================================*/
/**
  * @brief  FIFO control, sets TX and RX trigger levels and enables FIFO and save in _config
  *  				Note FCR[5:4] (=TX_IRQ_LVL) only accessible when EFR[4] is set (enhanced functions enable)
  *  				Note TLR only accessible when EFR[4] is set (enhanced functions enable) and MCR[2] is set  
  * @param  None
  * @retval None
  */
void SC16IS750_set_fifo_control(void) {
 
  // Set default fifoformat 
  // FCR
  _config.fifoenable = true;  
 
  // Note FCR[5:4] (=TX_IRQ_LVL) only accessible when EFR[4] is set (enhanced functions enable)
	// _config.fifoformat = FCR_RX_IRQ_8 | FCR_TX_IRQ_56;
  _config.fifoformat = FCR_RX_IRQ_8 | FCR_TX_IRQ_8;  //Default
 
  if (_config.fifoenable)
    // enable FIFO mode and set FIFO control values  
    SC16IS750_I2C_writeRegister(FCR, _config.fifoformat | FCR_ENABLE_FIFO);
  else
    // disable FIFO mode and set FIFO control values  
    SC16IS750_I2C_writeRegister(FCR, _config.fifoformat);
 
  // Set Trigger level register TLR for RX and TX interrupt generation
  // Note TLR only accessible when EFR[4] is set (enhanced functions enable) and MCR[2] is set
  // TRL Trigger levels for RX and TX are 0..15, meaning 0-60 with a granularity of 4 chars    
  // When TLR for RX or TX are 'Zero' the corresponding values in FCR are used. The FCR settings
  // have less resolution (only 4 levels) so TLR is considered an enhanced function.
  SC16IS750_I2C_writeRegister(TLR, 0x00);                                     // Use FCR Levels
//  this->writeRegister(TLR, (TLR_RX_DEFAULT << 4) | TLR_TX_DEFAULT);   // Use Default enhanced levels
 
}
/*==========================================================================================*/
/**
  * @brief  Flush the UART FIFOs while maintaining current FIFO mode.  
  * @param  None
  * @retval None
  */
void SC16IS750_flush(void) {
  // FCR is Write Only, use saved _config
 
  // reset TXFIFO, reset RXFIFO, non FIFO mode  
  SC16IS750_I2C_writeRegister(FCR, FCR_TX_FIFO_RST | FCR_RX_FIFO_RST);
  
  if (_config.fifoenable)
    // enable FIFO mode and set FIFO control values  
    SC16IS750_I2C_writeRegister(FCR, _config.fifoformat | FCR_ENABLE_FIFO);
  else
    // disable FIFO mode and set FIFO control values  
    SC16IS750_I2C_writeRegister(FCR, _config.fifoformat);
  
#if(0)
//original
 /*
   * Flush characters from SC16IS750 receive buffer.
   */
 
  // Note: This may not be the most appropriate flush approach.
  //       It might be better to just flush the UART's buffer
  //       rather than the buffer of the connected device
  //       which is essentially what this does.
  while(SC16IS750_readable() > 0) {
    getc();
  }
#endif
 
}
/*==========================================================================================*/
/**
  * @brief  Check that UART is connected and operational.  
  * @param  None
  * @retval bool true when connected, false otherwise.
  */
bool SC16IS750_connected(void) {
  // Perform read/write test to check if UART is working
  const char TEST_CHARACTER = 'H';
 
  SC16IS750_I2C_writeRegister(SPR, TEST_CHARACTER);
 
  return (SC16IS750_I2C_readRegister(SPR) == TEST_CHARACTER);
}
/*==========================================================================================*/
/**
  * @brief  Determine if there is a character available to read.
  * 				This is data that's already arrived and stored in the receive
  * 				buffer (which holds 64 chars).  
  * @param  None
  * @retval 1 if there is a character available to read, 0 otherwise.
  */
int SC16IS750_readable(void) {
  
//  if (this->readableCount() > 0) { // Check count
  if (SC16IS750_I2C_readRegister(LSR) & LSR_DR) { // Data in Receiver Bit, at least one character waiting
    return 1;
  }
  else {
    return 0; 
  }
}
/*==========================================================================================*/
/**
  * @brief  Determine how many characters are available to read.
  * 				This is data that's already arrived and stored in the receive
  * 				buffer (which holds 64 chars).  
  * @param  None
  * @retval int Characters available to read.
  */
int SC16IS750_readableCount(void) {
 
  return (SC16IS750_I2C_readRegister(RXLVL));
}
/*==========================================================================================*/
/**
  * @brief  Determine if there is space available to write a character. 
  * @param  None
  * @retval 1 if there is a space for a character to write, 0 otherwise.
  */
int SC16IS750_writable(void) {
 
//  if ((this->writableCount() > 0) { // Check count
  if (SC16IS750_I2C_readRegister(LSR) & LSR_THRE) { // THR Empty, space for at least one character
    return 1;
  }
  else {
    return 0;  
  }
}

/*==========================================================================================*/
/**
  * @brief  Determine how much space available for writing characters.
  * 				This considers data that's already stored in the transmit
  * 				buffer (which holds 64 chars). 
  * @param  None
  * @retval int character space available to write.
  */
int SC16IS750_writableCount(void) {
 
  return (SC16IS750_I2C_readRegister(TXLVL));  // TX Level
}
/*==========================================================================================*/
/**
  * @brief  Read char from UART Bridge.
  * 				Acts in the same manner as 'Serial.read()'. 
  * @param  None
  * @retval char read or -1 if no data available.
  */
int SC16IS750_getc() {
 
  if (!SC16IS750_readable()) {
    return -1;
  }
 
  return SC16IS750_I2C_readRegister(RHR);
}
/*==========================================================================================*/
/**
  * @brief  Write char to UART Bridge. Blocking when no free space in FIFO.
  * 				Acts in the same manner as 'Serial.read()'. 
  * @param  value char to be written
  * @retval value written.
  */
int SC16IS750_putc(int value) {
 
  while (SC16IS750_I2C_readRegister(TXLVL) == 0) {
    // Wait for space in TX buffer
//     wait_us(10);
  };
  SC16IS750_I2C_writeRegister(THR, value);
  
  return value;
}
/*==========================================================================================*/
/**
  * @brief  Write char string to UART Bridge. Blocking when no free space in FIFO. 
  * @param  *str: char string to be written
  * @retval none.
  */
void SC16IS750_writeString(const char *str) {
 
#if ENABLE_BULK_TRANSFERS
  int len, idx;
  
  len = strlen(str);  
 
  // Write blocks of BULK_BLOCK_LEN  
  while (len > BULK_BLOCK_LEN) {
    while(SC16IS750_I2C_readRegister(TXLVL) < BULK_BLOCK_LEN) {
      // Wait for space in TX buffer
//       wait_us(10);
    };  
  
    // Write a block of BULK_BLOCK_LEN bytes
#if (0)    
    // Note: can be optimized by writing registeraddress once and then repeatedly write the bytes.
    for (idx=0; idx<BULK_BLOCK_LEN; idx++) {
      SC16IS750_I2C_writeRegister(THR, str[idx]);
    };
#else
    // optimized
    SC16IS750_I2C_writeDataBlock(str, BULK_BLOCK_LEN);    
#endif
              
    len -= BULK_BLOCK_LEN;
    str += BULK_BLOCK_LEN;
  }
  
  // Write remaining bytes 
  // Note: can be optimized by writing registeraddress once and then repeatedly write the bytes.  
  for (idx=0; idx<len; idx++) {
    while (SC16IS750_I2C_readRegister(TXLVL) == 0) {
      // Wait for space in TX buffer
//       wait_us(10);
    };
    SC16IS750_I2C_writeRegister(THR, str[idx]);
  }  
 
 
#else
  // Single writes instead of bulktransfer
  int len, idx;
  
  len = strlen(str);
  for (idx=0; idx<len; idx++) {
    while (SC16IS750_I2C_readRegister(TXLVL) == 0) {
      // Wait for space in TX buffer
      wait_us(10);
    };
    SC16IS750_I2C_writeRegister(THR, str[idx]);
  }  
#endif  
}
/*==========================================================================================*/
/**
  * @brief  Write byte array to UART Bridge. Blocking when no free space in FIFO. 
  * @param  *data: byte array to be written    
  *   			len:   number of bytes to write 
  * @retval none.
  */
void SC16IS750_writeBytes(const char *data, int len) {
 
#if ENABLE_BULK_TRANSFERS
  int idx;
  
  // Write blocks of BULK_BLOCK_LEN  
  while (len > BULK_BLOCK_LEN) {
//     while(SC16IS750_I2C_readRegister(TXLVL) < BULK_BLOCK_LEN) {
//       // Wait for space in TX buffer
// //       wait_us(10);
//     };  
  
    // Write a block of BULK_BLOCK_LEN bytes
#if (0)    
    // Note: can be optimized by writing registeraddress once and then repeatedly write the bytes.
    for (idx=0; idx<BULK_BLOCK_LEN; idx++) {
      SC16IS750_I2C_writeRegister(THR, data[idx]);
    }
#else
    // optimized
    SC16IS750_I2C_writeDataBlock(data, BULK_BLOCK_LEN);    
#endif
              
    len  -= BULK_BLOCK_LEN;
    data += BULK_BLOCK_LEN;
  }
  
  // Write remaining bytes 
  // Note: can be optimized by writing registeraddress once and then repeatedly write the bytes.  
  for (idx=0; idx<len; idx++) {
    while (SC16IS750_I2C_readRegister(TXLVL) == 0) {
      // Wait for space in TX buffer
//       wait_us(10);
    };
    SC16IS750_I2C_writeRegister(THR, data[idx]);
  }
 
 
#else
  // Single writes instead of bulktransfer
  int idx;
  
  for (idx=0; idx<len; idx++) {
    while (SC16IS750_I2C_readRegister(TXLVL) == 0) {
      // Wait for space in TX buffer
      wait_us(10);
    };
    SC16IS750_I2C_writeRegister(THR, str[idx]);
  }  
#endif  
}
/*==========================================================================================*/
/**
  * @brief  Write byte array to UART Bridge. Blocking when no free space in FIFO. 
  * @param  *data: byte array to be written    
  *   			len:   number of bytes to write 
  * @retval none.
  */
int SC16IS750_readString(char *data, int len) {
	int idx;
	
	strcpy( data, "" );
	for (idx=0; idx<len-1; idx++) {
    while (SC16IS750_I2C_readRegister(RXLVL) == 0) {
			data[idx] = '\0';
      return idx;
    }
    data[idx] = SC16IS750_I2C_readRegister(RHR);
  }
	data[len] = '\0';
	
	return 0;
}
/*==========================================================================================*/
/**
  * @brief  Set direction of I/O port pins.
  * 				This method is specific to the SPI-I2C UART and not found on the 16750 
  * @param  bits: Bitpattern for I/O (1=output, 0=input) 
  * @retval none.
  */
void SC16IS750_ioSetDirection(unsigned char bits) {
  SC16IS750_I2C_writeRegister(IODIR, bits);
}
/*==========================================================================================*/
/**
  * @brief  Set bits of I/O port pins.
  * 				This method is specific to the SPI-I2C UART and not found on the 16750
  * @param  bits: Bitpattern for I/O (1= set output bit, 0 = clear output bit) 
  * @retval none.
  */
void SC16IS750_ioSetState(unsigned char bits) {
  SC16IS750_I2C_writeRegister(IOSTATE, bits);
}

/** Get bits of I/O port pins.
  * This method is specific to the SPI-I2C UART and not found on the 16750
  *   @param  none
  *   @return bits Bitpattern for I/O (1= bit set, 0 = bit cleared)
  */
/*==========================================================================================*/
/**
  * @brief  Get bits of I/O port pins.
  * 				This method is specific to the SPI-I2C UART and not found on the 16750
  * @param  none
  * @retval bits Bitpattern for I/O (1= bit set, 0 = bit cleared).
  */
unsigned char SC16IS750_ioGetState(void) {
  return SC16IS750_I2C_readRegister(IOSTATE) ;
}
/*==========================================================================================*/
/**
  * @brief  Software Reset SC16IS750 device.
  * 				This method is specific to the SPI-I2C UART and not found on the 16750
  * @param  none
  * @retval none
  */
void SC16IS750_swReset() {
  SC16IS750_I2C_writeRegister(IOCTRL, IOC_SW_RST);     
}

/*==========================================================================================*/
/*================================= function support	======================================*/
/*==========================================================================================*/
/** Write value to internal register.
  *   @param registerAddress   The address of the Register (enum RegisterName)
  *   @param data              The 8bit value to write
  *   @return none 
  */
void SC16IS750_I2C_writeRegister(RegisterName registerAddress, char data) {
	/* write value to internal register */
	I2C_Write(I2C1, SC16IS750_DEFAULT_ADDR, registerAddress, data);
}
/*==========================================================================================*/
/** Read value from internal register.
  *   @param registerAddress   The address of the Register (enum RegisterName)
  *   @return char             The 8bit value read from the register
  */
char SC16IS750_I2C_readRegister(RegisterName registerAddress) {
	char r[1];
	
	/*Read char from SC16IS750 register at <registerAddress>.*/
	r[0] = I2C_Read(I2C1, SC16IS750_DEFAULT_ADDR, registerAddress);
  return ( r[0] );
}
/*==========================================================================================*/
/** Write multiple datavalues to Transmitregister.
  * More Efficient implementation than writing individual bytes
  * Assume that previous check confirmed that the FIFO has sufficient free space to store the data
  * Pure virtual, must be declared in derived class.   
  *   @param char* databytes   The pointer to the block of data
  *   @param len               The number of bytes to write
  *   @return none 
  */
void SC16IS750_I2C_writeDataBlock (const char *data, int len) {
  
#if(0)  
  int i;
  char w[BULK_BLOCK_LEN];
  
  // Select the Transmit Holding Register
  // Assume that previous check confirmed that the FIFO has sufficient free space to store the data
//   w[0] = THR;
 
  // copy the data..
  for (i=0; i<len; i++)
    w[i] = data[i];
    
  I2C_WriteMulti( I2C1, SC16IS750_DEFAULT_ADDR, THR, w, len );
#else
  int i;
  
//   _i2c->start();
//   _i2c->write(_slaveAddress); 
 
  // Select the Transmit Holding Register
  // Assume that previous check confirmed that the FIFO has sufficient free space to store the data
//   _i2c->write(THR);
 
  // send the data..
//   for (i=0; i<len; i++)
//     _i2c->write(data[i]);
//     
//   _i2c->stop();  
#endif
}

/*********** Portions COPYRIGHT 2014 Intelenics. Co., Ltd.*****END OF FILE****/
