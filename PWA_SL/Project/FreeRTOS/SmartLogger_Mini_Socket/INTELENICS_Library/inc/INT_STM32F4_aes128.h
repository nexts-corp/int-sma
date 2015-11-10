/**
  ******************************************************************************
  * <h2><center>&copy;http://www.element14.com/community/docs/DOC-17134/l/aes128-a-c-implementation-for-encryption-and-decryption-source-code</center></h2>
  * @file    	INT_STM32F4_aes128.h
  * @author  	Uli Kretzschmar
  * @version 	V1.0.0
  * @date    	18-July-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
#ifndef _INT_STM32F4_AES_H
#define _INT_STM32F4_AES_H

#include <stdint.h>

extern const uint8_t _Private_Key[];

void vAES_encrypt(unsigned char *state, unsigned char *key);
void vAES_decrypt(unsigned char *state, unsigned char *key);

#endif /*_INT_STM32F4_AES_H*/



