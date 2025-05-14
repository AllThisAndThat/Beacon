#include "debug_led.h"

#include "syscfg.h"

DebugLed::DebugLed(TIM_HandleTypeDef hTim) {
  hTim_ = hTim;
  initiate();
}

DebugLed::~DebugLed() {

}

HAL_StatusTypeDef DebugLed::initiate() {
  HAL_StatusTypeDef status;

  hTim_.Instance = TIM1;
  hTim_.Init.Prescaler = 0;
  hTim_.Init.CounterMode = TIM_COUNTERMODE_UP;
  hTim_.Init.Period = 65535;
  hTim_.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  hTim_.Init.RepetitionCounter = 0;
  hTim_.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  status = HAL_TIM_Base_Init(&hTim_);
  if (status != HAL_OK) {return status;}

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  status = HAL_TIM_ConfigClockSource(&hTim_, &sClockSourceConfig); //HAL BUSY?
  if (status != HAL_OK) {return status;}

  status = HAL_TIM_PWM_Init(&hTim_);
  if (status != HAL_OK) {return status;}

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  status = HAL_TIMEx_MasterConfigSynchronization(&hTim_, &sMasterConfig);
  if (status != HAL_OK) {return status;}

  TIM_OC_InitTypeDef sConfigOC = {0};
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  status = HAL_TIM_PWM_ConfigChannel(&hTim_, &sConfigOC, TIM_CHANNEL_1);
  if (status != HAL_OK) {return status;}
  status = HAL_TIM_PWM_ConfigChannel(&hTim_, &sConfigOC, TIM_CHANNEL_2);
  if (status != HAL_OK) {return status;}
  status = HAL_TIM_PWM_ConfigChannel(&hTim_, &sConfigOC, TIM_CHANNEL_3);
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
  status = HAL_TIMEx_ConfigBreakDeadTime(&hTim_, &sBreakDeadTimeConfig);
  if (status != HAL_OK) {return status;}

  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_8| GPIO_PIN_9| GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  return HAL_OK;
}

HAL_StatusTypeDef DebugLed::set_color(Color color) {
  switch (color) {
    case Color::kRed:
      return set_color(0xFFF, 0, 0);
    case Color::kGreen:
      return set_color(0, 0xFF, 0);
    case Color::kBlue:
      return set_color(0, 0, 0xFFFF);
    case Color::kYellow:
      return set_color(0xFFFF, 0xFF, 0);
    default:
      return HAL_ERROR;
  }
  return HAL_OK;
}

HAL_StatusTypeDef DebugLed::set_color(uint16_t r, uint16_t g, uint16_t b) {
  hTim_.Instance->CCR1 = r;
  hTim_.Instance->CCR2 = g;
  hTim_.Instance->CCR3 = b;
  return HAL_OK;
}

HAL_StatusTypeDef DebugLed::action_start() {
  HAL_StatusTypeDef status;
  status =  HAL_TIM_PWM_Start(&hTim_, TIM_CHANNEL_1);
  if (status != HAL_OK) {return status;}

  status =  HAL_TIM_PWM_Start(&hTim_, TIM_CHANNEL_2);
  if (status != HAL_OK) {return status;}

  status =  HAL_TIM_PWM_Start(&hTim_, TIM_CHANNEL_3);
  if (status != HAL_OK) {return status;}

  return HAL_OK;
}

HAL_StatusTypeDef DebugLed::action_stop() {
  HAL_StatusTypeDef status;
  status =  HAL_TIM_PWM_Stop(&hTim_, TIM_CHANNEL_1);
  if (status != HAL_OK) {return status;}

  status =  HAL_TIM_PWM_Stop(&hTim_, TIM_CHANNEL_2);
  if (status != HAL_OK) {return status;}

  status =  HAL_TIM_PWM_Stop(&hTim_, TIM_CHANNEL_3);
  if (status != HAL_OK) {return status;}
}
