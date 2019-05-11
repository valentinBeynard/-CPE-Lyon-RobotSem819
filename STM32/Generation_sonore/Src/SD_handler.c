#include "SD_handler.h"
#include "stm32f3xx_spi_sd.h"
#include "stm32f3xx_hal.h"

/**
  * @brief  DeInitializes the SPI interface.
  * @param  None
  * @retval None
  */
void SD_LowLevel_DeInit(void)
{
//  GPIO_InitTypeDef  GPIO_InitStructure;
//  
//  SPI_Cmd(SD_SPI, DISABLE); /*!< SD_SPI disable */
//  SPI_I2S_DeInit(SD_SPI);   /*!< DeInitializes the SD_SPI */
//  
//  /*!< SD_SPI Periph clock disable */
//  RCC_APB1PeriphClockCmd(SD_SPI_CLK, DISABLE); 

//  /*!< Configure SD_SPI pins: SCK */
//  GPIO_InitStructure.Pin = SD_SPI_SCK_PIN;
//  GPIO_InitStructure.Mode = GPIO_Mode_IN;
//  GPIO_InitStructure.Pull = GPIO_PuPd_NOPULL;
//  GPIO_Init(SD_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

//  /*!< Configure SD_SPI pins: MISO */
//  GPIO_InitStructure.GPIO_Pin = SD_SPI_MISO_PIN;
//  GPIO_Init(SD_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

//  /*!< Configure SD_SPI pins: MOSI */
//  GPIO_InitStructure.GPIO_Pin = SD_SPI_MOSI_PIN;
//  GPIO_Init(SD_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

//  /*!< Configure SD_SPI_CS_PIN pin: SD Card CS pin */
//  GPIO_InitStructure.GPIO_Pin = SD_CS_PIN;
//  GPIO_Init(SD_CS_GPIO_PORT, &GPIO_InitStructure);

//  /*!< Configure SD_SPI_DETECT_PIN pin: SD Card detect pin */
//  GPIO_InitStructure.GPIO_Pin = SD_DETECT_PIN;
//  GPIO_Init(SD_DETECT_GPIO_PORT, &GPIO_InitStructure);
}

/**
  * @brief  Initializes the SD Card and put it into StandBy State (Ready for 
  *         data transfer).
  * @param  None
  * @retval None
  */
void SD_LowLevel_Init(void)
{
	
}