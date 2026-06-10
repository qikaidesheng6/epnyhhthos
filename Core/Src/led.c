#include "led.h"

/* ================= ??? LED ?? ================= */
#ifdef LED_MULTI_PORT_ENABLE


/* ??:????????? */
LED_TypeDef leds[LED_NUM] = {
    {GPIOB, GPIO_PIN_13},    
    {GPIOA, GPIO_PIN_8},
    {GPIOB, GPIO_PIN_15},
    {GPIOB, GPIO_PIN_14},
    {GPIOB, GPIO_PIN_1},
    {GPIOB, GPIO_PIN_0},
		{GPIOA, GPIO_PIN_7},
		{GPIOA, GPIO_PIN_6},
		{GPIOA, GPIO_PIN_4},
		{GPIOA, GPIO_PIN_3},
		{GPIOA, GPIO_PIN_2},
		{GPIOA, GPIO_PIN_1},
		{GPIOB, GPIO_PIN_9},
		{GPIOB, GPIO_PIN_8},
		{GPIOB, GPIO_PIN_7},
		{GPIOB, GPIO_PIN_6},
		{GPIOB, GPIO_PIN_5},
		{GPIOB, GPIO_PIN_4},
		{GPIOB, GPIO_PIN_3},
		{GPIOA, GPIO_PIN_15},
		{GPIOA, GPIO_PIN_12},
		{GPIOA, GPIO_PIN_11},
		{GPIOA, GPIO_PIN_10},
		{GPIOA, GPIO_PIN_9},
};

#endif
void LED_FlowOn_AllOn_FlowOff(void)
{
#ifdef LED_MULTI_PORT_ENABLE

    /* 1. 依次点亮 */
    for (int i = 0; i < LED_NUM; i++)
    {
        HAL_GPIO_WritePin(leds[i].port, leds[i].pin, LED_ON);
        HAL_Delay(60);
    }

    /* 2. 全亮 */
    for (int i = 0; i < LED_NUM; i++)
    {
        HAL_GPIO_WritePin(leds[i].port, leds[i].pin, LED_ON);
    }
    HAL_Delay(300);

    /* 3. 依次熄灭（✅ 新增） */
    for (int i = 0; i < LED_NUM; i++)
    {
        HAL_GPIO_WritePin(leds[i].port, leds[i].pin, LED_OFF);
        HAL_Delay(60);
    }

#endif
}

void LED_FlowAndFlash(void)
{
    /* 1. 依次点亮所有LED */
    for (int i = 0; i < LED_NUM; i++)
    {
        HAL_GPIO_WritePin(leds[i].port, leds[i].pin, LED_ON);
        HAL_Delay(60);
    }

    /* 2. 全亮 */
    for (int i = 0; i < LED_NUM; i++)
    {
        HAL_GPIO_WritePin(leds[i].port, leds[i].pin, LED_ON);
    }
    HAL_Delay(300);

    /* 3. 闪烁3 次 */
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < LED_NUM; j++)
            HAL_GPIO_WritePin(leds[j].port, leds[j].pin, LED_OFF);
        HAL_Delay(200);

        for (int j = 0; j < LED_NUM; j++)
            HAL_GPIO_WritePin(leds[j].port, leds[j].pin, LED_ON);
        HAL_Delay(200);
    }

    /* 4. 全灭 */
    for (int i = 0; i < LED_NUM; i++)
    {
        HAL_GPIO_WritePin(leds[i].port, leds[i].pin, LED_OFF);
    }
}

static uint16_t t = 0;
static uint16_t pwm = 0;
static uint8_t fade = 1;

void LED_Breath(void)
{
  // PWM 值递增/递减（三角波）
  if(fade)
  {
    pwm += BREATH_STEP;
    if(pwm >= PWM_PERIOD)
      fade = 0;
  }
  else
  {
    pwm -= BREATH_STEP;
    if(pwm == 0)
      fade = 1;
  }

  // 软件 PWM 生成
  // 共阳极: RESET = 亮, SET = 灭
  // t < on_time → 亮; t >= on_time → 灭
  // pwm=0 → 全灭; pwm=PWM_PERIOD → 最大亮度(由 MAX_DUTY 限制)
  for(t = 0; t < PWM_PERIOD; t++)
  {
    if(t < (pwm * MAX_DUTY / PWM_PERIOD))
    {
      for(uint8_t j = 0; j < LED_NUM; j++)
        HAL_GPIO_WritePin(leds[j].port, leds[j].pin, GPIO_PIN_RESET);
    }
    else
    {
      for(uint8_t j = 0; j < LED_NUM; j++)
        HAL_GPIO_WritePin(leds[j].port, leds[j].pin, GPIO_PIN_SET);
    }
  }
}

/* ================= LED ??? ================= */
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

#ifdef LED_MULTI_PORT_ENABLE
    /* ????? */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    for (int i = 0; i < LED_NUM; i++)
    {
        GPIO_InitStruct.Pin  = leds[i].pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(leds[i].port, &GPIO_InitStruct);

        /* ????(??:????) */
        HAL_GPIO_WritePin(leds[i].port, leds[i].pin, LED_OFF);
    }

#else
    /* ????? */
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin  = LED_ALL_PINS;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);

    /* ???? */
    HAL_GPIO_WritePin(LED_GPIO_PORT, LED_ALL_PINS, LED_OFF);
#endif
}

/* ================= LED ???? ================= */
void LED_On(uint16_t pin)
{
#ifndef LED_MULTI_PORT_ENABLE
    HAL_GPIO_WritePin(LED_GPIO_PORT, pin, LED_ON);
#endif
}

void LED_Off(uint16_t pin)
{
#ifndef LED_MULTI_PORT_ENABLE
    HAL_GPIO_WritePin(LED_GPIO_PORT, pin, LED_OFF);
#endif
}

void LED_Toggle(uint16_t pin)
{
#ifndef LED_MULTI_PORT_ENABLE
    HAL_GPIO_TogglePin(LED_GPIO_PORT, pin);
#endif
}

/* ================= ??? ================= */
void LED_Flow(void)
{
#ifdef LED_MULTI_PORT_ENABLE
    static uint8_t index = 0;

    /* ???? */
    for (int i = 0; i < LED_NUM; i++)
    {
        HAL_GPIO_WritePin(leds[i].port, leds[i].pin, LED_OFF);
    }

    /* ???? LED */
    HAL_GPIO_WritePin(leds[index].port, leds[index].pin, LED_ON);

    index++;
    if (index >= LED_NUM)
        index = 0;

#else
    static uint8_t index = 0;
    const uint16_t leds[] = {
        LED_PIN_1,
        LED_PIN_2,
        LED_PIN_3,
        LED_PIN_4
    };

    /* ???? */
    HAL_GPIO_WritePin(LED_GPIO_PORT, LED_ALL_PINS, LED_OFF);

    /* ???? LED */
    HAL_GPIO_WritePin(LED_GPIO_PORT, leds[index], LED_ON);

    index++;
    if (index >= 4)
        index = 0;
#endif
}

/* ================= ???? ================= */
void LED_AllOff(void)
{
#ifdef LED_MULTI_PORT_ENABLE
    for (int i = 0; i < LED_NUM; i++)
    {
        HAL_GPIO_WritePin(leds[i].port, leds[i].pin, LED_OFF);
    }
#else
    HAL_GPIO_WritePin(LED_GPIO_PORT, LED_ALL_PINS, LED_OFF);
#endif
}