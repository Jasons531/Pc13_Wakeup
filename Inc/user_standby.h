/*
 * user_standby.h	���ػ�����ͷ�ļ�
*/

#ifndef __USER_STANDBY_H__
#define __USER_STANDBY_H__

#include "stm32l0xx_hal.h"
/* ���Ͷ��� --------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/
#define PWR_KEY_RCC_CLK_ENABLE           __HAL_RCC_GPIOC_CLK_ENABLE
#define PWR_KEY_PIN                 	   GPIO_PIN_13
#define PWR_KEY_GPIO                     GPIOC
#define PWR_KEY_DOWN_LEVEL               SET  /* ��Դ��������ʱ����Ϊ�ߵ�ƽ��������������ΪSET */
#define PWR_KEY_EXTI_IRQHandler          EXTI4_15_IRQHandler
#define PWR_KEY_EXTI_IRQn                EXTI4_15_IRQn

/* ��չ���� ------------------------------------------------------------------*/
/* �������� ------------------------------------------------------------------*/
void StandbyInit(void);
uint8_t StandbyCheckPwrkey(void);
void StandbyEnterMode(void);
void StandbyExitMode(void);
void SYSCLKConfig_STOP(void);
#endif /* __USER_STANDBY_H__ */
