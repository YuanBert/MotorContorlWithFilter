/**
  ******************************************************************************
  * File Name          : BSP_Protocol.h
  * Description        : BSP_Protocol.h file
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
#ifndef __BSP_PROTOCOL_H
#define __BSP_PROTOCOL_H
#ifdef __cplusplus
 extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "main.h"
#include "stdint.h"

#define BSP_RX_LEN          512
   
/*******************************************************************************
** struct: sUsartReciveType
**
** DESCRIPTION:
**  --串口接收数据结构体
**
** CREATED: 2017/12/4, by Bert
**
** FILE: BSP_Protocol.h
**
** AUTHOR: Bert.Zhang
********************************************************************************
*/
struct sUsartReciveType
{
  uint8_t   RX_Flag:1;              //IDLE recive flag
  uint16_t  RX_Size;                //receive length
  uint8_t   RX_pData[BSP_RX_LEN];   //DMA receive buffer
};
   
/*******************************************************************************
** struct: sRequestCmd
**
** DESCRIPTION:
**  --this is the Request CMD about A9 and stm32
**
** CREATED: 2017/12/4, by Bert
**
** FILE: BSP_Protocol.h
**
** AUTHOR: Bert.Zhang
********************************************************************************
*/
struct sRequestCmd
{
  uint8_t       RequestCmdCode;
  uint8_t       RequestParam;
  uint8_t       Requestdata0;
  uint8_t       Requestdata1;
  uint8_t       Requestdata2;

  uint8_t       AckCmdCode;
  uint8_t       AckCode;
  
  uint8_t       ReciveOrSendFlag;           //接收或者发送标志位  
                                            // 0：空闲    1：接收    2：发送
  uint8_t       RevRequestFlag;               //接受Reqquest命令完成
  uint8_t       RevAckCmd;
  uint8_t       SendTimesCnt;               //发送次数计数
  uint8_t       ProErrorCode;               //错误代码
};
   

typedef struct sRequestCmd          REQCMD, * pREQCMD;
typedef struct sUsartReciveType     USARTRECIVETYPE,    * pUSARTRECIVETYPE;

typedef enum 
{
  BSP_OK       = 0x00U,
  BSP_ERROR    = 0x01U,
  BSP_BUSY     = 0x02U,
  BSP_TIMEOUT  = 0x03U
}BSP_StatusTypeDef;

BSP_StatusTypeDef BSP_SendRequestCmd(pREQCMD pRequestCmd);
BSP_StatusTypeDef BSP_AckRequestCmd(pREQCMD pRequestCmd);
BSP_StatusTypeDef BSP_SendAckCmd(pREQCMD pAckCmd);
void BSP_CheckRequestCmd(void);

   
#ifdef __cplusplus
}
#endif
#endif
/**********************END OF FILE*********************************************/


