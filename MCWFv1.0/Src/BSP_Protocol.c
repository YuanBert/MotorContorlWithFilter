/**
  ******************************************************************************
  * File Name          : BSP_Protocol.c
  * Description        : BSP_Protocol.c file
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
#include "BSP_Protocol.h"
#include "BSP_Motor.h"
#include "usart.h"
#include "gpio.h"
#include "dma.h"

/* USER CODE BEGIN 0 */
USARTRECIVETYPE UsartType;
static uint8_t CmdBuffer[10];

/* USER CODE END 0 */
extern PROTOCOLCMD      gProtocolCmd;
extern UART_HandleTypeDef huart1;
//UART_HandleTypeDef huart2;
//UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;

BSP_StatusTypeDef BSP_ProtocolInit(void)
{
  BSP_StatusTypeDef state = BSP_OK;
  
  CmdBuffer[0] = 0x53;
  CmdBuffer[1] = 0x53;
  
  return state;
}


BSP_StatusTypeDef BSP_SendData(uint8_t *pData, uint16_t size,uint32_t Timeout)
{
  BSP_StatusTypeDef state = BSP_OK;
  state = (BSP_StatusTypeDef)HAL_UART_Transmit_DMA(&huart1,pData,size);
  return state;
}
  
  
void UsartReceive_IDLE(UART_HandleTypeDef *huart)  
{
    uint32_t temp;  
  
    if((__HAL_UART_GET_FLAG(huart,UART_FLAG_IDLE) != RESET))  
    {   
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);  
        HAL_UART_DMAStop(&huart1);  
        temp = huart1.hdmarx->Instance->CNDTR;  
        UsartType.RX_Size =  BSP_RX_LEN - temp;   
        UsartType.RX_Flag = 1;  
        HAL_UART_Receive_DMA(&huart1,UsartType.RX_pData,BSP_RX_LEN);  
    }  
}


BSP_StatusTypeDef BSP_SendRequestCmd(pPROTOCOLCMD pRequestCmd)
{
    BSP_StatusTypeDef state = BSP_OK;
    gProtocolCmd.HandingFlag = 1;
    
    if(gProtocolCmd.ReciveOrSendFlag)
    {
      
      CmdBuffer[2] = gProtocolCmd.RequestCmdCode;
      CmdBuffer[3] = gProtocolCmd.RequestParam;
      CmdBuffer[4] = gProtocolCmd.Requestdata0;
      CmdBuffer[5] = gProtocolCmd.Requestdata1;
      CmdBuffer[6] = gProtocolCmd.Requestdata2;
      CmdBuffer[7] = (uint8_t)(CmdBuffer[2] + CmdBuffer[3] + CmdBuffer[4] +\
                               CmdBuffer[5] + CmdBuffer[6]);
      CmdBuffer[8] = 0xAA;
      CmdBuffer[9] = 0xAA;
      
    }
    
    state = BSP_SendData(CmdBuffer,10,0xFFFF);
    if(BSP_OK == state)
    {
        gProtocolCmd.ReciveOrSendFlag = 1;
        gProtocolCmd.SendTimesCnt++;
    }
    
    return state;
}

BSP_StatusTypeDef BSP_AckRequestCmd(pPROTOCOLCMD pRequestCmd)
{
    BSP_StatusTypeDef state = BSP_OK;
    if(gProtocolCmd.RevRequestFlag)
    {
      //调用相关处理函数
      state = BSP_SendAckCmd(gProtocolCmd.AckCmdCode,gProtocolCmd.AckCode);
      if(BSP_OK == state)
      {
        gProtocolCmd.RevRequestFlag = 0;
        gProtocolCmd.HandingFlag = 0;
      }
    }
    
    
    return state;
}

BSP_StatusTypeDef BSP_SendAckCmd(uint8_t AckCmd,uint8_t AckCode)
{
    BSP_StatusTypeDef state = BSP_OK;
    CmdBuffer[2] = AckCmd;
    CmdBuffer[3] = AckCode;
    CmdBuffer[4] = (uint8_t)(AckCode + AckCmd);
    CmdBuffer[5] = 0xAA;
    CmdBuffer[6] = 0xAA;
    
    state = BSP_SendData(CmdBuffer,7,0xFFFF);
    
    return state;
}

void BSP_CheckRequestCmd(void)
{
  if(gProtocolCmd.RevEchoFlag || gProtocolCmd.SendTimesCnt > 4)
  {
    gProtocolCmd.ReciveOrSendFlag = 0;
    gProtocolCmd.RevEchoFlag = 0;
  }
}

////////////////////////////////////////////////////////////////////////////////
void BSP_HandingCmdWithTestVersion(void)
{
  if(gProtocolCmd.HandingFlag && gProtocolCmd.RevRequestFlag)
  {
    switch(gProtocolCmd.RequestCmdCode & 0xF0 )
    {
    case 0xC0:BSP_SendAckCmd(0xAC,0x01);break;
    case 0xD0:BSP_Running_Door();BSP_SendAckCmd(0xAD,0x01);break;
    case 0xE0:BSP_SendAckCmd(0xAE,0x01);break;
    case 0xB0:BSP_SendAckCmd(0xAB,0x01);break;
    case 0xF0:BSP_SendAckCmd(0xAF,0x01);break;
    default:break;
    }
    
    gProtocolCmd.HandingFlag = 0;
    gProtocolCmd.RevRequestFlag = 0;
  }
}
void BSP_HandingUartDataWithTestVersion(void)
{  
  uint8_t temp;
  if(UsartType.RX_Flag)
  {
    UsartType.RX_Flag = 0;
    if(0x53 != UsartType.RX_pData[0] || 0x53 != UsartType.RX_pData[1])
    {
      return;
    }
    if(0xA0 == (UsartType.RX_pData[2] & 0xF0))
    {
      temp = UsartType.RX_pData[2] + UsartType.RX_pData[3];
      if(temp != UsartType.RX_pData[4])
      {
        return;
      }
      
      if(0xAA != UsartType.RX_pData[5] || 0xAA != UsartType.RX_pData[6])
      {
        return;
      }
      gProtocolCmd.AckCmdCode = UsartType.RX_pData[2];
      gProtocolCmd.AckCode = UsartType.RX_pData[3];
      gProtocolCmd.RevEchoFlag = 1;
      return;
    }
    temp = (uint8_t)(UsartType.RX_pData[2] + UsartType.RX_pData[3] + UsartType.RX_pData[4]+\
      UsartType.RX_pData[5] + UsartType.RX_pData[6]);
    if(temp != UsartType.RX_pData[7])
    {
      return;
    }      
    if(0xAA != UsartType.RX_pData[8] || 0xAA != UsartType.RX_pData[9])
    {
      return;
    }
    gProtocolCmd.RequestCmdCode = UsartType.RX_pData[2];
    gProtocolCmd.RequestParam = UsartType.RX_pData[3];
    gProtocolCmd.Requestdata0 = UsartType.RX_pData[4];
    gProtocolCmd.Requestdata1 = UsartType.RX_pData[5];
    gProtocolCmd.Requestdata2 = UsartType.RX_pData[6];
    gProtocolCmd.HandingFlag = 1;
    gProtocolCmd.RevRequestFlag = 1;
  }
}

void BSP_TrySend5TimesCmd(void)
{
  if(gProtocolCmd.ReciveOrSendFlag)
  {
    if(gProtocolCmd.RevEchoFlag)
    {
        //检测相关信息
      gProtocolCmd.ReciveOrSendFlag = 0;
      gProtocolCmd.SendTimesCnt = 0;
      gProtocolCmd.RevEchoFlag = 0;
    }
    else
    {
      if(gProtocolCmd.SendTimesCnt > 5)
      {
        //超时错误
        gProtocolCmd.ReciveOrSendFlag = 0;
        gProtocolCmd.SendTimesCnt = 0;
        gProtocolCmd.RevEchoFlag = 0;
      }
      else
      {
        BSP_SendData(CmdBuffer,10,0xFFFF);
        gProtocolCmd.SendTimesCnt++;
      }
    }
  }
}
/**
  * @}
  */
/**
  * @}
  */
/*****************************END OF FILE**************************************/

