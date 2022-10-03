#include "stm32f0xx_hal.h"

//FLASHд�����ݲ���
void Flash_WriteData(uint32_t addr,uint16_t *data,uint16_t Size)
{
  //1������FLASH
  HAL_FLASH_Unlock();
  
  //2������FLASH
  //��ʼ��FLASH_EraseInitTypeDef
  FLASH_EraseInitTypeDef f;
  f.TypeErase = FLASH_TYPEERASE_PAGES;
  f.PageAddress = addr;
  f.NbPages = 1;
  //����PageError
  uint32_t PageError = 0;
  //���ò�������
  HAL_FLASHEx_Erase(&f, &PageError);
  
  //3����FLASH��д
  uint32_t TempBuf = 0;
  for(uint32_t i = 0;i< Size ;i++)
  {
    TempBuf = ~(*(data+i));
    TempBuf <<= 16;
    TempBuf += *(data+i); //ȡ��У��
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD , addr + i * 4, TempBuf);
  }
  
  //4����סFLASH
  HAL_FLASH_Lock();
}

//FLASH��ȡ���ݲ���,�ɹ�����1
uint8_t Flash_ReadData(uint32_t addr,uint16_t *data,uint16_t Size)
{
  uint32_t temp;
  uint8_t result = 1;
  for(uint32_t i = 0;i< Size ;i++)
  {
    temp = *(__IO uint32_t*)(addr + i * 4);
    if((uint16_t)temp == (uint16_t)(~(temp>>16)))
    {
      *(data+i) = (uint16_t)temp;
    }
    else
    {
      result = 0;
    }
  }
  return result;
}
