/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*                                                                       */
/*   Portions COPYRIGHT 2017 STMicroelectronics                          */
/*   Portions Copyright (C) 2014, ChaN, all right reserved               */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

/**
  ******************************************************************************
  * @file    diskio.c 
  * @author  MCD Application Team
  * @version V1.4.1
  * @date    14-February-2017
  * @brief   FatFs low level disk I/O module.
  ******************************************************************************
  * @attention
  *
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
#include "diskio.h"
#include "ff_gen_drv.h"
#include "W25X128.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ATA 0
#define SPI_FLASH 1
/* Private variables ---------------------------------------------------------*/
extern Disk_drvTypeDef  disk;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Gets Disk Status 
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
  DSTATUS status = STA_NOINIT;

  switch (pdrv) {
    case ATA: /* SD CARD */
      break;
    case SPI_FLASH:
      /* SPI Flash 状态检测：读取SPI Flash 设备ID */
      if (FLASH_ID == SPI_FLASH_ReadID()) {
        /* 设备ID 读取结果正确 */
        status &= ~STA_NOINIT;
      } else {
        /* 设备ID 读取结果错误 */
        status = STA_NOINIT;;
      }
      break;
    default:
      status = STA_NOINIT;
  }
  return status;
}

/**
  * @brief  Initializes a Drive
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
  DSTATUS status = STA_NOINIT;
  switch (pdrv) {
    case ATA: /* SD CARD */
    break;
  case SPI_FLASH: /* SPI Flash */
    /* 唤醒SPI Flash */
    SPI_Flash_WAKEUP();
    /* 获取SPI Flash 芯片状态 */
    status=disk_status(SPI_FLASH);
    break;
  default:
    status = STA_NOINIT;
  }
  return status;
}

/**
  * @brief  Reads Sector(s) 
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	        /* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
DRESULT status = RES_PARERR;
  switch (pdrv) {
    case ATA: /* SD CARD */
      break;
    
    case SPI_FLASH:
      SPI_FLASH_BufferRead(buff, sector <<12, count<<12);
      status = RES_OK;
      break;
    
    default:
      status = RES_PARERR;
  }
    return status;
}

/**
  * @brief  Writes Sector(s)  
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */
#if _USE_WRITE == 1
DRESULT disk_write (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count        	/* Number of sectors to write */
)
{
  uint32_t write_addr;
  DRESULT status = RES_PARERR;
  if (!count) {
    return RES_PARERR; /* Check parameter */
  }
  
  switch (pdrv) {
    case ATA: /* SD CARD */
    break;
  
  case SPI_FLASH:
    write_addr = sector<<12;
    SPI_Flash_WAKEUP();
    SPI_FLASH_SectorErase(write_addr);
    if ((buff[0] == 'A') && (buff[1] == 'B') && (buff[2] == 'C') && (buff[3] == 'D')) {
      printf("writeAddr: %#X, sector: %#X.\n", write_addr, sector);
    }
    SPI_FLASH_BufferWrite((uint8_t *)buff, write_addr, count<<12);
    status = RES_OK;
    break;
  
  default:
    status = RES_PARERR;
  }
  return status;
}
#endif /* _USE_WRITE == 1 */

/**
  * @brief  I/O control operation  
  * @param  pdrv: Physical drive number (0..)
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
#if _USE_IOCTL == 1
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
  DRESULT status = RES_PARERR;
  switch (pdrv) {
    case ATA: /* SD CARD */
      break;
    case SPI_FLASH:
      switch (cmd) {
        /* 扇区数量：4096*4096/1024/1024=16(MB) */
        case GET_SECTOR_COUNT:
          *(DWORD * )buff = 1024;
          break;
        /* 扇区大小 */
        case GET_SECTOR_SIZE :
          *(WORD * )buff = 4096;
          break;
        /* 同时擦除扇区个数 */
        case GET_BLOCK_SIZE :
          *(DWORD * )buff = 1;
          break;
      }
      status = RES_OK;
      break;
    
    default:
      status = RES_PARERR;
    }
    return status;
}
#endif /* _USE_IOCTL == 1 */

/**
  * @brief  Gets Time from RTC 
  * @param  None
  * @retval Time in DWORD
  */
__weak DWORD get_fattime (void)
{
  /* 返回当前时间戳 */
  return ((DWORD)(2023 - 1980) << 25) /* Year 2015 */
         | ((DWORD)2 << 21) /* Month 1 */
         | ((DWORD)19 << 16) /* Mday 1 */
         | ((DWORD)0 << 11) /* Hour 0 */
         | ((DWORD)0 << 5) /* Min 0 */
         | ((DWORD)0 >> 1); /* Sec 0 */
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

