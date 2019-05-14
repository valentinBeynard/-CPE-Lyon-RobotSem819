#include "sound_handler.h"

const uint16_t sine_wave_array[256] = {
	2048,2098,2148,2198,2248,2298,2348,
2398,2447,2496,2545,2594,2642,2690,2737,
2785,2831,2877,2923,2968,3013,3057,3100,
3143,3185,3227,3267,3307,3347,3385,3423,
3460,3496,3531,3565,3598,3631,3662,3692,
3722,3750,3778,3804,3829,3854,3877,3899,
3920,3940,3958,3976,3992,4007,4021,4034,
4046,4056,4065,4073,4080,4086,4090,4093,
4095,4095,4095,4093,4090,4086,4080,4073,
4065,4056,4046,4034,4021,4007,3992,3976,          
3958,3940,3920,3899,3877,3854,3829,3804,
3778,3750,3722,3692,3662,3631,3598,3565,
3531,3496,3460,3423,3385,3347,3307,3267,
3227,3185,3143,3100,3057,3013,2968,2923,
2877,2831,2785,2737,2690,2642,2594,2545,
2496,2447,2398,2348,2298,2248,2198,2148,
2098,2048,1997,1947,1897,1847,1797,1747,
1697,1648,1599,1550,1501,1453,1405,1358,
1310,1264,1218,1172,1127,1082,1038,995,
952,910,868,828,788,748,710,672,635,599,
564,530,497,464,433,403,373,345,317,291,
266,241,218,196,175,155,137,119,103,88,
74,61,49,39,30,22,15,9,5,2,0,0,0,2,5,9,
15,22,30,39,49,61,74,88,103,119,137,155,
175,196,218,241,266,291,317,345,373,403,
433,464,497,530,564,599,635,672,710,748,
788,828,868,910,952,995,1038,1082,1127,
1172,1218,1264,1310,1358,1405,1453,1501,
1550,1599,1648,1697,1747,1797,1847,1897,1947,
1997 };

SOUND_PCK * sound_pck = 0;

uint16_t signal_time_H = 2 * 250;

uint16_t signal_time_L = 2 * 500;

uint16_t signal_time_Acq = 100;

uint16_t signal_time_counter = 0;

unsigned char inverse_signal = 1;

unsigned char is_high = 0;

char adc_buffer[ADC_BUFFER_SIZE];
byte adc_buffer_ptr = 0;

uint8_t start_flag[6] = "START\n"; 
uint8_t stop_flag[5] = "STOP\n"; 

byte acq_busy = 0;

uint32_t time_step_cnt = 0;

uint32_t adc1value;
uint32_t samples_counter = 0;

uint32_t somme50 = 0;
uint32_t adc2value;

SH_STATE sh_current_state = SH_IDLE;

/*
*		####################################################
*					ADC INTERRUPT
*		####################################################
*/
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc1){
	
	if (hadc1->Instance == ADC1){
		if(sh_current_state == SH_ACQ)
		{
			// For Simple recopy
			adc1value = HAL_ADC_GetValue(hadc1);
			
			/* Sampling for bluetooth transmission purpose */	
			sprintf(adc_buffer, "%d:%f\n", adc1value, (time_step_cnt * ADC1_TIME_STEP)); 
			time_step_cnt++;
			
			HAL_UART_Transmit(sound_pck->huart4, (uint8_t *)adc_buffer, ADC_BUFFER_SIZE, 5);
			
		}
		else{
			HAL_ADC_Stop(hadc1);
		}
	}
}

/*
*		####################################################
*					TIMER INTERRUPT
*		####################################################
*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/* Interrupt each 10 ms */
	if(htim->Instance == TIM3)
	{
		
		/* Sound Generator */
		if(sh_current_state == SH_GENE)
		{
			if(is_high == 1)
			{
				if(signal_time_counter >= signal_time_H)
				{
					is_high = 0;
					inverse_signal = 1;
					signal_time_counter = 0;
				}
				else
				{
					signal_time_counter++;
				}
			}
			else
			{
				if(signal_time_counter >= signal_time_L)
				{
					is_high = 1;
					inverse_signal = 1;
					signal_time_counter = 0;
				}
				else
				{
					signal_time_counter++;
				}
			}	
		}
		/* Sound Acquisition */
		else
		{
			if(signal_time_counter < signal_time_Acq)
			{
				signal_time_counter++;
			}
			else
			{
				sh_current_state = SH_IDLE;
				signal_time_counter = 0;
				acq_busy = 0;
				HAL_TIM_Base_Stop_IT(htim);
				HAL_UART_Transmit(sound_pck->huart4, stop_flag, 5, 10);
			}
			
		}
	}

	/* Envelop detection : sampling 100 points and compare with a threshold */
//	else if(htim->Instance == TIM2)
//	{
//		HAL_ADC_Start((sound_pck->hadc2));
//		HAL_ADC_PollForConversion((sound_pck->hadc2),10);
//		adc2value = HAL_ADC_GetValue((sound_pck->hadc2));
//		
//		if(samples_counter < 100){
//			if(adc2value < THRESHOLD){
//				somme50 += THRESHOLD-adc2value;
//			}
//			else{
//				somme50 += adc2value -THRESHOLD;
//			}
//			samples_counter++;
//		}
//		else{
//			if (adc2value == 0x000){
//				HAL_DAC_SetValue((sound_pck->hdac),DAC_CHANNEL_2,DAC_ALIGN_12B_R,0x000);
//			}
//			else{
//				if(somme50 > VDETECT){
//					HAL_DAC_SetValue(sound_pck->hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,0xFFF);
//				}
//				else{
//					HAL_DAC_SetValue(sound_pck->hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,0x000);
//				}
//				HAL_DAC_Start(sound_pck->hdac,DAC_CHANNEL_2);
//				samples_counter=0;
//				somme50 = 0;
//			}
//		}
//		
//	}
	
}
/*
*		####################################################
*					TIMER INTERRUPT END
*		####################################################
*/

void freq_calculator(SOUND_PCK* pck)
{
	switch(pck->cmd_pck->frequency_code)
	{
		case 0x01: // DO 261.6 Hz
			pck->htim6->Init.Period = 125;
			break;
		case 0x02:	// RE 293.7 Hz
			pck->htim6->Init.Period = 140;
			break;
		case 0x03:
			pck->htim6->Init.Period = 157;
			break;
		case 0x04:
			pck->htim6->Init.Period = 167;
			break;
		case 0x05:
			pck->htim6->Init.Period = 187;
			break;
		case 0x06:
			pck->htim6->Init.Period = 210;
			break;
		case 0x07:
			pck->htim6->Init.Period = 236;
			break;
		case 0x08:
			pck->htim6->Init.Period = 250;
			break;
		case 0x09:
			pck->htim6->Init.Period = 280;
			break;
		case 0x0A:
			pck->htim6->Init.Period = 315;
			break;
		case 0x0B:
			pck->htim6->Init.Period = 333;
			break;
		case 0x0C:
			pck->htim6->Init.Period = 374;
			break;
		case 0x0D:
			pck->htim6->Init.Period = 420;
			break;
		case 0x0E:
			pck->htim6->Init.Period = 471;
			break;
		case 0x0F:
			pck->htim6->Init.Period = 499;
			break;
		case 0x10:
			pck->htim6->Init.Period = 560;
			break;
		case 0x11:
			pck->htim6->Init.Period = 629;
			break;
		case 0x12:
			pck->htim6->Init.Period = 667;
			break;
		case 0x13:
			pck->htim6->Init.Period = 748;
			break;
		case 0x14:
			pck->htim6->Init.Period = 840;
			break;
		case 0x15:
			pck->htim6->Init.Period = 943;
			break;
		default:
			pck->htim6->Init.Period = 210;
			break;			
	}
	//pck->htim6->Init.Period = 48000000/ ((pck->cmd_pck->frequency_code) * (pck->htim6->Init.Prescaler + 1));
	//pck->htim6->Init.Period = 210;
}

/*
		Get main SINUS_PCK packet in order to working with it 
*/
void init_sound_handler(SOUND_PCK* pck)
{
	sound_pck = pck;
	
}

/*
Initialise le generateur de sinus : temps son/silence et fr�quence
*/
void init_sinus(SOUND_PCK* pck)
{
	// Initialise Temps son/silence
	signal_time_H = 2 * pck->cmd_pck->delay_ON;
	signal_time_L = 2 * pck->cmd_pck->delay_OFF;
	
	// Initialise la nouvelle fr�quence
	pck->htim6->Init.Prescaler = 1;
	freq_calculator(pck);

	HAL_TIM_Base_Stop(pck->htim6);
	HAL_DAC_Stop(pck->hdac,DAC_CHANNEL_1);
	
	// Timer 6 Init
	TIM_MasterConfigTypeDef sMasterConfig = {0};
  if (HAL_TIM_Base_Init(pck->htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(pck->htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  DAC_ChannelConfTypeDef sConfig = {0};
  if (HAL_DAC_Init(pck->hdac) != HAL_OK)
  {
    Error_Handler();
  }
  sConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  if (HAL_DAC_ConfigChannel(pck->hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
	
	// Re-start periph.
	HAL_TIM_Base_Start(pck->htim6);  
  HAL_DAC_Start(pck->hdac,DAC_CHANNEL_1);  
  HAL_DAC_Start_DMA(pck->hdac, DAC_CHANNEL_1, (uint32_t*)sine_wave_array, 256, DAC_ALIGN_12B_R);

	// Enable interrupt on timer 3
	HAL_TIM_Base_Start_IT(pck->htim3);
	
	HAL_DAC_Stop(pck->hdac,DAC_CHANNEL_1);
}

unsigned char send_signal(SOUND_PCK* pck)
{
	static unsigned char bips_counter = 0;
	
	if(bips_counter <= pck->cmd_pck->nbr_sound)
	{
		if(inverse_signal == 1)
		{			
			if(is_high)
			{
				HAL_DAC_Start(pck->hdac,DAC_CHANNEL_1);
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_SET);
				bips_counter++;
			}
			else
			{
				HAL_DAC_Stop(pck->hdac,DAC_CHANNEL_1);
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);
			}
			inverse_signal = 0;
		}

		return 1;
	}
	else
	{
		HAL_DAC_Stop(pck->hdac,DAC_CHANNEL_1);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);
		HAL_TIM_Base_Stop_IT(pck->htim3);
		bips_counter = 0;
		inverse_signal = 0;
		is_high = 0;
		signal_time_counter = 0;
		return 0;
	}
}

void sound_handler_process(SOUND_PCK * pck)
{
	switch(sh_current_state)
	{
		case SH_IDLE:
			if(pck->cmd_pck->Etat_GEN == GEN_oui)
			{
				sh_current_state = SH_GENE;
				pck->cmd_pck->Etat_GEN = GEN_non;
			}
			else if(pck->cmd_pck->Etat_ACQ == ACQ_oui)
			{
				sh_current_state = SH_ACQ;
				pck->cmd_pck->Etat_ACQ = ACQ_non;
			}
			else{
				sh_current_state = SH_IDLE;
			}
			break;
				
		case SH_GENE:
			sinus_generate(pck);
			break;

		case SH_ACQ:
			sound_acquisition(pck);
			break;
	}
}

void sinus_generate(SOUND_PCK * pck) 
{
	static unsigned char init_flag = 2;
	
	/* Init Sinus params */
	if(init_flag != 0)
	{
		init_sinus(pck);
		init_flag = 0;
	}
	else	/* End Condition to quit SH_GENE state */
	{
		/* All bips have been emitted */
		if(send_signal(pck) == 0)
		{
			sh_current_state = SH_IDLE;
			init_flag = 1;
		}
	}

}

void sound_acquisition(SOUND_PCK* pck)
{
	static uint16_t last_time_step = 0;
	
	if(acq_busy == 0)
	{
		signal_time_Acq = 10 * pck->cmd_pck->delay_acq;
		HAL_UART_Transmit(sound_pck->huart4, start_flag, 6, 10);
		HAL_ADC_Start(pck->hadc1);
		// Enable interrupt on timer 2 for envelop detection
		//HAL_TIM_Base_Start_IT(pck->htim2);
		// Enable interrupt on timer 3 for timing purpose
		HAL_TIM_Base_Start_IT(pck->htim3);
		acq_busy = 1;
	}
	/* Constant check and periodic data send from bluetooth */
	else
	{
		//time_step_cnt = 0;
	}
	
}

/*
		Recopy signal on ADC1 Input during all processing 
*/
void signal_recopieur()
{
		HAL_DAC_Start	(sound_pck->hdac, DAC_CHANNEL_1);
		HAL_DAC_SetValue (sound_pck->hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, adc1value);
}