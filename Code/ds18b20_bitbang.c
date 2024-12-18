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
  MX_TIM6_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  const ds18b20_bb_config_t ds18b20 = { .timer = &htim6, .port = GPIOC, .pin = GPIO_PIN_4, .rom_code = NULL};
  float temp;
  if (ds18b20_bb_init(&ds18b20, false) != HAL_OK)
  {
	  Error_Handler();
  }
  if (ds18b20_bb_sampling_start(&ds18b20) != HAL_OK)
  {
	  Error_Handler();
  }
  else
  {
  	  HAL_Delay(750);
  	  temp = ds18b20_bb_temp_get(&ds18b20);
      HAL_PWR_EnterSTANDBYMode();
  }
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}