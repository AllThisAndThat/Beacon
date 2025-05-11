#include "timer.h"

#include "syscfg.h"

Timer::Timer() {

}

Timer::~Timer() {

}

HAL_StatusTypeDef Timer::initiate() {
  HAL_StatusTypeDef status;

  htim_.Instance = TIM1;
  htim_.Init.Prescaler = 0;
  htim_.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim_.Init.Period = 65535;
  htim_.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim_.Init.RepetitionCounter = 0;
  htim_.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  status = HAL_TIM_Base_Init(&htim_);
  if (status != HAL_OK) {return status;}

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  status = HAL_TIM_ConfigClockSource(&htim_, &sClockSourceConfig);
  if (status != HAL_OK) {return status;}

  status = HAL_TIM_PWM_Init(&htim_);
  if (status != HAL_OK) {return status;}

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  status = HAL_TIMEx_MasterConfigSynchronization(&htim_, &sMasterConfig);
  if (status != HAL_OK) {return status;}

  TIM_OC_InitTypeDef sConfigOC = {0};
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  status = HAL_TIM_PWM_ConfigChannel(&htim_, &sConfigOC, TIM_CHANNEL_1);
  if (status != HAL_OK) {return status;}

  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.BreakAFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.Break2AFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  status = HAL_TIMEx_ConfigBreakDeadTime(&htim_, &sBreakDeadTimeConfig);
  if (status != HAL_OK) {return status;}

  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  return HAL_OK;
}

HAL_StatusTypeDef Timer::set_duty(uint32_t duty) {
  htim_.Instance->CCR1 = duty;
  return HAL_OK;
}

HAL_StatusTypeDef Timer::action_start() {
  return HAL_TIM_PWM_Start(&htim_, TIM_CHANNEL_1); 
}

HAL_StatusTypeDef Timer::action_stop() {
  return HAL_TIM_PWM_Stop(&htim_, TIM_CHANNEL_1); 
}
