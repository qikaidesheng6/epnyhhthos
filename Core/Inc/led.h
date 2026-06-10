#ifndef __LED_H
#define __LED_H

#include "stm32f1xx_hal.h"

/* ================= ????? ================= */

/*
 * ????? LED ???????,???????
 * ????????�??? LED ?????�
 */
#define LED_GPIO_PORT   GPIOB

/* ??:LED ????(???????) */
#define LED_PIN_1       GPIO_PIN_6
#define LED_PIN_2       GPIO_PIN_7
#define LED_PIN_3       GPIO_PIN_8
#define LED_PIN_4       GPIO_PIN_9

/* ?? LED ??(??????? / ??) */
#define LED_ALL_PINS    (LED_PIN_1 | LED_PIN_2 | LED_PIN_3 | LED_PIN_4)

/*
 * ????(?? ?????????)
 *
 * ?? LED(??? GPIO):????
 *    #define LED_ON   GPIO_PIN_RESET
 *    #define LED_OFF  GPIO_PIN_SET
 *
 * ?? LED(??? GPIO):????
 *    #define LED_ON   GPIO_PIN_SET
 *    #define LED_OFF  GPIO_PIN_RESET
 */
#define LED_ON          GPIO_PIN_RESET
#define LED_OFF         GPIO_PIN_SET

#define PWM_PERIOD  800
#define BRIGHTNESS  30

// 最大占空比(0~PWM_PERIOD)，控制最大亮度
// 800 = 100% 最亮; 600 = 75%; 400 = 50%; 越小越暗
#define MAX_DUTY    300

// 呼吸步长：越大呼吸越快，越小呼吸越慢
// 2 = 慢速; 4 = 中速; 8 = 快速; 16 = 很快
#define BREATH_STEP 6


/* ================= ??? LED ??(??) ================= */

/* ?? LED ??? GPIOA / GPIOB,??????? */
#define LED_MULTI_PORT_ENABLE

#ifdef LED_MULTI_PORT_ENABLE

typedef struct
{
    GPIO_TypeDef *port;   // GPIO ??
    uint16_t pin;         // GPIO ??
} LED_TypeDef;

#define LED_NUM  24

extern LED_TypeDef leds[LED_NUM];

#endif

/* ================= ???? ================= */

void LED_Init(void);

void LED_On(uint16_t pin);
void LED_Off(uint16_t pin);
void LED_Toggle(uint16_t pin);

void LED_Flow(void);      // ???
void LED_AllOff(void);    // ????
void LED_FlowAndFlash(void);  
void LED_FlowOn_AllOn_FlowOff(void);
void LED_Breath(void);
#endif