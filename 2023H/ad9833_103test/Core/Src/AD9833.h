#ifndef _AD9833_H
#define _AD9833_H

#include "gpio.h"
#define TRI_WAVE 	0  		//������ǲ�
#define SIN_WAVE 	1		//������Ҳ�
#define SQU_WAVE 	2		//�������


//void AD9833_WaveSeting(double frequence,unsigned int frequence_SFR,unsigned int WaveMode,unsigned int Phase );

//void AD9833_Init_GPIO(void);
//void AD9833_AmpSet(unsigned char amp);

void AD9833_WaveSeting(double frequence,unsigned int frequence_SFR,unsigned int WaveMode,unsigned int Phase, unsigned char channel );
void AD9833_WaveSeting_Double(double Freq0, double Freq1, unsigned int Freq_SFR, unsigned int WaveMode0, unsigned int WaveMode1, unsigned int Phase0, unsigned int Phase1);

#endif



