#include "xFFT.h"

ALIGN_32BYTES(Complex q[FFT_LENGTH])
__attribute__((section(".ARM.__at_0x24000000")));
ALIGN_32BYTES(Complex q1[FFT_LENGTH])
__attribute__((section(".ARM.__at_0x24040000")));

void FFT_DIS(float *p_phase0)
{
	//	for(uint32_t i=1550; i<6750; i++)
	//	for(uint32_t i = 0; i < FFT_LENGTH; i++)
	// for (uint32_t i = 0; i < FFT_LENGTH; i++)
	// {
	// 	printf("%f\r\n", q1[i].real);
	// 	//	printf("%d:\t%f\r\n", i,  q1[i].real);
	// }

	// for (uint32_t i = 0; i < FFT_LENGTH; i++)
	// {
	// 	if(q1[i].real<1)
	// 		q1[i].imag = 0;
	// 	printf("%f\r\n", q1[i].imag);
	// }

	float32_t mod_max,angle;
	uint32_t max_index1, max_index2;

	// printf("255\r\n");
	// arm_max_f32((float32_t *)&q1[1].real, FFT_LENGTH / 2 - 1, &mod_max, &max_index1);
	// max_index1 = max_index1 / 2 + 1;
	// printf("%.0f\n", (max_index1) * 1000 * 640.0 / FFT_LENGTH);

	// arm_max_f32((float32_t *)&q1[128 + max_index1].real, FFT_LENGTH / 2 - 128 - max_index1, &mod_max, &max_index2);
	// max_index2 = max_index2 / 2 + 128 + max_index1;
	// printf("%.0f\n", (max_index2) * 1000 * 640.0 / FFT_LENGTH);

	// printf("255\r\n");
	arm_max_f32((float32_t *)&q1[1].real, FFT_LENGTH / 2 - 1, &mod_max, &max_index1);
	*(&q1[1].real + max_index1) = 0;
	arm_max_f32((float32_t *)&q1[1].real, FFT_LENGTH / 2 - 1, &angle, &max_index2);
	*(&q1[1].real + max_index1) = mod_max;
	max_index1 = max_index1 / 2 + 1;
	max_index2 = max_index2 / 2 + 1;

	if(max_index1>max_index2)
	{
		max_index1 = max_index1 ^ max_index2;
		max_index2 = max_index1 ^ max_index2;
		max_index1 = max_index1 ^ max_index2;
	}

	
	// printf("%.0f\r\n", (max_index2) * 1000 * 640.0 / FFT_LENGTH);

	// float32_t rate = q1[3 * max_index1].real / q1[max_index1].real;
	// if (max_index2/3==max_index1)
	// 	rate = q1[3 * max_index2].real / q1[max_index2].real;
	if (max_index1 * 3 != max_index2)
		mod_max += 3 * (max_index1) * 640.0 / FFT_LENGTH;

	*p_phase0 += 10/*这里的参数（现在是10）需要调整*/ * 360.0; // ((max_index1) * 1000 * 640.0 / FFT_LENGTH);
	while (*p_phase0 * 4096.0 / 360 - 160 - 61.44 * (max_index2 / max_index1 - 2) >= 4096)
		*p_phase0 -= 360;
	float32_t phase1, phase2;
	phase1 = *p_phase0;
	phase2 = *p_phase0 + phase;
	while (phase1 * 4096.0 / 360 - 160 - 61.44 * (max_index2 / max_index1 - 2) < 0) //- (max_index1) * 6 * 640.0 / FFT_LENGTH
		phase1 += 360;
	while (phase1 * 4096.0 / 360 - 160 - 61.44 * (max_index2 / max_index1 - 2) >= 4096)
		phase1 -= 360;
	while (phase2 * 4096.0 / 360 - 160 - 61.44 * (max_index2 / max_index1 - 2) < 0) //- (max_index1) * 6 * 640.0 / FFT_LENGTH
		phase2 += 360;
	while (phase2 * 4096.0 / 360 - 160 - 61.44 * (max_index2 / max_index1 - 2) >= 4096)
		phase2 -= 360;

	if (mod_max > 1550 )
	// if (mod_max > 1450 - 3 * (max_index1) * 640.0 / FFT_LENGTH)
		// printf("%f sin\r\n", mod_max);
		printf("255 %.0f %.0f 1 %.0f %.0f\r\n",		(max_index1) * 1000 * 640.0 / FFT_LENGTH, 
													(max_index2) * 1000 * 640.0 / FFT_LENGTH, 
													phase1 * 4096.0 / 360 - 160 - 61.44 * (max_index2 / max_index1 - 2),
													phase2 * 4096.0 / 360 - 160 - 61.44 * (max_index2 / max_index1 - 2));
	else
		// printf("%f triangle\r\n", mod_max);
		printf("255 %.0f %.0f 0 %.0f %.0f\r\n", 	(max_index1) * 1000 * 640.0 / FFT_LENGTH, 
													(max_index2) * 1000 * 640.0 / FFT_LENGTH, 
													phase1 * 4096.0 / 360 - 160 - 61.44 * (max_index2 / max_index1 - 2),
													phase2 * 4096.0 / 360 - 160 - 61.44 * (max_index2 / max_index1 - 2));

	// printf("%.0f\r\n", phase * 4096.0 / 360 - 160);
	// angle = (q1[max_index2].imag + 0.5 * 3.1415926) - max_index2 / max_index1 * (q1[max_index1].imag + 0.5 * 3.1415926);
	// while(angle < 0)
	// 	angle += 2 * 3.1415926;
	// while (angle > 2 * 3.1415926)
	// 	angle -= 2 * 3.1415926;
	// printf("%.0f\n", angle * 2048 / 3.1415926 - (max_index1) * 6 * 640.0 / FFT_LENGTH);
}
void FFT_Init(void)
{

	for (uint32_t i = 0; i < FFT_LENGTH; i++)
	{
		/* *(0.54f-0.46f*cosf(6.283185307f*i/FFT_LENGTH)) */
		//			q1[i].real = adc1_data[i] *(0.5-0.5*cos(6.283185307*i/FFT_LENGTH))*3.3/65536;
		q1[i].real = adc1_data[i] * 3.3 / 65536;
		q1[i].imag = 0;
	}
	FFT(q1, 14);
	for (uint32_t i = 0; i < FFT_LENGTH; i++)
	{
		float mo, angle;
		arm_sqrt_f32((float)(q1[i].real * q1[i].real + q1[i].imag * q1[i].imag), &mo);
		arm_atan2_f32((float)q1[i].imag, (float)q1[i].real, &angle);
		q1[i].real = mo;
		q1[i].imag = angle;
	}
}

unsigned char FFT(Complex *d, int m)
{
#ifndef __EXTERN_W__
	static Complex *w;
	static int mw = 0;
	float arg, w_real, w_imag, wr_real, wr_imag, wtemp;
#endif
	static int n = 1;
	Complex temp, tmp1, tmp2;
	Complex *di, *dip, *dj, *wp;
	int i, j, k, l, le, wi;
#ifndef __EXTERN_W__
	if (m != mw)
	{
		if (mw != 0)
			free(w);
		mw = m;
		if (m == 0)
			return 0;
		n = 1 << m;
		le = n >> 1;
		w = q;
		if (!w)
			return 0;
		arg = 4.0 * atan(1.0) / le;
		wr_real = w_real = cos(arg);
		wr_imag = w_imag = -sin(arg);
		dj = w;
		for (j = 1; j < le; j++)
		{
			dj->real = (float)wr_real;
			dj->imag = (float)wr_imag;
			dj++;
			wtemp = wr_real * w_real - wr_imag * w_imag;
			wr_imag = wr_real * w_imag + wr_imag * w_real;
			wr_real = wtemp;
		}
	}
#else
	n = 1 << m;
#endif
	le = n;
	wi = 1;
	for (l = 0; l < m; l++)
	{
		le >>= 1;
		for (i = 0; i < n; i += (le << 1))
		{
			di = d + i;
			dip = di + le;
			temp.real = (di->real + dip->real);
			temp.imag = (di->imag + dip->imag);
			dip->real = (di->real - dip->real);
			dip->imag = (di->imag - dip->imag);
			*di = temp;
		}
		wp = (Complex *)w + wi - 1;
		for (j = 1; j < le; j++)
		{
			tmp1 = *wp;
			for (i = j; i < n; i += (le << 1))
			{
				di = d + i;
				dip = di + le;
				temp.real = (di->real + dip->real);
				temp.imag = (di->imag + dip->imag);
				tmp2.real = (di->real - dip->real);
				tmp2.imag = (di->imag - dip->imag);
				dip->real = (tmp2.real * tmp1.real - tmp2.imag * tmp1.imag);
				dip->imag = (tmp2.real * tmp1.imag + tmp2.imag * tmp1.real);
				*di = temp;
			}
			wp += wi;
		}
		wi <<= 1;
	}

	for (i = 0; i < n; i++)
	{
		j = 0;
		for (k = 0; k < m; k++)
			j = (j << 1) | ((i >> k) & 1);
		if (i < j)
		{
			di = d + i;
			dj = d + j;
			temp = *dj;
			*dj = *di;
			*di = temp;
		}
	}
	return 1;
}
