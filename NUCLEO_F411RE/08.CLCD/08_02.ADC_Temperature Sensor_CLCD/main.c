/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define delay_ms HAL_Delay
#define ADDRESS   0x27 << 1

#define RS1_EN1   0x05
#define RS1_EN0   0x01
#define RS0_EN1   0x04
#define RS0_EN0   0x00
#define BackLight 0x08
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
const float AVG_SLOPE = 4.3E-03;
const float V25       = 1.43;
const float ADC_TO_VOLT = 3.3 / 4096.0;

uint16_t adc1;
float vSense;
float temp;
char lcd_buf[17];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);

/* USER CODE BEGIN PFP */
void I2C_ScanAddresses(void);
void delay_us(int us);
void LCD_DATA(uint8_t data);
void LCD_CMD(uint8_t cmd);
void LCD_CMD_4bit(uint8_t cmd);
void LCD_INIT(void);
void LCD_XY(char x, char y);
void LCD_CLEAR(void);
void LCD_PUTS(char *str);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#ifdef __GNUC__
int __io_putchar(int ch)
#else
int fputc(int ch, FILE *f)
#endif
{
    if (ch == '\n')
        HAL_UART_Transmit(&huart2, (uint8_t*)"\r", 1, 0xFFFF);
    HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, 0xFFFF);
    return ch;
}

void I2C_ScanAddresses(void) {
    HAL_StatusTypeDef result;
    uint8_t i;
    printf("Scanning I2C addresses...\r\n");
    for (i = 1; i < 128; i++) {
        result = HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(i << 1), 1, 10);
        if (result == HAL_OK) {
            printf("I2C device found at address 0x%02X\r\n", i);
        }
    }
    printf("Scan complete.\r\n");
}

void delay_us(int us){
    volatile int cnt = us * 3;
    while(cnt--);
}

void LCD_DATA(uint8_t data) {
    uint8_t temp;
    temp = (data & 0xF0) | RS1_EN1 | BackLight;
    HAL_I2C_Master_Transmit(&hi2c1, ADDRESS, &temp, 1, 100);
    temp = (data & 0xF0) | RS1_EN0 | BackLight;
    HAL_I2C_Master_Transmit(&hi2c1, ADDRESS, &temp, 1, 100);
    temp = ((data << 4) & 0xF0) | RS1_EN1 | BackLight;
    HAL_I2C_Master_Transmit(&hi2c1, ADDRESS, &temp, 1, 100);
    temp = ((data << 4) & 0xF0) | RS1_EN0 | BackLight;
    HAL_I2C_Master_Transmit(&hi2c1, ADDRESS, &temp, 1, 100);
    delay_us(50);
}

void LCD_CMD(uint8_t cmd) {
    uint8_t temp;
    temp = (cmd & 0xF0) | RS0_EN1 | BackLight;
    HAL_I2C_Master_Transmit(&hi2c1, ADDRESS, &temp, 1, 100);
    temp = (cmd & 0xF0) | RS0_EN0 | BackLight;
    HAL_I2C_Master_Transmit(&hi2c1, ADDRESS, &temp, 1, 100);
    temp = ((cmd << 4) & 0xF0) | RS0_EN1 | BackLight;
    HAL_I2C_Master_Transmit(&hi2c1, ADDRESS, &temp, 1, 100);
    temp = ((cmd << 4) & 0xF0) | RS0_EN0 | BackLight;
    HAL_I2C_Master_Transmit(&hi2c1, ADDRESS, &temp, 1, 100);
    delay_us(50);
}

void LCD_CMD_4bit(uint8_t cmd) {
    uint8_t temp;
    temp = ((cmd << 4) & 0xF0) | RS0_EN1 | BackLight;
    HAL_I2C_Master_Transmit(&hi2c1, ADDRESS, &temp, 1, 100);
    temp = ((cmd << 4) & 0xF0) | RS0_EN0 | BackLight;
    HAL_I2C_Master_Transmit(&hi2c1, ADDRESS, &temp, 1, 100);
    delay_us(50);
}

void LCD_INIT(void) {
    delay_ms(100);
    LCD_CMD_4bit(0x03); delay_ms(5);
    LCD_CMD_4bit(0x03); delay_us(100);
    LCD_CMD_4bit(0x03); delay_us(100);
    LCD_CMD_4bit(0x02); delay_us(100);
    LCD_CMD(0x28);
    LCD_CMD(0x0C);
    LCD_CMD(0x01);
    delay_ms(2);
}

void LCD_XY(char x, char y) {
    if (y == 0) LCD_CMD(0x80 + x);
    else LCD_CMD(0xC0 + x);
}

void LCD_CLEAR(void) {
    LCD_CMD(0x01);
    delay_ms(2);
}

void LCD_PUTS(char *str) {
    while (*str) LCD_DATA(*str++);
}

// 온도계 커스텀 문자 (5x8 픽셀)
const uint8_t thermometer[8] = {
    0b00100,
    0b01010,
    0b01010,
    0b01010,
    0b01110,
    0b11111,
    0b11111,
    0b01110
};

/**
 * @brief CGRAM에 커스텀 문자 등록
 * @param location: 문자 번호 (0~7)
 * @param pattern: 8바이트 패턴 배열
 */
void LCD_CreateChar(uint8_t location, const uint8_t *pattern) {
    if (location > 7) return;
    LCD_CMD(0x40 | (location << 3));
    for (int i = 0; i < 8; i++) {
        LCD_DATA(pattern[i]);
    }
    LCD_CMD(0x80);
}

/**
 * @brief 커스텀 문자 출력
 * @param location: 등록된 문자 번호 (0~7)
 */
void LCD_PutCustomChar(uint8_t location) {
    if (location > 7) return;
    LCD_DATA(location);
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
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_ADC1_Init();

  /* USER CODE BEGIN 2 */
  I2C_ScanAddresses();

  LCD_INIT();

  // 온도계 커스텀 문자 등록 (위치 0번에 저장)
  LCD_CreateChar(0, thermometer);

  LCD_XY(0, 0); LCD_PUTS((char *)"STM32 Temp");
  LCD_XY(0, 1); LCD_PUTS((char *)"Monitor v1.0");
  HAL_Delay(2000);
  LCD_CLEAR();

  /* Start calibration*/
  if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /* Start the conversion process*/
  if (HAL_ADC_Start(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    HAL_ADC_PollForConversion(&hadc1, 100);
    adc1 = HAL_ADC_GetValue(&hadc1);

    vSense = adc1 * ADC_TO_VOLT;
    temp = (V25 - vSense) / AVG_SLOPE + 25.0;

    // 첫 번째 줄: [온도계] CPU ADC : 0000
    LCD_XY(0, 0);
    LCD_PutCustomChar(0);  // 온도계 아이콘
    snprintf(lcd_buf, sizeof(lcd_buf), "CPU ADC : %4d", adc1);
    LCD_PUTS(lcd_buf);

    // 두 번째 줄: [온도계] CPU Temp: 00°C
    LCD_XY(0, 1);
    LCD_PutCustomChar(0);  // 온도계 아이콘
    snprintf(lcd_buf, sizeof(lcd_buf), "CPU Temp:%3.0f%cC", temp, 0xDF);
    LCD_PUTS(lcd_buf);

    printf("ADC:%d  Temp:%.2fC\r\n", adc1, temp);

    HAL_Delay(500);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV8;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif
