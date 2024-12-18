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
  MX_RTC_Init();
  MX_I2C3_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint8_t buffer[8];
  float temp, hum;
  const am2320_config_t am2320 = {.i2c = &hi2c3, .address = 0xB8, .timeout = HAL_MAX_DELAY, .buffer = buffer};
  HAL_Delay(2000);
  if(am2320_data_fetch(&am2320) != HAL_OK)
  {
	Error_Handler();
  }
  else
  {
	  temp = am2320_temp_get(&am2320);
	  hum = am2320_humidity_get(&am2320);
  }
  HAL_PWR_EnterSTANDBYMode();
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}