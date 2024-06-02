#include "cs43l22.h"
#include "stdio.h"

static codec_state cs43l22_state = NOTREADY;

int cs43l22_mute()
{
	uint8_t i2c_data;
	HAL_StatusTypeDef status_i2c = 0;

	// headphone channel is off, page 38
	i2c_data = 0xff;
	status_i2c = HAL_I2C_Mem_Write(&CS43L22_I2C, CS43L22_I2C_ADDRESS, CS43l22_POWCON2_REG,
					                                    1, &i2c_data, 1, 100);

	// mute headphone A and B, page 52
	i2c_data = 0x01;
	status_i2c = HAL_I2C_Mem_Write(&CS43L22_I2C, CS43L22_I2C_ADDRESS, CS43l22_HEADAVOL_REG,
		                                    1, &i2c_data, 1, 100);
	status_i2c = HAL_I2C_Mem_Write(&CS43L22_I2C, CS43L22_I2C_ADDRESS, CS43l22_HEADBVOL_REG,
			                                    1, &i2c_data, 1, 100);
	return status_i2c;
}

int cs43l22_unmute()
{
	uint8_t i2c_data;
	HAL_StatusTypeDef status_i2c = 0;
	// unmute headphone A and B, page 52
	i2c_data = 0x00;
	status_i2c = HAL_I2C_Mem_Write(&CS43L22_I2C, CS43L22_I2C_ADDRESS, CS43l22_HEADAVOL_REG,
		                                    1, &i2c_data, 1, 100);
	status_i2c = HAL_I2C_Mem_Write(&CS43L22_I2C, CS43L22_I2C_ADDRESS, CS43l22_HEADBVOL_REG,
			                                    1, &i2c_data, 1, 100);
	// turn on headphone
	i2c_data = 0xaf;
	status_i2c = HAL_I2C_Mem_Write(&CS43L22_I2C, CS43L22_I2C_ADDRESS, CS43l22_POWCON2_REG,
				                                    1, &i2c_data, 1, 100);

	return status_i2c;
}
codec_state get_cs43l22_state()
{
	return cs43l22_state;
}
int cs43l22_init()
{
	HAL_StatusTypeDef status_i2c = 0;
	uint8_t i2c_data;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4, GPIO_PIN_SET);
	HAL_Delay(100);
	// power off the codec, page 38
	i2c_data = 0x01;
	status_i2c = HAL_I2C_Mem_Write(&CS43L22_I2C, CS43L22_I2C_ADDRESS, CS43l22_POWCON1_REG,
	                                    1, &i2c_data, 1, 100);

	// output device is headphone 0xaf, page 38
	i2c_data = 0b10101111;
	status_i2c = HAL_I2C_Mem_Write(&CS43L22_I2C, CS43L22_I2C_ADDRESS, CS43l22_POWCON2_REG,
										1, &i2c_data, 1, 100);
	if (status_i2c)
	{
		return status_i2c;
	}

	// clock auto-detect and divide mclk by 2
	i2c_data = 0b10000001;
	status_i2c = HAL_I2C_Mem_Write(&CS43L22_I2C, CS43L22_I2C_ADDRESS, CS43l22_CLKCON_REG,
										1, &i2c_data, 1, 100);
	if (status_i2c)
	{
		return status_i2c;
	}

	cs43l22_set_volume(200);

	// i2s up to 24-bit interface, 16-bit data length, page 41
	i2c_data = 0b00000111;
	status_i2c = HAL_I2C_Mem_Write(&CS43L22_I2C, CS43L22_I2C_ADDRESS, CS43l22_INTERCON1_REG,
										1, &i2c_data, 1, 100);
	if (status_i2c)
	{
		return status_i2c;
	}

	// enable digital soft ramp and de-emphasis filter
	i2c_data = 0b00000110;
	status_i2c = HAL_I2C_Mem_Write(&CS43L22_I2C, CS43L22_I2C_ADDRESS, CS43l22_MISCON_REG,
											1, &i2c_data, 1, 100);
	if (status_i2c)
	{
		return status_i2c;
	}

	//  PCM VOlume adjust, page 47
	i2c_data = 0x00;
	status_i2c = HAL_I2C_Mem_Write(&CS43L22_I2C, CS43L22_I2C_ADDRESS, CS43l22_PCMAVOLCON_REG,
										1, &i2c_data, 1, 100);
	if (status_i2c)
	{
		return status_i2c;
	}

	i2c_data = 0x00;
	status_i2c = HAL_I2C_Mem_Write(&CS43L22_I2C, CS43L22_I2C_ADDRESS, CS43l22_PCMBVOLCON_REG,
										1, &i2c_data, 1, 100);
	if (status_i2c)
	{
		return status_i2c;
	}

	cs43l22_state = READY;
	return 0;

}

int cs43l22_play(int16_t *pbuffer, uint32_t size)
{
	HAL_StatusTypeDef status_i2c = 0;
	uint8_t i2c_data;
	if (cs43l22_state != PLAY)
	{
		status_i2c = cs43l22_unmute();
		if (status_i2c)
		{
			return status_i2c;
		}
		// powered up, page 38
		i2c_data = 0b10011110;
		status_i2c = HAL_I2C_Mem_Write(&CS43L22_I2C, CS43L22_I2C_ADDRESS, CS43l22_POWCON1_REG,
												1, &i2c_data, 1, 100);
		if (status_i2c)
		{
			return status_i2c;
		}
		cs43l22_state = PLAY;
		HAL_I2S_Transmit_DMA(&CS43L22_I2S, (uint16_t * )pbuffer, size);
	}
	return 0;
}
int cs43l22_pause()
{
	HAL_StatusTypeDef status_i2c = 0;
	uint8_t i2c_data;

	if (cs43l22_state == PLAY || cs43l22_state == RESUME)
	{
		status_i2c = cs43l22_mute();
		if (status_i2c)
		{
			return status_i2c;
		}

		// powered off, page 38
		i2c_data = 0x01;
		status_i2c = HAL_I2C_Mem_Write(&CS43L22_I2C, CS43L22_I2C_ADDRESS, CS43l22_POWCON1_REG,
												1, &i2c_data, 1, 100);
		if (status_i2c)
		{
			return status_i2c;
		}
		cs43l22_state = PAUSE;
		HAL_I2S_DMAPause(&CS43L22_I2S);
	}
	return 0;
}

int cs43l22_resume()
{
	HAL_StatusTypeDef status_i2c = 0;
	uint8_t i2c_data;
	if (cs43l22_state == PAUSE)
	{
		status_i2c = cs43l22_unmute();
		if (status_i2c)
		{
			return status_i2c;
		}
		// powered up, page 38
		i2c_data = 0b10011110;
		status_i2c = HAL_I2C_Mem_Write(&CS43L22_I2C, CS43L22_I2C_ADDRESS, CS43l22_POWCON1_REG,
												1, &i2c_data, 1, 100);
		if (status_i2c)
		{
			return status_i2c;
		}
		cs43l22_state = RESUME;
		HAL_I2S_DMAResume(&CS43L22_I2S);
	}
	return 0;
}

int cs43l22_stop()
{
	HAL_StatusTypeDef status_i2c = 0;
	uint8_t i2c_data;
	if (cs43l22_state != STOP)
	{
		status_i2c = cs43l22_mute();
		if (status_i2c)
		{
			return status_i2c;
		}
		// powered off, page 38
		i2c_data = 0x9f;
		status_i2c = HAL_I2C_Mem_Write(&CS43L22_I2C, CS43L22_I2C_ADDRESS, CS43l22_POWCON1_REG,
												1, &i2c_data, 1, 100);
		if (status_i2c)
		{
			return status_i2c;
		}
		cs43l22_state = STOP;
		HAL_I2S_DMAStop(&CS43L22_I2S);
	}
	return 0;
}

void cs43l22_read_reg(uint8_t reg, uint8_t *reg_value)
{
	HAL_I2C_Mem_Read(&CS43L22_I2C, CS43L22_I2C_ADDRESS, reg,
			1, reg_value, 1, 100);
}

int cs43l22_set_volume(uint8_t volume)
{
	HAL_StatusTypeDef status_i2c = 0;
	// page 52
	if (volume > 0xe6)
	{
		volume -= 0xe7;
	}
	else
	{
		volume += 0x19;
	}
	status_i2c = HAL_I2C_Mem_Write(&CS43L22_I2C, CS43L22_I2C_ADDRESS, CS43l22_MASAVOL_REG,
											1, &volume, 1, 100);
	status_i2c = HAL_I2C_Mem_Write(&CS43L22_I2C, CS43L22_I2C_ADDRESS, CS43l22_MASBVOL_REG,
												1, &volume, 1, 100);
	return status_i2c;

}

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{
  if(hi2s == &CS43L22_I2S)
  {
	AUDIO_I2S_TxCpltCallback();
  }
}

void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
  if(hi2s == &CS43L22_I2S)
  {

    AUDIO_I2S_TxHalfCpltCallback();
  }
}
__weak void AUDIO_I2S_TxCpltCallback(void)
{
}
__weak void AUDIO_I2S_TxHalfCpltCallback(void)
{

}

void cs43l22_set_frequency(uint32_t frequency);
void cs43l22_read_id();
