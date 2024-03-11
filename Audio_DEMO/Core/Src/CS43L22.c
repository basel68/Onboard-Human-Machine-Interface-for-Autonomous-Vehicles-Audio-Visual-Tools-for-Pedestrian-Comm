/*
 * CS43L22.c
 *
 *  Created on: Mar 11, 2024
 *      Author: BASSEL
 *      Source file for the 24 bit DAC known as Audio codec
 */

#include "CS43L22.h"
codec_state state;

int Audio_init(){
	HAL_StatusTypeDef i2c_status=0;
	uint8_t i2c_data;
	//Power up the DAC,page 32
	HAL_GPIO_WritePin(CS43122_RST_Port, CS43122_RST_Pin, GPIO_PIN_RESET);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(CS43122_RST_Port, CS43122_RST_Pin, GPIO_PIN_SET);
	HAL_Delay(1000);
//	uint8_t pin=HAL_GPIO_ReadPin(CS43122_RST_Port, CS43122_RST_Pin);

	//power off the codec, page 38
	i2c_data=0x01;
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_POWER1_REG, BYTE, &i2c_data, BYTE, 100);

	// enable the headphones 0xaf,page 39
	i2c_data=0b10101111;
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_POWER2_REG, BYTE, &i2c_data, BYTE, 100);

	//if there is no error and i2c is not busy the status will be HAL_OK WHICH IS 0 SO if cond is skipped
	if(i2c_status){
		return i2c_status;
	}
	// enable autodetect CLK and MCLK/2, page 41
	i2c_data=0b10000001;
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_CLKCON_REG, BYTE, &i2c_data, BYTE, 100);
	if(i2c_status){
			return i2c_status;
	}
	// i2s up to 24 bit interface ,16 bit data length, page 41
	i2c_data=0b00000111;
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_INTERCON1_REG, BYTE, &i2c_data, BYTE, 100);
	if(i2c_status){
			return i2c_status;
	}
	// enable soft ramp and de-emphasis filter, page 45
	i2c_data=0b00000110;
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_MISC0N_REG, BYTE, &i2c_data, BYTE, 100);
	if(i2c_status){
			return i2c_status;
	}
	// PCM Volume adjust , page 48
	i2c_data=0x00;
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_PCMAVOLCON_REG, BYTE, &i2c_data, BYTE, 100);
	i2c_data=0x00;
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_PCMBVOLCON_REG, BYTE, &i2c_data, BYTE, 100);
	if(i2c_status){
			return i2c_status;
	}
	state=READY;
	return 0;

}




int Audio_mute(){
	HAL_StatusTypeDef i2c_status;
	uint8_t i2c_data;
	//headphone off, page 38
	i2c_data=0xff;
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_POWER2_REG, BYTE, &i2c_data, BYTE, 100);

	//mute headphone A and B, page 52
	i2c_data=0x01;
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_HEADVOLA_REG, BYTE, &i2c_data, BYTE, 100);
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_HEADVOLB_REG, BYTE, &i2c_data, BYTE, 100);

	return i2c_status;

}
int Audio_play(uint16_t *buffer,uint32_t size){
	HAL_StatusTypeDef i2c_status;
	uint8_t i2c_data;
    if(state!=PLAY){
		i2c_status=Audio_unmute();
		if(i2c_status){
			return i2c_status;
		}
		//power on the codec, page 38
		i2c_data=POWERUP;
		i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_POWER1_REG, BYTE, &i2c_data, BYTE, 100);
		if(i2c_status){
			return i2c_status;
		}

		state=PLAY;
		//transfer audio to codec
		HAL_I2S_Transmit_DMA(&CS43122_I2S,buffer,size);
    }
    return 0;

}
int Audio_pause(){
	HAL_StatusTypeDef i2c_status;
	uint8_t i2c_data;
	if(state==PLAY || state==RESUME){// we only pause if we are in PLAY or RESUME states
		//mute
		i2c_status=Audio_mute();
		if(i2c_status){
			return i2c_status;
		}
		//power off the codec, page 38
		i2c_data=0x01;
		i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_POWER1_REG, BYTE, &i2c_data, BYTE, 100);
		if(i2c_status){
			return i2c_status;
		}
		state=PAUSE;
		for(int i=0;i<200;i++);
		HAL_I2S_DMAPause(&CS43122_I2S);

	}
	return 0;
}
int Audio_stop(){
	HAL_StatusTypeDef i2c_status;
	uint8_t i2c_data;
	if(state==PLAY || state==RESUME){// we only pause if we are in PLAY or RESUME states
		//mute
		i2c_status=Audio_mute();
		if(i2c_status){
			return i2c_status;
		}
		//power off the codec, page 38
		i2c_data=0x01;
		i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_POWER1_REG, BYTE, &i2c_data, BYTE, 100);
		if(i2c_status){
			return i2c_status;
		}
		state=PAUSE;
		for(int i=0;i<200;i++);
		HAL_I2S_DMAStop(&CS43122_I2S);

	}
	return 0;
}
int Audio_resume(){
	HAL_StatusTypeDef i2c_status;
	uint8_t i2c_data;
	if(state==PAUSE){
		//unmute
		i2c_status=Audio_unmute();
		if(i2c_status){
			return i2c_status;
		}
		//power on the codec, page 38
		i2c_data=POWERUP;
		i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_POWER1_REG, BYTE, &i2c_data, BYTE, 100);
		if(i2c_status){
			return i2c_status;
		}
		state=RESUME;
		for(int i=0;i<200;i++);
		HAL_I2S_DMAResume(&CS43122_I2S);
	}
	return 0;
}
int Audio_unmute(){
	HAL_StatusTypeDef i2c_status;
	uint8_t i2c_data;

	//mute headphone A and B, page 52
	i2c_data=0x00;
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_HEADVOLA_REG, BYTE, &i2c_data, BYTE, 100);
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_HEADVOLB_REG, BYTE, &i2c_data, BYTE, 100);

	//headphone on, page 38
	i2c_data=0xaf;
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_POWER2_REG, BYTE, &i2c_data, BYTE, 100);
	return i2c_status;
}
void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s){
	if(hi2s== &CS43122_I2S){
		i2s_cplt_callback();
	}
}
__weak void i2s_cplt_callback(void){
	;
}
