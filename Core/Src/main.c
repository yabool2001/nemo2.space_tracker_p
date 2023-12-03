/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart4;

/* USER CODE BEGIN PV */
const char*	hello = "\nHello nemo2.space tracker p\n\n" ;
const char*	fv = "0.0.1" ;
char		rtc_dt_s[20] ;

// TIM
uint16_t	tim_seconds = 0 ; // Powinien być ten sam typ co my_lx6_gnss_active_time_ths

// RTC
RTC_TimeTypeDef rtc_t ;
RTC_DateTypeDef rtc_d ;
uint32_t		rtc_alarm_time = 7200 ;

// Astro
uint16_t	astro_payload_id = 0 ;
char		payload[ASTRO_PAYLOAD_MAX_LEN] = {0}; // 160 bajtów

// Flags
bool		is_system_already_initialized = false ; // Recognize if system has successful GNSS contact and has real time, Based on rtc settings.
bool		is_astro_evt_flag = false ;

// temp
// Przykładowe współrzędne
int32_t last_latitude_astro_geo_wr = 52278250 ; // Początkowe współrzędne: 52.278250 * 10000000
int32_t last_longitude_astro_geo_wr = 20809116 ; // Początkowe współrzędne: 20.809116 * 10000000
int32_t latitude_astro_geo_wr , longitude_astro_geo_wr ;

//int32_t latitude_astro_geo_wr = last_latitude_astro_geo_wr ;
//int32_t longitude_astro_geo_wr = last_longitude_astro_geo_wr ;
float pdop ;

//SYSTEM
char	dbg_m[DBG_PAYLOAD_MAX_LEN] = {0} ;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_RTC_Init(void);
static void MX_TIM6_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USART4_UART_Init(void);
/* USER CODE BEGIN PFP */
bool is_system_initialized ( void ) ;
bool my_astro_init ( TIM_HandleTypeDef ) ;
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_RTC_Init();
  MX_TIM6_Init();
  MX_USART3_UART_Init();
  MX_USART4_UART_Init();
  /* USER CODE BEGIN 2 */

  // System hello
  HAL_UART_Transmit ( HUART_DBG , (uint8_t*) hello , strlen (hello) , UART_TIMEOUT ) ;

  // System Init
  my_tim_init ( HTIM ) ;

  if ( !is_system_initialized () )
  {
	  if ( !my_astro_init ( htim6 ) )
	  {
		  HAL_NVIC_SystemReset () ;
	  }
	  else
	  {
		  while ( is_evt_pin_high() )
		  {
			  send_debug_logs ( "main.c,ucb2,is_evt_pin_high" ) ;
			  my_astro_read_evt_reg () ;
		  }
		  sprintf ( payload , "fv=%s" , fv ) ;
		  sprintf ( dbg_m , "main.c,ucb2,payload,%u %s" , astro_payload_id , payload ) ; // Żeby astro_payload_id był taki jak wysłany, bo po wysłaniu będzie zwiększony
		  my_astro_write_coordinates ( last_latitude_astro_geo_wr , last_longitude_astro_geo_wr ) ;
		  my_astro_add_payload_2_queue ( astro_payload_id++ , payload ) ;
		  send_debug_logs ( dbg_m ) ;
	  }
  }
  if ( my_rtc_set_alarm ( rtc_alarm_time ) )
  {
	  HAL_SuspendTick () ; // Jak nie wyłączę to mnie przerwanie SysTick od razu wybudzi!!!
	  HAL_PWR_EnterSTOPMode ( PWR_LOWPOWERREGULATOR_ON , PWR_STOPENTRY_WFE ) ;
	  HAL_ResumeTick () ;
	  my_rtc_get_dt_s ( rtc_dt_s ) ;
	  send_debug_logs ( rtc_dt_s ) ;
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  while ( is_evt_pin_high() )
	  {
		  send_debug_logs ( "main.c,ucbw,is_evt_pin_high" ) ;
		  my_astro_read_evt_reg () ;
	  }
	  // Prepare payload to Astronode
	  my_rand_get_coordinates ( &last_latitude_astro_geo_wr , &last_longitude_astro_geo_wr , &latitude_astro_geo_wr , &longitude_astro_geo_wr ) ;
	  my_astro_write_coordinates ( latitude_astro_geo_wr , longitude_astro_geo_wr ) ;
	  pdop = ((float) rand () / RAND_MAX) * 99.9 ;
	  sprintf ( payload , "%.1f" , pdop ) ;
	  sprintf ( dbg_m , "main.c,ucnw,payload,%u %s" , astro_payload_id , payload ) ; // Żeby astro_payload_id był taki jak wysłany, bo po wysłaniu będzie zwiększony
	  my_astro_add_payload_2_queue ( astro_payload_id++ , payload ) ;
	  send_debug_logs ( dbg_m ) ;

	  while ( is_evt_pin_high() )
	  {
		  send_debug_logs ( "main.c,ucbw,is_evt_pin_high" ) ;
		  my_astro_read_evt_reg () ;
	  }
	  send_debug_logs ( "main.c,ucbw,nothing to do! Going to stop for 10 s" ) ;
	  if ( my_rtc_set_alarm ( rtc_alarm_time ) )
	  {
		  HAL_SuspendTick () ; // Jak nie wyłączę to mnie przerwanie SysTick od razu wybudzi!!!
		  HAL_PWR_EnterSTOPMode ( PWR_LOWPOWERREGULATOR_ON , PWR_STOPENTRY_WFE ) ;
		  HAL_ResumeTick () ;
	  }
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

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  RTC_AlarmTypeDef sAlarm = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0;
  sTime.Minutes = 0;
  sTime.Seconds = 0;
  sTime.SubSeconds = 0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_SATURDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 1;
  sDate.Year = 0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the Alarm A
  */
  sAlarm.AlarmTime.Hours = 0;
  sAlarm.AlarmTime.Minutes = 0;
  sAlarm.AlarmTime.Seconds = 0;
  sAlarm.AlarmTime.SubSeconds = 0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 1;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 16000-1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 1000-1;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief USART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART4_UART_Init(void)
{

  /* USER CODE BEGIN USART4_Init 0 */

  /* USER CODE END USART4_Init 0 */

  /* USER CODE BEGIN USART4_Init 1 */

  /* USER CODE END USART4_Init 1 */
  huart4.Instance = USART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART4_Init 2 */

  /* USER CODE END USART4_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LDG_GPIO_Port, LDG_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, ASTRO_WKUP_Pin|ASTRO_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LDG_Pin */
  GPIO_InitStruct.Pin = LDG_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LDG_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : ASTRO_WKUP_Pin ASTRO_RST_Pin */
  GPIO_InitStruct.Pin = ASTRO_WKUP_Pin|ASTRO_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : ASTRO_EVT_IT2_Pin */
  GPIO_InitStruct.Pin = ASTRO_EVT_IT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ASTRO_EVT_IT2_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_3_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void send_debug_logs ( char* p_tx_buffer )
{
    uint32_t length = strlen ( p_tx_buffer ) ;

    if ( length > UART_TX_MAX_BUFF_SIZE )
    {
        HAL_UART_Transmit ( HUART_DBG , ( uint8_t* ) "[ERROR] UART buffer reached max length.\n" , 42 , 1000 ) ;
        length = UART_TX_MAX_BUFF_SIZE;
    }

    HAL_UART_Transmit ( HUART_DBG , ( uint8_t* ) p_tx_buffer , length , 1000 ) ;
    HAL_UART_Transmit ( HUART_DBG , ( uint8_t* ) "\n" , 1 , 1000 ) ;
}
void reset_astronode ( void )
{
    HAL_GPIO_WritePin ( ASTRO_RST_GPIO_Port , ASTRO_RST_Pin , GPIO_PIN_SET ) ;
    HAL_Delay ( 1 ) ;
    HAL_GPIO_WritePin ( ASTRO_RST_GPIO_Port , ASTRO_RST_Pin , GPIO_PIN_RESET ) ;
    HAL_Delay ( 250 ) ;
}
void send_astronode_request ( uint8_t* p_tx_buffer , uint32_t length )
{
    send_debug_logs ( "Message sent to the Astronode --> " ) ;
    send_debug_logs ( ( char* ) p_tx_buffer ) ;

    HAL_UART_Transmit ( HUART_ASTRO , p_tx_buffer , length , 1000 ) ;
}
bool is_astronode_character_received ( uint8_t* p_rx_char )
{
    return ( HAL_UART_Receive ( HUART_ASTRO , p_rx_char , 1 , 100 ) == HAL_OK ? true : false ) ;
}
bool is_evt_pin_high ( void )
{
	return ( HAL_GPIO_ReadPin ( /*GPIOA*/ ASTRO_EVT_IT2_GPIO_Port , ASTRO_EVT_IT2_Pin ) == GPIO_PIN_SET ? true : false);
}
uint32_t get_systick ( void )
{
    return HAL_GetTick() ;
}
bool is_systick_timeout_over ( uint32_t starting_value , uint16_t duration )
{
    return ( get_systick () - starting_value > duration ) ? true : false ;
}

// TIM functions
void my_tim_init ( TIM_HandleTypeDef htim )
{
	__HAL_TIM_CLEAR_IT ( &htim , TIM_IT_UPDATE ) ;
}

void my_tim_start ( TIM_HandleTypeDef htim )
{
	tim_seconds = 0 ;
	HAL_TIM_Base_Start_IT ( &htim ) ;
}

void my_tim_stop ( TIM_HandleTypeDef htim )
{
	HAL_TIM_Base_Stop_IT ( &htim ) ;
}

// Astronode functions
bool my_astro_init ( TIM_HandleTypeDef htim )
{
	bool cfg_wr = false ;
	tim_seconds = 0 ;

	my_tim_start ( htim ) ;
	while ( tim_seconds < MY_ASTRO_INIT_TIME && !cfg_wr )
	{
		reset_astronode () ;
		HAL_Delay ( 100 ) ;
		// Satellite Acknowledgement (true): Asset informed of ACK to satellite (default)
		// Add Geolocation (true)
		// Enable_ephemeris (true)
		// Deep Sleep Mode (false) NOT used
		// Satellite Ack Event Pin Mask (true): EVT pin shows EVT register Payload Ack bit state
		// Reset Notification Event Pin Mask (true):  EVT pin shows EVT register Reset Event Notification bit state
		// Command Available Event Pin Mask (true): EVT pin shows EVT register Command Available bit state
		// Message Transmission (Tx) Pending Event Pin Mask (false):  EVT pin does not show EVT register Msg Tx Pending bit state
		cfg_wr = astronode_send_cfg_wr ( true , true , true , false , true , true , true , false  ) ;
	}
	tim_seconds = 0 ;
	my_tim_stop ( htim ) ;
	if ( cfg_wr )
	{
		astronode_send_rtc_rr () ;
		astronode_send_cfg_sr () ;
		astronode_send_mpn_rr () ;
		astronode_send_msn_rr () ;
		astronode_send_mgi_rr () ;
		astronode_send_pld_fr () ;
		return true ;
	}
	else
	{
		return false ;
	}
}


// System functions
bool is_system_initialized ( void )
{
	// Nie próbuj robić nic z Astronode, bo nie wiesz czy nie trzeba go zainicjować restartem. Ogranicz się do samego systemu.
	uint16_t yyyy = my_rtc_get_dt_s ( rtc_dt_s ) ;
	send_debug_logs ( rtc_dt_s ) ;
	return ( yyyy >= FIRMWARE_RELEASE_YEAR ) ? true : false ;
}

// RTC Callbacks
void HAL_RTC_AlarmAEventCallback ( RTC_HandleTypeDef* hrtc )
{
	// is_rtc_alarm_a_flag = true ;
	//__HAL_RTC_ALARM_CLEAR_FLAG ( hrtc , RTC_FLAG_ALRAF ) ;  // Wyczyść flagę alarmu
	send_debug_logs ( "main.c,HAL_RTC_AlarmAEventCallback," ) ;
}

// TIM Callbacks

void HAL_TIM_PeriodElapsedCallback ( TIM_HandleTypeDef *htim )
{
	if ( htim->Instance == TIM6 )
	{
		tim_seconds++ ;
		if ( tim_seconds > TIM_SECONDS_THS_SYSTEM_RESET )
		{
			send_debug_logs ( "main.c,HAL_TIM_PeriodElapsedCallback,HAL_NVIC_SystemReset" ) ;
			HAL_NVIC_SystemReset () ;
		}
	}
}

// EXTI Callbacks
void HAL_GPIO_EXTI_Rising_Callback ( uint16_t GPIO_Pin )
{
	//zamiast poniższego może lepiej wrócić do pierwotnego przeglądania GPIO w trybie input bez przerwania
	/*
	if ( GPIO_Pin == 0x4 ) // ASTRO_EVT_Pin = 0x1000
	{
		is_astro_evt_flag = true ;
	}
	*/
	char m[70];
	sprintf ( m , "main.c,HAL_GPIO_EXTI_Rising_Callback,%d" , GPIO_Pin ) ;
	send_debug_logs ( m ) ;
}

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
