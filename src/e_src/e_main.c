/**
  ******************************************************************************
  * @file    GPIO_Toggle\main.c
  * @author  MCD Application Team
  * @version  V2.2.0
  * @date     30-September-2014
  * @brief   This file contains the main function for GPIO Toggle example.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "stdio.h"


#define AS_TRANSIMITER		0
#define AS_RECEIVER				1
#define AS_TX_OR_RX				AS_RECEIVER
#define SI4432_TX()			(AS_TX_OR_RX == AS_TRANSIMITER)
#define SI4432_RX()			(AS_TX_OR_RX == AS_RECEIVER)

typedef struct{
GPIO_TypeDef* GPIOx;
GPIO_Pin_TypeDef GPIO_Pin;
}STRUCT_IO_PORT_PIN;

/**
  * @addtogroup GPIO_Toggle
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TIM4_PERIOD       124
#define E_TIM4_PERIOD		24
#define E_TIM4_BASE_TIME	200	//	200 us

#ifdef _RAISONANCE_
#define PUTCHAR_PROTOTYPE int putchar (char c)
#define GETCHAR_PROTOTYPE int getchar (void)
#elif defined (_COSMIC_)
#define PUTCHAR_PROTOTYPE char putchar (char c)
#define GETCHAR_PROTOTYPE char getchar (void)
#else /* _IAR_ */
#define PUTCHAR_PROTOTYPE int putchar (int c)
#define GETCHAR_PROTOTYPE int getchar (void)
#endif /* _RAISONANCE_ */
/* Evalboard I/Os configuration */

#define MOTOR_CONTROL_MODE_1		1		//	在交流的周期内部调整脉宽
#define MOTOR_CONTROL_MODE_2		2		//	以交流的一个周期为基准，周期内部不调整脉宽
#define MOTOR_CONTROL_MODE			MOTOR_CONTROL_MODE_1

#if (MOTOR_CONTROL_MODE==MOTOR_CONTROL_MODE_1)
#define AC_MODE_50HZ
//#define AC_MODE_60HZ
#endif

#define MOTOR_PORT			(GPIOA)		//	GPIOB
#define MOTOR_PIN				(GPIO_PIN_1)	//	 | GPIO_PIN_4)

#define EXTI_TLI_PORT		(GPIOA)
#define EXTI_TLI_PIN		(GPIO_PIN_1)

#define LED_1_PORT  (GPIOD)
#define LED_1_PINS  (GPIO_PIN_7)

#define LED_2_PORT  (GPIOE)
#define LED_2_PINS  (GPIO_PIN_6)

#define LED_3_PORT  (GPIOE)
#define LED_3_PINS  (GPIO_PIN_5)

#define LED_4_PORT  (GPIOE)
#define LED_4_PINS  (GPIO_PIN_0)

#define LED_5_PORT  (GPIOG)
#define LED_5_PINS  (GPIO_PIN_1)

#define LED_6_PORT  (GPIOC)
#define LED_6_PINS  (GPIO_PIN_6)

#define LED_7_PORT  (GPIOD)
#define LED_7_PINS  (GPIO_PIN_4)

#define LED_8_PORT  (GPIOD)
#define LED_8_PINS  (GPIO_PIN_3)

#define LED_9_PORT  (GPIOD)
#define LED_9_PINS  (GPIO_PIN_2)

#define LED_10_PORT  (GPIOD)
#define LED_10_PINS  (GPIO_PIN_0)

#define LED_11_PORT  (GPIOC)
#define LED_11_PINS  (GPIO_PIN_3)

#define LED_12_PORT  (GPIOC)
#define LED_12_PINS  (GPIO_PIN_5)

#define LED_13_PORT  (GPIOC)
#define LED_13_PINS  (GPIO_PIN_7)

#define LED_STAT_PORT  (GPIOA)
#define LED_STAT_PINS  (GPIO_PIN_1)

#define IO_LED_NUM		(13)

const STRUCT_IO_PORT_PIN Led_Io_Port_Pin[IO_LED_NUM] = {
{LED_1_PORT,	LED_1_PINS},
{LED_2_PORT,	LED_2_PINS},
{LED_3_PORT,	LED_3_PINS},
{LED_4_PORT,	LED_4_PINS},
{LED_5_PORT,	LED_5_PINS},
{LED_6_PORT,	LED_6_PINS},
{LED_7_PORT,	LED_7_PINS},
{LED_8_PORT,	LED_8_PINS},
{LED_9_PORT,	LED_9_PINS},
{LED_10_PORT,	LED_10_PINS},
{LED_11_PORT,	LED_11_PINS},
{LED_12_PORT,	LED_12_PINS},
{LED_13_PORT,	LED_13_PINS},
};

#define LED_TIMEROUT		5*60	//	30
uint16_t g_LedTimer = 0;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t TimingDelay = 0;
__IO uint8_t time4_cnt = 0;
__IO uint8_t time_1ms_dly_flag = 0;
__IO uint8_t time_1ms_flag = 0, time_1ms_cnt = 0;
__IO uint8_t time_10ms_flag = 0, time_10ms_cnt = 0;
__IO uint8_t time_100ms_flag = 0, time_100ms_cnt = 0;
__IO uint8_t time_1s_flag = 0, time_1s_cnt = 0;

uint8_t onOff = 0;
uint8_t motorSpeed = 5;
#define MOTOR_SPEED_MAX		10
#define MOTOR_SPEED_MIN		0
uint8_t motorTimerCnt = 0;

#define PAIR_DATA_COUNT		12
uint8_t rmt_pair_data[PAIR_DATA_COUNT];
uint8_t rmt_pair_timerout = 5;

/* Private function prototypes -----------------------------------------------*/
void Delay (uint16_t nCount);
void DelayMs(uint32_t cnt);
static void CLK_Config(void);
static void TIM4_Config(void);
static void MotorOn(void);
static void MotorOff(void);

/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/

void KeyLed(uint8_t led_index, uint8_t flag)
{
	if(flag)
		GPIO_WriteLow(Led_Io_Port_Pin[led_index].GPIOx, (GPIO_Pin_TypeDef)Led_Io_Port_Pin[led_index].GPIO_Pin);
	else
		GPIO_WriteHigh(Led_Io_Port_Pin[led_index].GPIOx, (GPIO_Pin_TypeDef)Led_Io_Port_Pin[led_index].GPIO_Pin);
}


void KeyLedAllOff(void)
{
	uint8_t i;
	for(i=0; i<IO_LED_NUM; i++)
		KeyLed(i, 0);
}

enum{
FAN_STAT_ALL_ON,
FAN_STAT_ALL_OFF,
FAN_STAT_BLINK,
};
uint8_t g_FanStat = FAN_STAT_BLINK;
uint8_t g_FanTimeCnt = 0;
void FanFunc(void);

#include "e_eeprom.c"
#include "e_si4432.c"
#include "e_si4432_encode_decode.c"
#include "e_get_key.c"
#include "e_deal_key.c"
#include "e_on_timer.c"

void FanOn(void)
{
	GPIO_WriteHigh(key_def[12].GPIOx, (GPIO_Pin_TypeDef)key_def[12].GPIO_Pin);
}
void FanOff(void)
{
	GPIO_WriteLow(key_def[12].GPIOx, (GPIO_Pin_TypeDef)key_def[12].GPIO_Pin);
}
void FanCtrlInit(void)
{
  GPIO_Init(key_def[12].GPIOx, (GPIO_Pin_TypeDef)key_def[12].GPIO_Pin, GPIO_MODE_OUT_PP_HIGH_FAST);
	FanOff();
	g_FanStat = FAN_STAT_BLINK;
	g_FanTimeCnt = 0;
}
void FanFunc(void)
{
	switch(g_FanStat)
	{
	case FAN_STAT_ALL_ON:
		FanOn();
		g_FanTimeCnt = 0;
		break;
	case FAN_STAT_ALL_OFF:
		FanOff();
		g_FanTimeCnt = 0;
		break;
	case FAN_STAT_BLINK:
		if(++g_FanTimeCnt >= 180)	g_FanTimeCnt = 0;

		if(g_FanTimeCnt < 80)
		{
			FanOn();
		}
		else if(g_FanTimeCnt < 180)
		{
			FanOff();
		}
		break;
	}
}

#if 0
uint8_t ChipUniqueID[12];
void Get_ChipID(void)
{
uint8_t i;
	printf("\n\r\n\r");
	for(i=0; i<12; i++)
	{
		ChipUniqueID[i] = *(__IO uint16_t *)(0x4926+i);
		printf(" %x ", ChipUniqueID[i]);
	}
	printf("\n\r\n\r");
}
#endif


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
//	char ans;

  /* Clock configuration -----------------------------------------*/
  CLK_Config();

  /* TIM4 configuration -----------------------------------------*/
  TIM4_Config();

  /* Initialize I/Os in Output Mode */
  GPIO_Init(LED_1_PORT, (GPIO_Pin_TypeDef)LED_1_PINS, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(LED_2_PORT, (GPIO_Pin_TypeDef)LED_2_PINS, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(LED_3_PORT, (GPIO_Pin_TypeDef)LED_3_PINS, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(LED_4_PORT, (GPIO_Pin_TypeDef)LED_4_PINS, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(LED_5_PORT, (GPIO_Pin_TypeDef)LED_5_PINS, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(LED_6_PORT, (GPIO_Pin_TypeDef)LED_6_PINS, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(LED_7_PORT, (GPIO_Pin_TypeDef)LED_7_PINS, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(LED_8_PORT, (GPIO_Pin_TypeDef)LED_8_PINS, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(LED_9_PORT, (GPIO_Pin_TypeDef)LED_9_PINS, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(LED_10_PORT, (GPIO_Pin_TypeDef)LED_10_PINS, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(LED_11_PORT, (GPIO_Pin_TypeDef)LED_11_PINS, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(LED_12_PORT, (GPIO_Pin_TypeDef)LED_12_PINS, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(LED_13_PORT, (GPIO_Pin_TypeDef)LED_13_PINS, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(LED_STAT_PORT, (GPIO_Pin_TypeDef)LED_STAT_PINS, GPIO_MODE_OUT_PP_LOW_FAST);

	KeyLedAllOff();


	//	Initialize external interrupt TLI
//  GPIO_Init(EXTI_TLI_PORT, (GPIO_Pin_TypeDef)EXTI_TLI_PIN, GPIO_MODE_IN_FL_IT);

//	EXTI_SetTLISensitivity(EXTI_TLISENSITIVITY_FALL_ONLY);

	// Initialize motor contorl pin
	GPIO_Init(MOTOR_PORT, (GPIO_Pin_TypeDef)MOTOR_PIN, GPIO_MODE_OUT_PP_LOW_FAST);


#ifndef DISABLE_UART3
  UART3_DeInit();
  /* UART3 configuration ------------------------------------------------------*/
  /* UART3 configured as follow:
        - BaudRate = 115200 baud
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Receive and transmit enabled
        - UART3 Clock disabled
  */
  UART3_Init((uint32_t)38400, UART3_WORDLENGTH_8D, UART3_STOPBITS_1, UART3_PARITY_NO,
              UART3_MODE_TXRX_ENABLE);
#endif

  /* enable interrupts */
  enableInterrupts();


  /* Output a message on Hyperterminal using printf function */
//  printf("\n\rUART3 Example :retarget the C library printf()/getchar() functions to the UART\n\r");
//  printf("\n\rEnter Text\n\r");

//	Get_ChipID();

	KeyInit();

	InitEeprom();

//	E_read_pair_data();
//	E_read_motor_speed();

//	E_InitSi4432();

	FanCtrlInit();

  while (1)
  {
//  	ans = getchar();
//		printf("%c", ans);
		On1ms();
		On10ms();
		On100ms();
		On1s();
    /* Toggles LEDs */
//    GPIO_WriteReverse(LED_1_PORT, (GPIO_Pin_TypeDef)LED_1_PINS);
//    Delay(0xFFFF);
  }

}

/**
  * @brief  Configure system clock to run at 16Mhz
  * @param  None
  * @retval None
  */
static void CLK_Config(void)
{
    /* Initialization of the clock */
    /* Clock divider to HSI/1 */
//    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);

		CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSE, DISABLE, CLK_CURRENTCLOCKSTATE_DISABLE);
}


/**
  * @brief  Configure TIM4 to generate an update interrupt each 1ms
  * @param  None
  * @retval None
  */
static void TIM4_Config(void)
{
  /* TIM4 configuration:
   - TIM4CLK is set to 16 MHz, the TIM4 Prescaler is equal to 128 so the TIM1 counter
   clock used is 16 MHz / 128 = 125 000 Hz
  - With 125 000 Hz we can generate time base:
      max time base is 2.048 ms if TIM4_PERIOD = 255 --> (255 + 1) / 125000 = 2.048 ms
      min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 125000 = 0.016 ms
  - In this example we need to generate a time base equal to 1 ms
   so TIM4_PERIOD = (0.001 * 125000 - 1) = 124 */

  /* Time base configuration */
  TIM4_TimeBaseInit(TIM4_PRESCALER_128, TIM4_PERIOD);
  //	Change timer4 interrupt period as ( 24 + 1) / 12500 = 0.2 ms (200us)
  TIM4_TimeBaseInit(TIM4_PRESCALER_128, E_TIM4_PERIOD);
  TIM4_TimeBaseInit(TIM4_PRESCALER_64, 172);	//	使用外部11.0592M晶振  1ms中断
  /* Clear TIM4 update flag */
  TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  /* Enable update interrupt */
  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);

  /* Enable TIM4 */
  TIM4_Cmd(ENABLE);
}

#if (MOTOR_CONTROL_MODE==MOTOR_CONTROL_MODE_1)

	#if defined(AC_MODE_50HZ)

#define MOTOR_TIME_MIN		3
#define MOTOR_TIME_MAX		27
const uint8_t Speed_DATA[11]  =
{
0,
MOTOR_TIME_MAX,
25,	23, 21, 19, 16, 13, 10, 7,
MOTOR_TIME_MIN
};

	#elif defined(AC_MODE_60HZ)

#define MOTOR_TIME_MIN		2
#define MOTOR_TIME_MAX		30
const uint8_t Speed_DATA[11]  =
{
0,
MOTOR_TIME_MAX,
28,	24, 20, 18, 13, 9, 7, 5,
MOTOR_TIME_MIN
};

	#endif

#elif (MOTOR_CONTROL_MODE==MOTOR_CONTROL_MODE_2)

#define MOTOR_TIME_MIN		5
#define MOTOR_TIME_MAX		15
const uint8_t Speed_DATA[11]  =
{
0,
MOTOR_TIME_MIN,
6,	8, 9, 10, 11, 12, 13, 14,
MOTOR_TIME_MAX
};
#endif
void TIM4_Handle(void)
{
#if (MOTOR_CONTROL_MODE==MOTOR_CONTROL_MODE_1)

	if(onOff && motorSpeed)
	{
		if(motorTimerCnt < 0xff)	++motorTimerCnt;

		if( (motorTimerCnt <= (Speed_DATA[motorSpeed]+3) ) && (motorTimerCnt >= (Speed_DATA[motorSpeed]) ) )
		{
			MotorOn();
		}
		else
		{
			MotorOff();
		}
	}
	else
	{
		MotorOff();
	}

#endif

//	GPIO_WriteReverse(Led_Io_Port_Pin[0].GPIOx, (GPIO_Pin_TypeDef)Led_Io_Port_Pin[0].GPIO_Pin);

//	GPIO_WriteReverse(LED_1_PORT, (GPIO_Pin_TypeDef)LED_1_PINS);
//	if( ++time4_cnt >= (1000 / E_TIM4_BASE_TIME) )	//	1ms timeout
	{
		time4_cnt = 0;
		time_1ms_flag = 1;

		time_1ms_dly_flag = 1;

		if( ++time_1ms_cnt >= 10)		//	10ms timeout
		{
			time_1ms_cnt = 0;
			time_10ms_flag = 1;

			if( ++time_10ms_cnt >= 10 )	//	100ms timeout
			{
				time_10ms_cnt = 0;
				time_100ms_flag = 1;

				if( ++time_100ms_cnt >= 10 )	//	1s timeout
				{
					time_100ms_cnt = 0;
					time_1s_flag = 1;

					if( ++time_1s_cnt >= 1 )
					{
					}
				}
			}
		}
	}


}

void DelayMs(uint32_t cnt)
{
	while(cnt--)
	{
		while(! time_1ms_dly_flag);
		time_1ms_dly_flag = 0;
	}
}


void TLI_Handle(void)
{
//	GPIO_WriteReverse(LED_1_PORT, (GPIO_Pin_TypeDef)LED_1_PINS);
//	GPIO_WriteReverse(MOTOR_PORT, (GPIO_Pin_TypeDef)MOTOR_PIN);
#if (MOTOR_CONTROL_MODE==MOTOR_CONTROL_MODE_1)

	if(onOff)
	{
		MotorOff();
		motorTimerCnt = 0;	//	Speed_DATA[motorSpeed];
	}
	else
	{
		MotorOff();
	}

#elif (MOTOR_CONTROL_MODE==MOTOR_CONTROL_MODE_2)

	if( onOff )
	{
		if( tmp >= Speed_DATA[motorSpeed] )
		{
			MotorOff();
		}
		else
		{
			MotorOn();
		}
		if(++tmp >= MOTOR_TIME_MAX)
		{
			tmp = 0;
		}
	}
	else
	{
		tmp = 0;
		MotorOff();
	}

#endif
}

void MotorOn(void)
{
	GPIO_WriteLow(MOTOR_PORT, MOTOR_PIN);
}

void MotorOff(void)
{
	GPIO_WriteHigh(MOTOR_PORT, MOTOR_PIN);
}


/**
  * @brief Delay
  * @param nCount
  * @retval None
  */
void Delay(uint16_t nCount)
{
  /* Decrement nCount value */
  while (nCount != 0)
  {
    nCount--;
  }
}

#ifndef DISABLE_UART3
/**
  * @brief Retargets the C library printf function to the UART.
  * @param c Character to send
  * @retval char Character sent
  */
PUTCHAR_PROTOTYPE
{
  /* Write a character to the UART3 */
  UART3_SendData8(c);
  /* Loop until the end of transmission */
  while (UART3_GetFlagStatus(UART3_FLAG_TXE) == RESET);

  return (c);
}

/**
  * @brief Retargets the C library scanf function to the USART.
  * @param None
  * @retval char Character to Read
  */
GETCHAR_PROTOTYPE
{
#ifdef _COSMIC_
  char c = 0;
#else
  int c = 0;
#endif
  /* Loop until the Read data register flag is SET */
  while (UART3_GetFlagStatus(UART3_FLAG_RXNE) == RESET);
    c = UART3_ReceiveData8();
  return (c);
}
#endif

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
