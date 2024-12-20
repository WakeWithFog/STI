
#include "ad9833.h"

//***************************
//		Pin assign
//		STM32			AD9833
//		PF12 		---> FSYNC
//		PF1 		---> SCK
//		PG6 		---> DAT
//		PC9			---> CS
//***************************

/*端口定义 */
//	#define PORT_FSYNC	GPIOB
//	#define PIN_FSYNC	GPIO_Pin_15  //PC7

//	#define PORT_SCK	GPIOB
//	#define PIN_SCK		GPIO_Pin_14  //PC8

//	#define PORT_DAT	GPIOB
//	#define PIN_DAT		GPIO_Pin_13  //PC9

//	#define PORT_CS		GPIOB
//	#define PIN_CS		GPIO_Pin_12  //数字电位器片选//PA8

//****************************************************************

#define FSY0_0() HAL_GPIO_WritePin(DDS_FSY0_GPIO_Port, DDS_FSY0_Pin, GPIO_PIN_RESET)
#define FSY0_1() HAL_GPIO_WritePin(DDS_FSY0_GPIO_Port, DDS_FSY0_Pin, GPIO_PIN_SET)

#define SCK_0() HAL_GPIO_WritePin(DDS_CLK_GPIO_Port, DDS_CLK_Pin, GPIO_PIN_RESET)
#define SCK_1() HAL_GPIO_WritePin(DDS_CLK_GPIO_Port, DDS_CLK_Pin, GPIO_PIN_SET)

#define DAT_0() HAL_GPIO_WritePin(DDS_DAT_GPIO_Port, DDS_DAT_Pin, GPIO_PIN_RESET)
#define DAT_1() HAL_GPIO_WritePin(DDS_DAT_GPIO_Port, DDS_DAT_Pin, GPIO_PIN_SET)

#define FSY1_0() HAL_GPIO_WritePin(DDS_FSY1_GPIO_Port, DDS_FSY1_Pin, GPIO_PIN_RESET)
#define FSY1_1() HAL_GPIO_WritePin(DDS_FSY1_GPIO_Port, DDS_FSY1_Pin, GPIO_PIN_SET)
// 初始化AD9833 GPIO

// void AD9833_Init_GPIO()
//{
//     GPIO_InitTypeDef GPIO_InitStructure;

//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

//	GPIO_InitStructure.GPIO_Pin = PIN_FSYNC|PIN_SCK|PIN_DAT|PIN_CS;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_Init(PORT_SCK, &GPIO_InitStructure);

//}

/*
*********************************************************************************************************
*	函 数 名: AD9833_Delay
*	功能说明: 时钟延时
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void AD9833_Delay(void)
{
	uint16_t i;
	for (i = 0; i < 1; i++)
		;
}

/*
*********************************************************************************************************
*	函 数 名: AD9833_Write
*	功能说明: 向SPI总线发送16个bit数据
*	形    参: TxData : 数据
*	返 回 值: 无
*********************************************************************************************************
*/
void AD9833_Write(unsigned int TxData, unsigned char channel)
{
	unsigned char i;

	SCK_1();
	// AD9833_Delay();
	if (channel)
	{			  // 通道1
		FSY1_1(); // FSY拉高
		// AD9833_Delay();
		FSY1_0(); // FSY拉低
		// AD9833_Delay();
	}
	else
	{ // 通道0
		FSY0_1();
		// AD9833_Delay();
		FSY0_0();
		// AD9833_Delay();
	}
	for (i = 0; i < 16; i++)
	{
		if (TxData & 0x8000) //
			DAT_1();
		else
			DAT_0();

		AD9833_Delay();
		SCK_0();
		AD9833_Delay();
		SCK_1();

		TxData <<= 1; // 无符号数左移1位后赋值
	}
	FSY0_1(); // 写得有问题，没考虑FSY1的情况
}

/*
*********************************************************************************************************
*	函 数 名: AD9833_AmpSet
*	功能说明: 改变输出信号幅度值
*	形    参: 1.amp ：幅度值  0- 255
*	返 回 值: 无
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*	函 数 名: AD9833_WaveSeting
*	功能说明: 向SPI总线发送16个bit数据
*	形    参: 1.Freq: 频率值, 0.1 hz - 12Mhz
			  2.Freq_SFR: 0 或 1
			  3.WaveMode: TRI_WAVE(三角波),SIN_WAVE(正弦波),SQU_WAVE(方波)
			  4.Phase : 波形的初相位
*	返 回 值: 无
*********************************************************************************************************
*/
void AD9833_WaveSeting(double Freq, unsigned int Freq_SFR, unsigned int WaveMode, unsigned int Phase, unsigned char channel)
{

	int frequence_LSB, frequence_MSB, Phs_data;
	double frequence_mid, frequence_DATA;
	long int frequence_hex;

	/*********************************计算频率的16进制值***********************************/
	frequence_mid = 268435456 / 25; // 适合25M晶振
	// 如果时钟频率不为25MHZ，修改该处的频率值，单位MHz ，AD9833最大支持25MHz
	frequence_DATA = Freq;
	frequence_DATA = frequence_DATA / 1000000;
	frequence_DATA = frequence_DATA * frequence_mid;
	frequence_hex = frequence_DATA;			// 这个frequence_hex的值是32位的一个很大的数字，需要拆分成两个14位进行处理；
	frequence_LSB = frequence_hex;			// frequence_hex低16位送给frequence_LSB
	frequence_LSB = frequence_LSB & 0x3fff; // 去除最高两位，16位数换去掉高位后变成了14位
	frequence_MSB = frequence_hex >> 14;	// frequence_hex高16位送给frequence_HSB
	frequence_MSB = frequence_MSB & 0x3fff; // 去除最高两位，16位数换去掉高位后变成了14位

	Phs_data = Phase | 0xC000;	   // 相位值
	AD9833_Write(0x0100, channel); // 复位AD9833,即RESET位为1
	AD9833_Write(0x2100, channel); // 选择数据一次写入，B28位和RESET位为1

	if (Freq_SFR == 0) // 把数据设置到设置频率寄存器0
	{
		frequence_LSB = frequence_LSB | 0x4000;
		frequence_MSB = frequence_MSB | 0x4000;
		// 使用频率寄存器0输出波形
		AD9833_Write(frequence_LSB, channel); // L14，选择频率寄存器0的低14位数据输入
		AD9833_Write(frequence_MSB, channel); // H14 频率寄存器的高14位数据输入
		AD9833_Write(Phs_data, channel);	  // 设置相位
		// AD9833_Write(0x2000); /**设置FSELECT位为0，芯片进入工作状态,频率寄存器0输出波形**/
	}
	if (Freq_SFR == 1) // 把数据设置到设置频率寄存器1
	{
		frequence_LSB = frequence_LSB | 0x8000;
		frequence_MSB = frequence_MSB | 0x8000;
		// 使用频率寄存器1输出波形
		AD9833_Write(frequence_LSB, channel); // L14，选择频率寄存器1的低14位输入
		AD9833_Write(frequence_MSB, channel); // H14 频率寄存器1为
		AD9833_Write(Phs_data, channel);	  // 设置相位
		// AD9833_Write(0x2800); /**设置FSELECT位为0，设置FSELECT位为1，即使用频率寄存器1的值，芯片进入工作状态,频率寄存器1输出波形**/
	}

	if (WaveMode == TRI_WAVE) // 输出三角波波形
		AD9833_Write(0x2002, channel);
	if (WaveMode == SQU_WAVE) // 输出方波波形
		AD9833_Write(0x2028, channel);
	if (WaveMode == SIN_WAVE) // 输出正弦波形
		AD9833_Write(0x2000, channel);
}

/*
*********************************************************************************************************
*	函 数 名: AD9833_WaveSeting_Double
*	功能说明: 向SPI总线发送16个bit数据
*	形    参: 1.Freq0: channel0频率值, 0.1 hz - 12Mhz
			  2.Freq1: channel1频率值, 0.1 hz - 12Mhz
			  3.Freq_SFR: 0 或 1
			  4.WaveMode: TRI_WAVE(三角波),SIN_WAVE(正弦波),SQU_WAVE(方波)
			  5.Phase : 波形1的初相位,调节该值可调整两个波形之间的相位差(并非与360度一比一对应)
*	返 回 值: 无
*********************************************************************************************************
*/
void AD9833_WaveSeting_Double(double Freq0, double Freq1, unsigned int Freq_SFR, unsigned int WaveMode0, unsigned int WaveMode1, unsigned int Phase0, unsigned int Phase1)
{

	int frequence_LSB0, frequence_MSB0, frequence_LSB1, frequence_MSB1, Phs_data0, Phs_data1;
	double frequence_mid, frequence_DATA;
	long int frequence_hex;

	/*********************************计算频率的16进制值***********************************/
	frequence_mid = 268435456 / 25; // 适合25M晶振
	// 如果时钟频率不为25MHZ，修改该处的频率值，单位MHz ，AD9833最大支持25MHz
	frequence_DATA = Freq0;
	frequence_DATA = frequence_DATA / 1000000;
	frequence_DATA = frequence_DATA * frequence_mid;
	frequence_hex = frequence_DATA;			  // 这个frequence_hex的值是32位的一个很大的数字，需要拆分成两个14位进行处理；
	frequence_LSB0 = frequence_hex;			  // frequence_hex低16位送给frequence_LSB
	frequence_LSB0 = frequence_LSB0 & 0x3fff; // 去除最高两位，16位数换去掉高位后变成了14位
	frequence_MSB0 = frequence_hex >> 14;	  // frequence_hex高16位送给frequence_HSB
	frequence_MSB0 = frequence_MSB0 & 0x3fff; // 去除最高两位，16位数换去掉高位后变成了14位

	frequence_DATA = Freq1;
	frequence_DATA = frequence_DATA / 1000000;
	frequence_DATA = frequence_DATA * frequence_mid;
	frequence_hex = frequence_DATA;			  // 这个frequence_hex的值是32位的一个很大的数字，需要拆分成两个14位进行处理；
	frequence_LSB1 = frequence_hex;			  // frequence_hex低16位送给frequence_LSB
	frequence_LSB1 = frequence_LSB1 & 0x3fff; // 去除最高两位，16位数换去掉高位后变成了14位
	frequence_MSB1 = frequence_hex >> 14;	  // frequence_hex高16位送给frequence_HSB
	frequence_MSB1 = frequence_MSB1 & 0x3fff; // 去除最高两位，16位数换去掉高位后变成了14位

	Phs_data0 = Phase0 | 0xC000; // 相位值
	Phs_data1 = Phase1 | 0xC000;
	AD9833_Write(0x0100, 0);   // 复位AD9833,即RESET位为1
	AD9833_Write(0x2100, 0);   // 选择数据一次写入，B28位和RESET位为1
	AD9833_Write(0x0100, 1);   // 复位AD9833,即RESET位为1
	AD9833_Write(0x2100, 1);   // 选择数据一次写入，B28位和RESET位为1

	if (Freq_SFR == 0) // 把数据设置到设置频率寄存器0
	{
		frequence_LSB0 = frequence_LSB0 | 0x4000;
		frequence_MSB0 = frequence_MSB0 | 0x4000;
		frequence_LSB1 = frequence_LSB1 | 0x4000;
		frequence_MSB1 = frequence_MSB1 | 0x4000;
		// 使用频率寄存器0输出波形
		AD9833_Write(frequence_LSB0, 0); // L14，选择频率寄存器0的低14位数据输入
		AD9833_Write(frequence_MSB0, 0); // H14 频率寄存器的高14位数据输入
		AD9833_Write(Phs_data0, 0);		 // 设置相位0
		AD9833_Write(Phs_data1, 1);		 // 设置相位1
		AD9833_Write(frequence_LSB1, 1); // L14，选择频率寄存器0的低14位数据输入
		AD9833_Write(frequence_MSB1, 1); // H14 频率寄存器的高14位数据输入
		// AD9833_Write(0x2000); /**设置FSELECT位为0，芯片进入工作状态,频率寄存器0输出波形**/
	}
	if (Freq_SFR == 1) // 把数据设置到设置频率寄存器1
	{
		frequence_LSB0 = frequence_LSB0 | 0x8000;
		frequence_MSB0 = frequence_MSB0 | 0x8000;
		frequence_LSB1 = frequence_LSB1 | 0x8000;
		frequence_MSB1 = frequence_MSB1 | 0x8000;
		// 使用频率寄存器1输出波形
		AD9833_Write(frequence_LSB0, 0); // L14，选择频率寄存器1的低14位输入
		AD9833_Write(frequence_MSB0, 0); // H14 频率寄存器1为
		AD9833_Write(Phs_data0, 0);		 // 设置相位0
		AD9833_Write(Phs_data1, 1);		 // 设置相位1
		AD9833_Write(frequence_LSB1, 1); // L14，选择频率寄存器1的低14位输入
		AD9833_Write(frequence_MSB1, 1); // H14 频率寄存器1为
		// AD9833_Write(0x2800); /**设置FSELECT位为0，设置FSELECT位为1，即使用频率寄存器1的值，芯片进入工作状态,频率寄存器1输出波形**/
	}

	// if (WaveMode == TRI_WAVE) // 输出三角波波形
	// {
	// 	AD9833_Write(0x2002, 0);
	// 	AD9833_Write(0x2002, 1);
	// }
	// if (WaveMode == SQU_WAVE) // 输出方波波形
	// {
	// 	AD9833_Write(0x2028, 0);
	// 	AD9833_Write(0x2028, 1);
	// }
	// if (WaveMode == SIN_WAVE) // 输出正弦波形
	// {
	// 	AD9833_Write(0x2000, 0);
	// 	AD9833_Write(0x2000, 1);
	// }
	switch (WaveMode0)
	{
	case TRI_WAVE:
		AD9833_Write(0x2002, 0);
		break;
	case SIN_WAVE:
		AD9833_Write(0x2000, 0);
		break;
	case SQU_WAVE:
		AD9833_Write(0x2028, 0);
		break;
	default:
		break;
	}
	switch (WaveMode1)
	{
	case TRI_WAVE:
		AD9833_Write(0x2002, 1);
		break;
	case SIN_WAVE:
		AD9833_Write(0x2000, 1);
		break;
	case SQU_WAVE:
		AD9833_Write(0x2028, 1);
		break;
	default:
		break;
	}
}
