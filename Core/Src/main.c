/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "W25X128.h"
#include "ff.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
FATFS fs; /* FatFs 文件系统对象 */
FIL fnew; /* 文件对象 */
FRESULT res_flash; /* 文件操作结果 */
UINT fnum; /* 文件成功读写数量 */
BYTE buffer[1024]= {0}; /* 读缓冲区 */
/* 写缓冲区*/
BYTE textFileBuffer[] = "ABCDEFGHIJKLMN\r\nabcdefghijklmn\r\n";
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  int cnt = 0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

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
  MX_USART1_UART_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
  printf("Init OK!\n");
  printf("Test: SPI FLASH FileSystem FatFs.\n");
  res_flash = f_mount(&fs,"1:",1);
  if (res_flash == FR_NO_FILESYSTEM) {
    printf("》FLASH 还没有文件系统，即将进行格式化...\r\n");
    /* 格式化 */
    res_flash=f_mkfs("1:",0,0);
    
    if (res_flash == FR_OK)
    {
      printf("》FLASH 已成功格式化文件系统。\r\n");
      /* 格式化后，先取消挂载 */
      res_flash = f_mount(NULL,"1:",1);
      /* 重新挂载 */
      res_flash = f_mount(&fs,"1:",1);
    } else {
      printf("《《格式化失败。》》\r\n");
      while (1);
    }
  } else if (res_flash!=FR_OK) {
    printf("！！外部Flash 挂载文件系统失败。(%d)\r\n",res_flash);
    printf("！！可能原因：SPI Flash 初始化不成功。\r\n");
    while (1);
  } else {
    printf("》文件系统挂载成功，可以进行读写测试\r\n");
  }

  /*----------------------- 文件系统测试：写测试 -------------------*/
  /* 打开文件，每次都以新的形式打开，属性为可写 */
  printf("\r\n****** 即将进行文件写入测试... ******\r\n");
  res_flash = f_open(&fnew, "1:FatFs 读写测试文件.txt", FA_CREATE_ALWAYS | FA_WRITE );
  if ( res_flash == FR_OK ) {
    printf("》打开/创建FatFs 读写测试文件.txt 文件成功，向文件写入数据。\r\n");
    /* 将指定存储区内容写入到文件内 */
    res_flash=f_write(&fnew, textFileBuffer, sizeof(textFileBuffer), &fnum);
    if (res_flash==FR_OK) {
    printf("》文件写入成功，写入字节数据：%d\n", fnum);
    printf("》向文件写入的数据为：\r\n%s\r\n", textFileBuffer);
    } else {
    printf("！！文件写入失败：(%d)\n", res_flash);
    }
    /* 不再读写，关闭文件 */
    f_close(&fnew);
  } else {
    printf("！！打开/创建文件失败。\r\n");
  }
  
  /*------------------- 文件系统测试：读测试 --------------------------*/
  printf("****** 即将进行文件读取测试... ******\r\n");
  res_flash = f_open(&fnew, "1:FatFs 读写测试文件.txt",
  FA_OPEN_EXISTING | FA_READ);
  if (res_flash == FR_OK) {
    printf("》打开文件成功。\r\n");
    res_flash = f_read(&fnew, buffer, sizeof(buffer), &fnum);
    if (res_flash==FR_OK) {
      printf("》文件读取成功,读到字节数据：%d\r\n",fnum);
      printf("》读取得的文件数据为：\r\n%s \r\n", buffer);
    } else {
      printf("！！文件读取失败：(%d)\n",res_flash);
    }
  } else {
    printf("！！打开文件失败。\r\n");
  }
  /* 不再读写，关闭文件 */
  f_close(&fnew);
  
  /* 不再使用文件系统，取消挂载文件系统 */
  f_mount(NULL,"1:",1);  
  
  
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
  while (1)
  {
    
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if (cnt >= 50) {
      cnt = 0;
      HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
      HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
    }
    cnt++;
    HAL_Delay(10);
        
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
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
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
