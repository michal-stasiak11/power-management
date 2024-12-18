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
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint16_t buffer;
  float temp = 0;
  const adc_config_t adc1 = { .adc = &hadc1, .buffer = &buffer, .resolution = 12, .channels = 1};
  const adc_sensor_config_t lm35dz = { .adc_config = &adc1, .converter = lm35dz_temp_get, .channel = 0};
  HAL_Delay(10);
	adc_values_poll(&adc1);
	temp = adc_sensor_value_get(&lm35dz);
	HAL_PWR_EnterSTANDBYMode();
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}