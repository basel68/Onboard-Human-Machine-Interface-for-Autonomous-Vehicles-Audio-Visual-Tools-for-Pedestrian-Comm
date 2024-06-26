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
	//reset and set the reset pin of the codec ,page 32
	HAL_GPIO_WritePin(CS43122_RST_Port, CS43122_RST_Pin, GPIO_PIN_RESET);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(CS43122_RST_Port, CS43122_RST_Pin, GPIO_PIN_SET);
	HAL_Delay(1000);
	//power off the codec, page 38
	i2c_data=POWEROFF;
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_POWER1_REG, BYTE, &i2c_data, BYTE, 100);

	// enable power of the headphones and speakers 0xaa,page 39
	i2c_data=HED_SPK_PWR_EN;
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_POWER2_REG, BYTE, &i2c_data, BYTE, 100);

	//if there is no error and i2c is not busy the status will be HAL_OK WHICH IS 0 SO if cond is skipped
	if(i2c_status){
		return i2c_status;
	}
	// enable autodetect CLK , page 41
	i2c_data=AUTO_CLK_EN;
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_CLKCON_REG, BYTE, &i2c_data, BYTE, 100);
	if(i2c_status){
			return i2c_status;
	}
	// i2s up to 24 bit interface ,16 bit data length, page 41
	i2c_data=_24BIT_16DATA;
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_INTERCON1_REG, BYTE, &i2c_data, BYTE, 100);
	if(i2c_status){
			return i2c_status;
	}
	// enable soft ramp and de-emphasis filter, page 45
	i2c_data=SOFT_DE_EMPH;
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_MISC0N_REG, BYTE, &i2c_data, BYTE, 100);
	if(i2c_status){
			return i2c_status;
	}
	// PCM Volume adjust , page 48
	i2c_data=PCM_VOL_EN;
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_PCMAVOLCON_REG, BYTE, &i2c_data, BYTE, 100);

	i2c_data=PCM_VOL_DI;
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_PCMBVOLCON_REG, BYTE, &i2c_data, BYTE, 100);
	if(i2c_status){
			return i2c_status;
	}
	i2c_data=0b01111111;
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_PASSVOLCON_REG, BYTE, &i2c_data, BYTE, 100);
	if(i2c_status){
			return i2c_status;
	}

	//DATASHEET INIT SEQUENCE
	// Step 1: Write 0x99 to register 0x00
	uint8_t data = 0x99;
	i2c_status = HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, 0x00, BYTE, &data, sizeof(data), HAL_MAX_DELAY);
	if(i2c_status != HAL_OK) {
	    // Handle error
	}

	// Step 2: Write 0x80 to register 0x47
	data = 0x80;
	i2c_status = HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, 0x47, BYTE, &data, sizeof(data), HAL_MAX_DELAY);
	if(i2c_status != HAL_OK) {
	    // Handle error
	}

	// Step 3: Write ‘1’b to bit 7 in register 0x32
	// First, read the current value of the register
	uint8_t regValue;
	i2c_status = HAL_I2C_Mem_Read(&CS43122_I2C, CS43122_I2C_ADDRESSR, 0x32, BYTE, &regValue, sizeof(regValue), HAL_MAX_DELAY);
	if(i2c_status != HAL_OK) {
	    // Handle error
	}

	// Then, set bit 7 to '1'
	regValue |= (1 << 7);
	i2c_status = HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, 0x32, BYTE, &regValue, sizeof(regValue), HAL_MAX_DELAY);
	if(i2c_status != HAL_OK) {
	    // Handle error
	}

	// Step 4: Write ‘0’b to bit 7 in register 0x32
	// Read the current value again in case it has changed
	i2c_status = HAL_I2C_Mem_Read(&CS43122_I2C, CS43122_I2C_ADDRESSR, 0x32, BYTE, &regValue, sizeof(regValue), HAL_MAX_DELAY);
	if(i2c_status != HAL_OK) {
	    // Handle error
	}

	// Then, clear bit 7 to '0'
	regValue &= ~(1 << 7);
	i2c_status = HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, 0x32, BYTE, &regValue, sizeof(regValue), HAL_MAX_DELAY);
	if(i2c_status != HAL_OK) {
	    // Handle error
	}

	// Step 5: Write 0x00 to register 0x00
	data = 0x00;
	i2c_status = HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, 0x00, BYTE, &data, sizeof(data), HAL_MAX_DELAY);
	if(i2c_status != HAL_OK) {
	    // Handle error
	}
	state=READY;
	return 0;

}




int Audio_mute(){
	HAL_StatusTypeDef i2c_status;
	uint8_t i2c_data;
	//headphone and speakers power off, page 38
	i2c_data=HED_SPK_PWR_DI;
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_POWER2_REG, BYTE, &i2c_data, BYTE, 100);

	//headphone and speakers Volume Controls A and B to 0, page 52
	i2c_data=HED_SPK_VOL_DOWN;
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_HEADVOLA_REG, BYTE, &i2c_data, BYTE, 100);
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_HEADVOLB_REG, BYTE, &i2c_data, BYTE, 100);
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_SPEAKERVOLA_REG, BYTE, &i2c_data, BYTE, 100);
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_SPEAKERVOLB_REG, BYTE, &i2c_data, BYTE, 100);
	//mute the headphones and speakers page 46
	i2c_data=HED_SPK_MUTE;
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_PLAYCON2_REG, BYTE, &i2c_data, BYTE, 100);
	if(i2c_status){
		return i2c_status;
	}
	return i2c_status;

}
int Audio_play(uint16_t *buffer,uint32_t size){
	HAL_StatusTypeDef i2c_status;
	uint8_t i2c_data;

		//power on the codec, page 38
		i2c_data=POWERUP;
		i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_POWER1_REG, BYTE, &i2c_data, BYTE, 100);
		if(i2c_status){
			return i2c_status;
		}

		state=PLAY;
		//transfer audio to codec
		HAL_I2S_Transmit_DMA(&CS43122_I2S,buffer,size);

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
		i2c_data=POWEROFF;
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
		i2c_data=POWEROFF;
		i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_POWER1_REG, BYTE, &i2c_data, BYTE, 100);
		if(i2c_status){
			return i2c_status;
		}
		state=PAUSE;
		HAL_Delay(100);
//		for(int i=0;i<200;i++);
		HAL_I2S_DMAStop(&CS43122_I2S);


	}
	return 0;
}
int Audio_resume(){
	HAL_StatusTypeDef i2c_status;
	uint8_t i2c_data;

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

	return 0;
}
int Audio_unmute(){
	HAL_StatusTypeDef i2c_status;
	uint8_t i2c_data;

	//headphone and speakers Volume Controls A and B UP, page 52
	i2c_data=HED_SPK_VOL_UP;
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_HEADVOLA_REG, BYTE, &i2c_data, BYTE, 100);
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_HEADVOLB_REG, BYTE, &i2c_data, BYTE, 100);
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_SPEAKERVOLA_REG, BYTE, &i2c_data, BYTE, 100);
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_SPEAKERVOLB_REG, BYTE, &i2c_data, BYTE, 100);

	//headphone and speakers on, page 38
	i2c_data=HED_SPK_PWR_EN;
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_POWER2_REG, BYTE, &i2c_data, BYTE, 100);
	//unmute the headphones and speakers page 46
	i2c_data=HED_SPK_UNMUTE;
	i2c_status=HAL_I2C_Mem_Write(&CS43122_I2C, CS43122_I2C_ADDRESS, CS43122_PLAYCON2_REG, BYTE, &i2c_data, BYTE, 100);
	if(i2c_status){
		return i2c_status;
	}
	return i2c_status;
}


