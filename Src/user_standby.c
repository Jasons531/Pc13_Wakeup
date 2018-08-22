/*
 * user_standby.c	���ػ������ļ�
*/

#include "user_standby.h"

extern UART_HandleTypeDef huart1;

/**
  * StandbyInit: ��ʼ��
  *	�˴�ֻ�ǳ�ʼ�����ڣ���ӡ��
  */
void StandbyInit(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();	//����ʱ��ʹ��
	PWR_KEY_RCC_CLK_ENABLE(); 		//����ʱ��ʹ��
		
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
//	_Error_Handler(__FILE__, __LINE__);
	}
}
/*
 * �˳�����ģʽ
*/
void StandbyExitMode(void)
{
	/* ����IOӲ����ʼ���ṹ����� */
	GPIO_InitTypeDef GPIO_InitStruct;

	/* ʹ��(����)KEY���Ŷ�ӦIO�˿�ʱ�� */  
	PWR_KEY_RCC_CLK_ENABLE(); 

	/* ����KEY2 GPIO:�ж�ģʽ���½��ش��� */
	GPIO_InitStruct.Pin = PWR_KEY_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING; // �ر�ע������Ҫʹ���ж�ģʽ,�����������ش���
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(PWR_KEY_GPIO, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(PWR_KEY_EXTI_IRQn, 1, 1);	//�˴����ж����ȼ����ܱ�ϵͳʱ�ӵ����ȼ��ߣ����򰴼��������ò�����
	HAL_NVIC_EnableIRQ(PWR_KEY_EXTI_IRQn);			//�ж�ʹ��
}

/**
  * ��������: ���ڼ�ⰴ���Ƿ񱻳�ʱ�䰴��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ����1 ����������ʱ�䰴��  0 ������û�б���ʱ�䰴��
  */
uint8_t StandbyCheckPwrkey(void)
{			
	uint8_t downCnt =0;																//��¼���µĴ���
	uint8_t upCnt =0;																//��¼�ɿ��Ĵ���			

	while(1)																		//��ѭ������return����
	{	
		HAL_Delay(30);																//�ӳ�һ��ʱ���ټ��
		if(HAL_GPIO_ReadPin(PWR_KEY_GPIO,PWR_KEY_PIN)==PWR_KEY_DOWN_LEVEL)			//��⵽���°���
		{
			
			downCnt++;																//��¼���´���
			upCnt=0;																//��������ͷż�¼
			if(downCnt>=100)														//����ʱ���㹻
			{
				printf("������Դ��ť\r\n");
				return 1; 															//��⵽������ʱ�䳤����
			}
		}
		else 
		{
			upCnt++; 																//��¼�ͷŴ���
			if(upCnt>5)																//������⵽�ͷų���5��
			{
				printf("����ʱ�䲻��\r\n");
				return 0;															//����ʱ��̫�̣����ǰ�����������
			}
		}
	}

}
/*���º����ɷ��������ļ�ʵ��*/
/**
*���������ⲿ�жϣ�Ӧ������ĺ����Ƶ�stm32l0xx_it.c�У������Ժ����
*/
void EXTI4_15_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI4_15_IRQn 0 */

  /* USER CODE END EXTI4_15_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
  /* USER CODE BEGIN EXTI4_15_IRQn 1 */

  /* USER CODE END EXTI4_15_IRQn 1 */
}

///**
//  * ��������: �����ⲿ�жϷ�����
//  * �������: GPIO_Pin���ж�����
//  * �� �� ֵ: ��
//  * ˵    ��: ��
//  */
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//  if(GPIO_Pin==PWR_KEY_PIN)
//  {
//    printf("\n EXTI13�ж� \n");	
//   if(PWR_Check_Standby())//��ʱ��ʾ�ػ�
//   {
//      /* �������л���Դ: ��������PA0 */
//      HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);

//      /* ������л��ѱ�־λ */
//      __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
//      
//      /* ʹ�ܻ������ţ�PA0��Ϊϵͳ�������� */
////      HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

//	/* ʹ�ܻ������ţ�PC13��Ϊϵͳ�������� */
//      HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2);

//      /* �������ģʽ */
//      HAL_PWR_EnterSTANDBYMode();
//   }
//  }
//}

/**
  * ��������ģʽ(�ػ�)
  */
void StandbyEnterMode(void)
{
	/* ���û���Դ:��������PA0 ����PA0����ˮ���������ϣ�Ϊ�������⣬������� */
	HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);

	/* ������л��ѱ�־λ */
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

	/* ʹ�ܻ������ţ�PC13��Ϊϵͳ�������� */
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2);

	/* �������ģʽ */
	HAL_PWR_EnterSTANDBYMode();
}

///**
//  * ͣ�����Ѻ�����ϵͳʱ��: ʹ�� HSE, PLL������ѡ��PLL��Ϊϵͳʱ��.��������˺���
//  */
//void SYSCLKConfig_STOP(void)//��SystemClock_Configһ�£�ֱ�Ӹ���
//{
//	RCC_OscInitTypeDef RCC_OscInitStruct;
//  RCC_ClkInitTypeDef RCC_ClkInitStruct;
//  RCC_PeriphCLKInitTypeDef PeriphClkInit;

//    /**Configure the main internal regulator output voltage 
//    */
//  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

//    /**Configure LSE Drive Capability 
//    */
//  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

//    /**Initializes the CPU, AHB and APB busses clocks 
//    */
//  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
//  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
//  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
//  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
//  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
//  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_8;
//  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
//  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }

//    /**Initializes the CPU, AHB and APB busses clocks 
//    */
//  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
//                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
//  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
//  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
//  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
//  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

//  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }

//  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_LPUART1
//                              |RCC_PERIPHCLK_RTC;
//  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
//  PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
//  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
//  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }

//    /**Configure the Systick interrupt time 
//    */
//  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

//    /**Configure the Systick 
//    */
//  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

//  /* SysTick_IRQn interrupt configuration */
//  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
//}