/*
 * CS43L22.h
 *
 *  Created on: Mar 11, 2024
 *      Author: BASSEL
 *      Header file for the 24 bit DAC known as Audio codec
 */

#ifndef INC_CS43L22_H_
#define INC_CS43L22_H_
#include "main.h"
extern I2C_HandleTypeDef hi2c1;
extern I2S_HandleTypeDef hi2s3;
#define CS43122_I2C                hi2c1
#define CS43122_I2S                hi2s3

#define CS43122_I2C_ADDRESS         0x94
#define CS43122_I2C_ADDRESSR        0x95
#define BYTE 1
#define CS43122_RST_Pin            Audio_RST_Pin
#define CS43122_RST_Port           Audio_RST_GPIO_Port

#define POWERUP 					0b10011110

#define CS43122_ID_REG              0x01
#define CS43122_POWER1_REG          0x02
#define CS43122_POWER2_REG          0x04
#define CS43122_CLKCON_REG          0x05
#define CS43122_INTERCON1_REG       0x06
#define CS43122_INTERCON2_REG       0x07
#define CS43122_PASSA_REG           0x08
#define CS43122_PASSB_REG           0x09
#define CS43122_GANGCON_REG         0x0c
#define CS43122_PLAYCON1_REG        0x0d
#define CS43122_PLAYCON2_REG        0x0f
#define CS43122_MISC0N_REG          0x0e
#define CS43122_PASSVOLCON_REG      0x14
#define CS43122_PASSVOLBCON_REG     0x15
#define CS43122_PCMAVOLCON_REG      0x1a
#define CS43122_PCMBVOLCON_REG      0x1b
#define CS43122_HEADVOLA_REG         0x22
#define CS43122_HEADVOLB_REG        0x23

// Enumeration for codec states
typedef enum
{
    PLAY,
    STOP,
    RESUME,
    PAUSE,
    NOT_READY,
    READY
} codec_state;

int Audio_init();
int Audio_unmute();
int Audio_resume();
int Audio_stop();
int Audio_pause();
int Audio_play(uint16_t *buffer,uint32_t size);
int Audio_mute();
__weak void i2s_cplt_callback(void);



#endif /* INC_CS43L22_H_ */
