/**
  ******************************************************************************
  * @file    INT_STM32F4_i2c.h
  * @author  Intelenics Application Team
  ******************************************************************************
**/

#ifndef INT_STM32F4_I2C_H
#define INT_STM32F4_I2C_H 110

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_i2c.h"

/* Exported types ------------------------------------------------------------*/
typedef enum {
	I2C_PinsPack_1,
	I2C_PinsPack_2,
	I2C_PinsPack_3
} I2C_PinsPack_t;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
// Timeout
#ifndef I2C_TIMEOUT
#define I2C_TIMEOUT					20000
#endif

//I2C1 settings, change them in defines.h project file
#ifndef I2C1_ACKNOWLEDGED_ADDRESS
#define I2C1_ACKNOWLEDGED_ADDRESS	I2C_AcknowledgedAddress_7bit
#endif
#ifndef I2C1_MODE
#define I2C1_MODE					I2C_Mode_I2C
#endif
#ifndef I2C1_OWN_ADDRESS
#define I2C1_OWN_ADDRESS				0x00
#endif
#ifndef I2C1_ACK
#define I2C1_ACK						I2C_Ack_Disable
#endif
#ifndef I2C1_DUTY_CYCLE
#define I2C1_DUTY_CYCLE				I2C_DutyCycle_2
#endif

//I2C2 settings, change them in defines.h project file
#ifndef I2C2_ACKNOWLEDGED_ADDRESS
#define I2C2_ACKNOWLEDGED_ADDRESS	I2C_AcknowledgedAddress_7bit
#endif
#ifndef I2C2_MODE
#define I2C2_MODE					I2C_Mode_I2C
#endif
#ifndef I2C2_OWN_ADDRESS
#define I2C2_OWN_ADDRESS				0x00
#endif
#ifndef I2C2_ACK
#define I2C2_ACK						I2C_Ack_Disable
#endif
#ifndef I2C2_DUTY_CYCLE
#define I2C2_DUTY_CYCLE				I2C_DutyCycle_2
#endif

//I2C3 settings, change them in defines.h project file
#ifndef I2C3_ACKNOWLEDGED_ADDRESS
#define I2C3_ACKNOWLEDGED_ADDRESS	I2C_AcknowledgedAddress_7bit
#endif
#ifndef I2C3_MODE
#define I2C3_MODE					I2C_Mode_I2C
#endif
#ifndef I2C3_OWN_ADDRESS
#define I2C3_OWN_ADDRESS				0x00
#endif
#ifndef I2C3_ACK
#define I2C3_ACK						I2C_Ack_Disable
#endif
#ifndef I2C3_DUTY_CYCLE
#define I2C3_DUTY_CYCLE				I2C_DutyCycle_2
#endif

//I2C speed modes
#define I2C_CLOCK_STANDARD			100000
#define I2C_CLOCK_FAST_MODE			400000
#define I2C_CLOCK_FAST_MODE_PLUS		1000000
#define I2C_CLOCK_HIGH_SPEED			3400000

/* Exported functions ------------------------------------------------------- */ 
/* Initialize I2C */
void vI2C_Initial(I2C_TypeDef* I2Cx, I2C_PinsPack_t pinspack, uint32_t clockSpeed);

#ifdef I2C1

/* Initialize I2C1 */
void I2C1_InitPins(I2C_PinsPack_t pinspack);
#endif

/* Read single byte from slave */
uint8_t I2C_Read(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg);

/* Write single byte from slave */
void I2C_Write(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t data);

/* Read multi bytes from slave */
void I2C_ReadMulti(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t *data, uint16_t count);

/* Write multi bytes from slave */
void I2C_WriteMulti(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t *data, uint16_t count);

/* Checks if device is connected to I2C bus */
uint8_t I2C_IsDeviceConnected(I2C_TypeDef* I2Cx, uint8_t address);

/* I2C Start condition */
int16_t I2C_Start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction, uint8_t ack);

/* Stop condition */
uint8_t I2C_Stop(I2C_TypeDef* I2Cx);

/* Read byte without ack */
uint8_t I2C_ReadNack(I2C_TypeDef* I2Cx);

/* Read byte with ack  */
extern uint8_t I2C_ReadAck(I2C_TypeDef* I2Cx);

/* Write to slave */
void I2C_WriteData(I2C_TypeDef* I2Cx, uint8_t data);

#endif

/*********** Portions COPYRIGHT 2014 Intelenics. Co., Ltd.*****END OF FILE****/
