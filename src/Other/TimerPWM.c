void TimerPWM()
{
	//
	GPIO_InitTypeDef Gpio_InitStr;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	//TIM2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	// GPIOA clock enable
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

  //TIM2 CH1(PA0) CH2(PA1) CH3(PA2) CH4(PA3)
	Gpio_InitStr.GPIO_Mode = GPIO_Mode_AF_PP;
	Gpio_InitStr.GPIO_Speed = GPIO_Speed_50MHz;
	Gpio_InitStr.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_Init(GPIOA, &Gpio_InitStr);

	// Time Base configuration
	TIM_TimeBaseStructure.TIM_Prescaler = getHclkFrequency()/Frequency-1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = LargestData-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	//
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;//Init with duty cycle zero.
	//High active
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  //TIM2
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);
	TIM_OC4Init(TIM2, &TIM_OCInitStructure);

	
	TIM_Cmd(TIM3, ENABLE);
	
	//TIM2
	TIM_Cmd(TIM2, ENABLE);

	/* TIM2 Main Output Enable */
	TIM_CtrlPWMOutputs(TIM2, ENABLE);

	//TIM2
	TIM_SetCompare1(TIM2, 50);
	
	TIM_SetCompare2(TIM2, 50);

  TIM_SetCompare3(TIM2, 50);
	
	TIM_SetCompare4(TIM2, 50);
}