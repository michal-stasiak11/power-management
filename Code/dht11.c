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
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint8_t buffer[5], temp, hum;
  const dht11_config_t dht11 = {.timer = &htim6, .port = GPIOC, .pin = GPIO_PIN_4, .buffer = buffer};
  if(dht11_data_fetch(&dht11) != HAL_OK)
  {
	  Error_Handler();
  }
  else
  {
	  temp = dht11_temp_get(&dht11);
	  hum = dht11_humidity_get(&dht11);
  }
  HAL_PWR_EnterSTANDBYMode();
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}