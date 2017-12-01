/**
  ******************************************************************************
  * File Name          : BSP_Motor.h
  * Description        : This file contains all the functions prototypes for 
  *                      the BSP_Motor  
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_MOTOR_H
#define __BSP_MOTOR_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "main.h" 

/* USER CODE BEGIN  defines */
#define UPDIR                   0
#define DOWNDIR                 1
/* USER CODE END  defines */
   
/* USER CODE BEGIN Prototypes */
   
   enum e_MOTOR_ERROR_CODE{
        Motor_OK = 0,
        Motor_RunErr = 1,
        Motor_VerRasterErr = 2,
        Motor_HorRasterErr = 3,
        Motor_OtherErr = 4
   };
   
   typedef enum e_MOTOR_ERROR_CODE MOTOR_ERROR_CODE;
 
   typedef struct s_MotorMachine{
     uint8_t     VerticalRasterState;
     uint8_t     HorizontalRasterState;
     uint8_t     RuningState;
     uint8_t     RunDir;
     uint16_t    SetRunSpeed;
     uint16_t    CurrentRunSpeed;
     uint8_t     StartFlag;
     uint8_t     VerFilterCnt;
     uint8_t     HorFilterCnt;
     uint8_t     Motor_Error;
     uint8_t     EncounteredFlag;           //Óö×è±ê¼ÇÎ»
   }MOTORMACHINE,*pMOTORMACHINE;
   

/* USER CODE END Prototypes */

void BSP_Motor_Init(void);

void BSP_Motor_Running(uint8_t nDir);

void BSP_Motor_Stop(void);

void BSP_Motor_Start(void);

void BSP_Running_Door(void);

void BSP_Checking_Err(void);
   
#ifdef __cplusplus
}
#endif
#endif

/***************************************************************END OF FILE****/
