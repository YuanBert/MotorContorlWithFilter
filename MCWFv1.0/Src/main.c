/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "BSP_Motor.h"
#include "BSP_Protocol.h"
//#include "BSP_Motor.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
MOTORMACHINE gMotorMachine;
PROTOCOLCMD      gProtocolCmd;
extern USARTRECIVETYPE UsartType;


uint8_t gVerLastReadVal;
uint8_t gVerCurrentReadVal;
uint8_t gHorLastReadVal;
uint8_t gHorCurrentReadVal;
uint8_t gGentleSensorVal;
uint8_t gGentleSensorLastVal;
uint8_t gGentleSensorFlag;
uint8_t gTIM4CntUpFlag;
uint8_t gTIM5CntUpFlag;
uint8_t gTIM5SendCntUpFlag;

uint16_t gTIM4Cnt;
uint16_t gTIM5Cnt;
uint16_t gTIM5SendCnt;
uint16_t gGentleSensorCnt;
  




/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM4_Init();
  MX_USB_DEVICE_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  MX_TIM5_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();

  /* USER CODE BEGIN 2 */
  //HAL_TIM_Base_Start(&htim4);
  HAL_TIM_Base_Start_IT(&htim4);
  HAL_TIM_Base_Start_IT(&htim5);
  BSP_Motor_Init();
  BSP_ProtocolInit();
  HAL_UART_Receive_DMA(&huart1,UsartType.RX_pData,BSP_RX_LEN);
  __HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);   //使能串口空闲中断
  //BSP_Running_Door();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
    if(1 == gMotorMachine.StartFlag)
    {
      BSP_Running_Door();
      gMotorMachine.StartFlag = 0;
    }

    
    BSP_HandingCmdWithTestVersion();
    
    if(gGentleSensorFlag)
    {
      
      gProtocolCmd.RequestCmdCode = 0xC1;
      gProtocolCmd.RequestParam   = 0x01;
      gProtocolCmd.Requestdata0   = 0x00;
      gProtocolCmd.Requestdata1   = 0x00;
      gProtocolCmd.Requestdata2   = 0x00;
      BSP_SendRequestCmd(&gProtocolCmd);
      gGentleSensorFlag = 0;
    }
        
    if(gTIM5CntUpFlag)
    {
      gTIM5CntUpFlag = 0;
      BSP_HandingUartDataWithTestVersion();
      //调用应答函数来处
    }
    
    if(gTIM5SendCntUpFlag)
    {
      BSP_TrySend5TimesCmd();
      gTIM5SendCntUpFlag = 0;
    }
  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC|RCC_PERIPHCLK_USB;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/** NVIC Configuration
*/
static void MX_NVIC_Init(void)
{
  /* USB_LP_CAN1_RX0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
  /* USART1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
  /* TIM4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM4_IRQn);
  /* TIM5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM5_IRQn);
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

/* USER CODE BEGIN 4 */
/**
  * @brief  Period elapsed callback in non blocking mode 
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the __HAL_TIM_PeriodElapsedCallback could be implemented in the user file
   */
  
  /* 0.1ms */
  if(htim4.Instance == htim->Instance)
  {
    gVerCurrentReadVal = HAL_GPIO_ReadPin(VerRasterInput_GPIO_Port,VerRasterInput_Pin);
    gHorCurrentReadVal = HAL_GPIO_ReadPin(HorRasterInput_GPIO_Port,HorRasterInput_Pin);
    
    if(1 == gVerCurrentReadVal && 1 == gVerLastReadVal)
    {
      gMotorMachine.VerFilterCnt ++;
      if(gMotorMachine.VerFilterCnt > 20)       //高电平信号持续时间大于2MS,
      {
        gMotorMachine.VerticalRasterState = 1;  //到位停机
        gMotorMachine.VerFilterCnt = 0;
        if(UPDIR == gMotorMachine.RunDir)
        {
          gMotorMachine.RunDir = DOWNDIR;
          gMotorMachine.RuningState = 0;
          BSP_Motor_Stop();
        }
      }
    }
    else
    {
      gMotorMachine.VerticalRasterState = 0;
      gMotorMachine.VerFilterCnt = 0;
    }
    
    if(1 == gHorCurrentReadVal && 1 == gHorLastReadVal)
    {
      gMotorMachine.HorFilterCnt++;
      if(gMotorMachine.HorFilterCnt > 20)
      {
        gMotorMachine.HorizontalRasterState = 1;
        gMotorMachine.HorFilterCnt = 0;
        if(DOWNDIR == gMotorMachine.RunDir)
        {
          
          gMotorMachine.RunDir = UPDIR;
          gMotorMachine.RuningState = 0;
          BSP_Motor_Stop();
        }
      }
    }
    else
    {
      gMotorMachine.HorizontalRasterState = 0;
      gMotorMachine.HorFilterCnt = 0;
    }
    
    gVerLastReadVal = gVerCurrentReadVal;
    gHorLastReadVal = gHorCurrentReadVal;
    
      
    gTIM4Cnt++;
    
    if(gTIM4Cnt > 60000)
    {
      gTIM4Cnt = 0;
      gTIM4CntUpFlag = 1;
      gMotorMachine.StartFlag = 1;
    }
  }
  
  if(htim5.Instance == htim->Instance)
  { 
    
    gGentleSensorVal   = HAL_GPIO_ReadPin(GentleSensor_GPIO_Port,GentleSensor_Pin);
    
    if(0 == gGentleSensorVal && 0 == gGentleSensorLastVal)
    {
      if(0 == gMotorMachine.GentleSensorFlag)
      {
        gGentleSensorCnt ++;
        if(gGentleSensorCnt > 50 && 0 == gGentleSensorFlag)
        {
          gGentleSensorFlag = 1;
          gGentleSensorCnt = 0;
          gMotorMachine.GentleSensorFlag = 1;
        }
      }
    }
    else
    {
      gMotorMachine.GentleSensorFlag = 0;
      gGentleSensorFlag = 0;
      gGentleSensorCnt = 0;
    }
    gGentleSensorLastVal = gGentleSensorVal;
    
    gTIM5Cnt++;
    if(gTIM5Cnt > 50)
    {
      gTIM5Cnt = 0;
      gTIM5CntUpFlag = 1;
    } 
    
    gTIM5SendCnt ++;
    if(gTIM5SendCnt > 5000)
    {
      gTIM5SendCntUpFlag = 1;
      gTIM5SendCnt = 0;
    }
    
  }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
