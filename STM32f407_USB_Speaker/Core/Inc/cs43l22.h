#ifndef INC_CS43L22_H_
#define INC_CS43L22_H_

#include "main.h"

// I2C address
#define CS43L22_I2C_ADDRESS             0b10010100


// I2C1 and I2S3 handlers
extern I2C_HandleTypeDef hi2c1;
extern I2S_HandleTypeDef hi2s3;

#define CS43L22_I2C             hi2c1
#define CS43L22_I2S             hi2s3


//#define CS43L22_RST_Pin   CS43L22_RESET_Pin
//#define CS43L22_RST_Port  CS43L22_RESET_GPIO_Port
/*
 *
 * Register's addresses
 */
#define CS43l22_ID_REG          		0x01
#define CS43l22_POWCON1_REG       		0x02
#define CS43l22_POWCON2_REG       		0x04
#define CS43l22_CLKCON_REG          	0x05
#define CS43l22_INTERCON1_REG           0x06
#define CS43l22_INTERCON2_REG           0x07
#define CS43l22_PASSA_REG               0x08
#define CS43l22_PASSB_REG               0x09
#define CS43l22_ZC_SR_REG               0x0a
#define CS43l22_GANGCON_REG             0x0c
#define CS43l22_PLAYCON1_REG            0x0d
#define CS43l22_MISCON_REG              0x0e
#define CS43l22_PLAYCON2_REG            0x0f
#define CS43l22_PASSVOLACON_REG         0x14
#define CS43l22_PASSVOLBCON_REG         0x15
#define CS43l22_PCMAVOLCON_REG          0x1a
#define CS43l22_PCMBVOLCON_REG          0x1b
#define CS43l22_BEEPFREQDUR_REG         0x1c
#define CS43l22_BEEPVOL_REG	            0x1d
#define CS43l22_BEEPTONE_REG	        0x1e
#define CS43l22_TONECON_REG 	        0x1f
#define CS43l22_MASAVOL_REG 	        0x20
#define CS43l22_MASBVOL_REG 	        0x21
#define CS43l22_HEADAVOL_REG 	        0x22
#define CS43l22_HEADBVOL_REG 	        0x23
#define CS43l22_SPKAVOL_REG 	        0x24
#define CS43l22_SPKBVOL_REG 	        0x25
#define CS43l22_PCMCHSW_REG 	        0x26
#define CS43l22_LIMCON1_REG 	        0x27
#define CS43l22_LIMCON2_REG 	        0x28
#define CS43l22_LIMATTACK_REG 	        0x29
#define CS43l22_STATUS_REG 	            0x2e
#define CS43l22_BATCOMP_REG 	        0x2f
#define CS43l22_VPBATLEVEL_REG 	        0x30
#define CS43l22_SPKSTATUS_REG 	        0x31
#define CS43l22_CHARPUMPFREQREG         0x34

int cs43l22_init();
void cs43l22_deinit();
void cs43l22_start();
int cs43l22_play(int16_t *pbuffer, uint32_t size);
int cs43l22_pause();
int cs43l22_resume();
int cs43l22_stop();
int cs43l22_set_volume(uint8_t volume);
void cs43l22_read_reg(uint8_t reg, uint8_t *reg_value);
int cs43l22_unmute();
int cs43l22_mute();

void AUDIO_I2S_TxCpltCallback(void);
void AUDIO_I2S_TxHalfCpltCallback(void);
typedef enum
{
	PLAY,
	STOP,
	RESUME,
	PAUSE,
	NOTREADY,
	READY
} codec_state;

codec_state get_cs43l22_state();
#endif /* INC_CS43L22_H_ */
