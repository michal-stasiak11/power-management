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
  float temp, pressure;
  const lps25hb_config_t lps25hb = {.i2c = &hi2c3, .address = 0xBA, .timeout = HAL_MAX_DELAY, .calib = 100};
  if(lps25hb_init(&lps25hb) != HAL_OK)
  {
	  Error_Handler();
  }
  else
  {
	  HAL_Delay(3000);
	  temp = lps25hb_temp_get(&lps25hb);
	  pressure = lps25hb_pressure_get(&lps25hb);;
	  lps25hb_uninit(&lps25hb);
  }
  HAL_PWR_EnterSTANDBYMode();
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}