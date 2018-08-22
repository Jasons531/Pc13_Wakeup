/*
 * user_standby.c	开关机驱动文件
*/

#include "user_standby.h"

extern UART_HandleTypeDef huart1;

/**
  * StandbyInit: 初始化
  *	此处只是初始化串口，打印用
  */
void StandbyInit(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();	//串口时钟使能
	PWR_KEY_RCC_CLK_ENABLE(); 		//引脚时钟使能
		
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
 * 退出休眠模式
*/
void StandbyExitMode(void)
{
	/* 定义IO硬件初始化结构体变量 */
	GPIO_InitTypeDef GPIO_InitStruct;

	/* 使能(开启)KEY引脚对应IO端口时钟 */  
	PWR_KEY_RCC_CLK_ENABLE(); 

	/* 配置KEY2 GPIO:中断模式，下降沿触发 */
	GPIO_InitStruct.Pin = PWR_KEY_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING; // 特别注意这里要使用中断模式,下拉，上升沿触发
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(PWR_KEY_GPIO, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(PWR_KEY_EXTI_IRQn, 1, 1);	//此处的中断优先级不能比系统时钟的优先级高，否则按键消抖就用不了了
	HAL_NVIC_EnableIRQ(PWR_KEY_EXTI_IRQn);			//中断使用
}

/**
  * 函数功能: 用于检测按键是否被长时间按下
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：1 ：按键被长时间按下  0 ：按键没有被长时间按下
  */
uint8_t StandbyCheckPwrkey(void)
{			
	uint8_t downCnt =0;																//记录按下的次数
	uint8_t upCnt =0;																//记录松开的次数			

	while(1)																		//死循环，由return结束
	{	
		HAL_Delay(30);																//延迟一段时间再检测
		if(HAL_GPIO_ReadPin(PWR_KEY_GPIO,PWR_KEY_PIN)==PWR_KEY_DOWN_LEVEL)			//检测到按下按键
		{
			
			downCnt++;																//记录按下次数
			upCnt=0;																//清除按键释放记录
			if(downCnt>=100)														//按下时间足够
			{
				printf("长按电源按钮\r\n");
				return 1; 															//检测到按键被时间长按下
			}
		}
		else 
		{
			upCnt++; 																//记录释放次数
			if(upCnt>5)																//连续检测到释放超过5次
			{
				printf("按下时间不足\r\n");
				return 0;															//按下时间太短，不是按键长按操作
			}
		}
	}

}
/*以下函数可放在其它文件实现*/
/**
*若有其它外部中断，应将下面的函数移到stm32l0xx_it.c中，方便以后管理
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
//  * 函数功能: 按键外部中断服务函数
//  * 输入参数: GPIO_Pin：中断引脚
//  * 返 回 值: 无
//  * 说    明: 无
//  */
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//  if(GPIO_Pin==PWR_KEY_PIN)
//  {
//    printf("\n EXTI13中断 \n");	
//   if(PWR_Check_Standby())//此时表示关机
//   {
//      /* 禁用所有唤醒源: 唤醒引脚PA0 */
//      HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);

//      /* 清除所有唤醒标志位 */
//      __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
//      
//      /* 使能唤醒引脚：PA0做为系统唤醒输入 */
////      HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

//	/* 使能唤醒引脚：PC13做为系统唤醒输入 */
//      HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2);

//      /* 进入待机模式 */
//      HAL_PWR_EnterSTANDBYMode();
//   }
//  }
//}

/**
  * 进入休眠模式(关机)
  */
void StandbyEnterMode(void)
{
	/* 禁用唤醒源:唤醒引脚PA0 由于PA0接在水流传感器上，为避免意外，将其禁用 */
	HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);

	/* 清除所有唤醒标志位 */
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

	/* 使能唤醒引脚：PC13做为系统唤醒输入 */
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2);

	/* 进入待机模式 */
	HAL_PWR_EnterSTANDBYMode();
}

///**
//  * 停机唤醒后配置系统时钟: 使能 HSE, PLL，并且选择PLL作为系统时钟.休眠无需此函数
//  */
//void SYSCLKConfig_STOP(void)//与SystemClock_Config一致，直接复制
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