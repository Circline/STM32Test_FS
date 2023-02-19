/*
 * @Description: 
 * @Version: 2.0
 * @Author: wuyue.nan
 * @Date: 2023-02-19 13:42:11
 * @LastEditors: wuyue.nan
 * @LastEditTime: 2023-02-19 23:27:59
 */
#include "W25X128.h"
#include "spi.h"


void SPI_FLASH_WaitForWriteEnd(void)
{
  uint8_t recv;
  uint8_t cmd = W25X_ReadStatusReg;
  SPI_FLASH_CS_LOW();

  do
  {
    HAL_SPI_TransmitReceive(&hspi2, &cmd, &recv, 1, 200);
  } while ((recv & WIP_Flag) == SET);
  SPI_FLASH_CS_HIGH();
}

void SPI_FLASH_WriteEnable(void)
{
  uint8_t cmd = W25X_WriteEnable;
  SPI_FLASH_CS_LOW();
  HAL_SPI_Transmit(&hspi2, &cmd, 1, 20);
  SPI_FLASH_CS_HIGH();
  SPI_FLASH_WaitForWriteEnd();
}

void SPI_FLASH_SectorErase(uint32_t SectorAddr)
{
  uint8_t cmd = W25X_SectorErase;
  SPI_FLASH_WriteEnable();

  SPI_FLASH_CS_LOW();

  HAL_SPI_Transmit(&hspi2, &cmd, 1, 20);
  cmd = (SectorAddr & 0xFF0000) >> 16;
  HAL_SPI_Transmit(&hspi2, &cmd, 1, 20);
  cmd = (SectorAddr & 0xFF00) >> 8;
  HAL_SPI_Transmit(&hspi2, &cmd, 1, 20);
  cmd = SectorAddr & 0xFF;
  HAL_SPI_Transmit(&hspi2, &cmd, 1, 20);

  SPI_FLASH_CS_HIGH();
  HAL_Delay(50);
  SPI_FLASH_WaitForWriteEnd();
}

void SPI_FLASH_BulkErase(void)
{
  uint8_t cmd = W25X_ChipErase;
  SPI_FLASH_WriteEnable();

  SPI_FLASH_CS_LOW();
  HAL_SPI_Transmit(&hspi2, &cmd, 1, 20);
  SPI_FLASH_CS_HIGH();

  SPI_FLASH_WaitForWriteEnd();
}

void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t cmd = W25X_PageProgram;
  SPI_FLASH_WriteEnable();
  SPI_FLASH_CS_LOW();

  HAL_SPI_Transmit(&hspi2, &cmd, 1, 20);
  cmd = (WriteAddr & 0xFF0000) >> 16;
  HAL_SPI_Transmit(&hspi2, &cmd, 1, 20);
  cmd = (WriteAddr & 0xFF00) >> 8;
  HAL_SPI_Transmit(&hspi2, &cmd, 1, 20);
  cmd = WriteAddr & 0xFF;
  HAL_SPI_Transmit(&hspi2, &cmd, 1, 20);

  if(NumByteToWrite > SPI_FLASH_PerWritePageSize)
  {
     NumByteToWrite = SPI_FLASH_PerWritePageSize;
  }
  HAL_SPI_Transmit(&hspi2, pBuffer, NumByteToWrite, 500);

  SPI_FLASH_CS_HIGH();
  SPI_FLASH_WaitForWriteEnd();
  HAL_Delay(50);
}

void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % SPI_FLASH_PageSize;
  count = SPI_FLASH_PageSize - Addr;	
  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

  SPI_Flash_WAKEUP();

  if (Addr == 0) {
    if (NumOfPage == 0) {
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
    } else {/* NumByteToWrite > SPI_FLASH_PageSize */
      while (NumOfPage--) {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
  } else {
		/* NumByteToWrite < SPI_FLASH_PageSize */
    if (NumOfPage == 0) {
      if (NumOfSingle > count) {
        temp = NumOfSingle - count;
				SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;
				
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
      } else {				
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
      }
    } else {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
      NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

      SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;
			
      while (NumOfPage--) {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }
      if (NumOfSingle != 0) {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
  uint8_t cmd = W25X_ReadData;
  SPI_FLASH_WriteEnable();
  HAL_Delay(10);
  SPI_FLASH_CS_LOW();

  HAL_SPI_Transmit(&hspi2, &cmd, 1, 20);
  cmd = (ReadAddr & 0xFF0000) >> 16;
  HAL_SPI_Transmit(&hspi2, &cmd, 1, 20);
  cmd = (ReadAddr & 0xFF00) >> 8;
  HAL_SPI_Transmit(&hspi2, &cmd, 1, 20);
  cmd = ReadAddr & 0xFF;
  HAL_SPI_Transmit(&hspi2, &cmd, 1, 20);

//  while (NumByteToRead--) {
//    HAL_SPI_Receive(&hspi2, pBuffer++, 1, 200);
//  }
  HAL_SPI_Receive(&hspi2, pBuffer, NumByteToRead, 200);
  
  SPI_FLASH_CS_HIGH();
}



uint32_t SPI_FLASH_ReadID(void)
{
  uint8_t cmd = W25X_JedecDeviceID;
  uint32_t tmp;

  SPI_FLASH_WriteEnable();
  SPI_FLASH_CS_LOW();
  HAL_SPI_Transmit(&hspi2, &cmd, 1, 20);
  HAL_SPI_Receive(&hspi2, (uint8_t *)&tmp, 3, 20);
  SPI_FLASH_CS_HIGH();

  tmp = BIG_2_LITTLE32(tmp);
  return tmp >> 8;
}

uint32_t SPI_FLASH_ReadDeviceID(void)
{
  uint32_t cmd = W25X_DeviceID;
  uint8_t tmp;
  
  SPI_FLASH_WriteEnable();
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();
  /* Send "RDID " instruction */
  HAL_SPI_Transmit(&hspi2, (uint8_t *)&cmd, 4, 50);  
  /* Read a byte from the FLASH */
  HAL_SPI_Receive(&hspi2, &tmp, 1, 20);
  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();

  return tmp;
}

void SPI_Flash_PowerDown(void)   
{ 
  uint8_t cmd = W25X_PowerDown;
  SPI_FLASH_WriteEnable();
  SPI_FLASH_CS_LOW();
  HAL_SPI_Transmit(&hspi2, &cmd, 1, 20);
  SPI_FLASH_CS_HIGH();
}

void SPI_Flash_WAKEUP(void)   
{
  uint16_t cnt = 300;
  uint8_t cmd = W25X_ReleasePowerDown;
  SPI_FLASH_WriteEnable();
  SPI_FLASH_CS_LOW();
  HAL_SPI_Transmit(&hspi2, &cmd, 1, 20);
  SPI_FLASH_CS_HIGH();
  while(cnt--);
  SPI_FLASH_WaitForWriteEnd();
}


