/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "TM1620.h"
#include "multi_button.h"
#include "delay.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
int numb = 0;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int fputc(int ch, FILE *f)
{
	/* 发送一个字节数据 */
	HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,0xffff);
	return ch;
}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
unsigned char buffer[2];
char array[4];
unsigned char volume = 4;
unsigned char page[3];
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Set_Fre(uint32_t fre)  //88MHz~108MHz  200KHz步进 
{
	uint16_t i;
	buffer[0]=0xf0;
	buffer[1]=0x09;
	if(HAL_I2C_Mem_Write(&hi2c1, 0x22, 0x02,I2C_MEMADD_SIZE_8BIT, buffer, 2, 20)==HAL_OK)
	{
	}
//	HAL_Delay(10);
	if((fre>=8700)&&(fre<=10800))
	{
		i = (fre -8700) /10;
		i = (i<<6)|0x10;
		buffer[0]=(i>>8);
		buffer[1]=i&0xff;
		HAL_I2C_Mem_Write(&hi2c1, 0x22, 0x03,I2C_MEMADD_SIZE_8BIT, buffer, 2, 20);
	}
	else
	{
		buffer[0]=0x00;
		buffer[1]=0x10;
		HAL_I2C_Mem_Write(&hi2c1, 0x22, 0x03,I2C_MEMADD_SIZE_8BIT, buffer, 2, 20);
	}

	buffer[0]=0x90;
	buffer[1]=0x80+volume-1;
	if(HAL_I2C_Mem_Write(&hi2c1, 0x22, 0x05,I2C_MEMADD_SIZE_8BIT, buffer, 2, 20)==HAL_OK)
	{

	}
}

void setvolume(unsigned char i)
{
	buffer[0]=0x90;
	buffer[1]=0x80+i-1;
	if(HAL_I2C_Mem_Write(&hi2c1, 0x22, 0x05,I2C_MEMADD_SIZE_8BIT, buffer, 2, 20)==HAL_OK)
	{

	}
}

void seekup()
{
	buffer[0]=0xC3;
	buffer[1]=0x09;
	HAL_I2C_Mem_Write(&hi2c1, 0x22, 0x02,I2C_MEMADD_SIZE_8BIT, buffer, 2, 20);
	HAL_Delay(5);
	buffer[0]=0xC2;
	buffer[1]=0x09;
	HAL_I2C_Mem_Write(&hi2c1, 0x22, 0x02,I2C_MEMADD_SIZE_8BIT, buffer, 2, 20);
	HAL_Delay(5);
}

void seekdown()
{
	buffer[0]=0xC1;
	buffer[1]=0x09;
	HAL_I2C_Mem_Write(&hi2c1, 0x22, 0x02,I2C_MEMADD_SIZE_8BIT, buffer, 2, 20);
	HAL_Delay(5);
	buffer[0]=0xC0;
	buffer[1]=0x09;
	HAL_I2C_Mem_Write(&hi2c1, 0x22, 0x02,I2C_MEMADD_SIZE_8BIT, buffer, 2, 20);
	HAL_Delay(5);
}

void setMono(unsigned char i)
{
	if(i==1)
	{
		buffer[0]=0xE2;
		buffer[1]=0x09;
		HAL_I2C_Mem_Write(&hi2c1, 0x22, 0x02,I2C_MEMADD_SIZE_8BIT, buffer, 2, 20);
	}
	else
	{
		buffer[0]=0xC2;
		buffer[1]=0x09;
		HAL_I2C_Mem_Write(&hi2c1, 0x22, 0x02,I2C_MEMADD_SIZE_8BIT, buffer, 2, 20);
	}
}

Button button1,button2;  //按钮控件

uint8_t read_button1_GPIO()
{
	return HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin);
}

uint8_t read_button2_GPIO()
{
	return HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin);
}

void BTN1_SINGLE_CLICK_Handler(void* btn)
{
	if(page[0] == 1)
	{
	  numb--;
	  if((numb<0))
	  {
		  numb = 210;
	  }
	  TM1620_Display(870+numb);
	  Set_Fre(8700+numb*10);
  }
	if(page[0] == 2)
	{
		volume--;
		if(volume <1)
		{
			volume =16;
		}
		TM1620_Display(volume);
		setvolume(volume);
	}
//	  printf("PRESS_UP");
	//do something...
}

void BTN1_DOUBLE_CLICK_Handler(void* btn)
{
//	printf("PRESS_UP");
	//do something...
	if(page[0] == 1)
	{
		page[0] = 2;
		TM1620_Display(volume);
	}
	else if(page[0] ==2)
	{
		page[0] = 1;
		TM1620_Display(870+numb);
	}
}

void BTN1_LONG_PRESS_HOLD_Handler(void* btn)
{
	delay_ms(60);
	if(page[0] == 1)
	{
		numb--;
		if((numb<0))
			{
				numb = 210;
			}
			TM1620_Display(870+numb);
			Set_Fre(8700+numb*10);
	}
	//do something...
}
	

void BTN2_SINGLE_CLICK_Handler(void* btn)
{
	if(page[0] == 1)
	{
		  numb++;
		  if((numb>210))
		  {
			  numb = 0;
		  }
		  TM1620_Display(870+numb);
		  Set_Fre(8700+numb*10);
	  }
			if(page[0] == 2)
	{
		volume++;
		if(volume >16)
		{ 
			volume = 1;
		}
		TM1620_Display(volume);
		setvolume(volume);
	}
	//do something...
}

void BTN2_DOUBLE_CLICK_Handler(void* btn)
{
//	printf("PRESS_UP");
	//do something...
}

void BTN2_LONG_PRESS_HOLD_Handler(void* btn)
{
	delay_ms(60);
	if(page[0] == 1)
	{
	  numb++;
	  if((numb>210))
	  {
		  numb = 0;
	  }
	  TM1620_Display(870+numb);
	  Set_Fre(8700+numb*10);
	//do something...
	 }

}

void Key_init()
{
	//全部初始化才能button_start
	button_init(&button1, read_button1_GPIO, 0);//低电平触发
	button_init(&button2, read_button2_GPIO, 0);//低电平触发

	
//	button_attach(&button1, PRESS_DOWN,       btn1_callback);
//	button_attach(&button1, PRESS_UP,         btn1_callback);
//	button_attach(&button1, PRESS_REPEAT,     btn1_callback);
	button_attach(&button1, SINGLE_CLICK,     BTN1_SINGLE_CLICK_Handler);
	button_attach(&button1, DOUBLE_CLICK,     BTN1_DOUBLE_CLICK_Handler);
//	button_attach(&button1, LONG_PRESS_START, btn1_callback);
	button_attach(&button1, LONG_PRESS_HOLD,  BTN1_LONG_PRESS_HOLD_Handler);

//	button_attach(&button2, PRESS_DOWN,       btn2_callback);
//	button_attach(&button2, PRESS_UP,         btn2_callback);
//	button_attach(&button2, PRESS_REPEAT,     btn2_callback);
	button_attach(&button2, SINGLE_CLICK,     BTN2_SINGLE_CLICK_Handler);
	button_attach(&button2, DOUBLE_CLICK,     BTN2_DOUBLE_CLICK_Handler);
//	button_attach(&button2, LONG_PRESS_START, btn2_callback);
	button_attach(&button2, LONG_PRESS_HOLD,  BTN2_LONG_PRESS_HOLD_Handler);
	
	button_start(&button1);
	button_start(&button2);
	
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	
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
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  TM1620_Init();
  setMono(1);
  Set_Fre(8700+numb*10);
  array[0] = (8700+numb)/1000;
  array[1] = ((8700+numb)%1000)/100;
  array[2] = ((8700+numb)%1000)/100;
  array[3] = (8700+numb)%10;
  page[0] = 1;
  TM1620_Display(870+numb);
  Key_init();
  delay_init(48);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//	  if((HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin)==GPIO_PIN_RESET)||(HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin)==GPIO_PIN_RESET))
//	  {
//		  HAL_Delay(10);
//		  {
//			  if(HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin)==GPIO_PIN_RESET)
//			  {
//				  numb--;
//				  if((numb<0))
//				  {
//					  numb = 210;
//				  }
//				  TM1620_Display(870+numb);
//				  Set_Fre(8700+numb*10);
//			  }
//			  else if(HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin)==GPIO_PIN_RESET)
//			  {
//				  numb++;
//				  if((numb>210))
//				  {
//					  numb = 0;
//				  }
//				  TM1620_Display(870+numb);
//				  Set_Fre(8700+numb*10);
//			  }
//		  }
//	  }
	  


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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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
