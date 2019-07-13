/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "main.h"

/* Private defines -----------------------------------------------------------*/
typedef struct
{
    uint8_t green;
    uint8_t red;
    uint8_t blue;

} ws2812b_led_t;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
void init(void)
{
    // init clock to 16 MHz
    // HSIDIV = 1, CPUDIV = 1
    CLK->CKDIVR = 0;

    // init GPIO
    GPIOB->DDR |= LED_PIN | WS2812B_PIN;
}


static void delay_us(uint32_t ms)
{
    // very very rough estimation
    for(uint32_t i = 0; i < ms; i++)
    {
        __asm__("nop\n nop\n nop\n");
        __asm__("nop\n nop\n nop\n");
    }
}

static void ws2812b_send_reset(void)
{
    // 50+ us low
    for(uint8_t i = 0; i < 200; i++)
    {
        __asm__("nop\n nop\n nop\n");
    }
    return;
}

void ws2812b_send_byte(uint8_t byte)
{
    int8_t i = 8;
    while(i > 0)
    {
        if(byte & (1 << (i-1)))
        {
            // send 1: 0.8 us high, 0.45 us low
            WS2812B_PORT->ODR |= WS2812B_PIN;
            __asm__("nop\n nop\n nop\n nop\n");
            __asm__("nop\n nop\n nop\n");
        } else {
            //send 0: 0.4 us high, 0.85 us low
            WS2812B_PORT->ODR |= WS2812B_PIN;
            __asm__("nop\n nop\n nop\n nop\n nop\n");
        }
        // no waiting, sw will take time to load next bit
        WS2812B_PORT->ODR &= ~WS2812B_PIN;
        //__asm__("nop\n nop\n nop\n");
        i--;
    }

}

void ws2812b_send_leds(ws2812b_led_t *leds, uint8_t count)
{
    for(uint8_t i = 0; i < count; i++)
    {
        // G7..G0 R7..R0 B7..B0, MSB first
        ws2812b_send_byte(leds[i].green);
        ws2812b_send_byte(leds[i].red);
        ws2812b_send_byte(leds[i].blue);
    }
    return;
}

void main(void)
{
	init();

    #define LEDS_COUNT 12
    ws2812b_led_t leds[LEDS_COUNT];

	#define MAX_VAL 16
	uint8_t current_led = 0;
	uint8_t current_val = MAX_VAL;
	uint8_t next_led;

    ws2812b_send_reset();
    while(1) {
/*
		// sort of memset
        for(uint8_t i = 0; i < sizeof(leds); i++)
        {
            *(((uint8_t *) leds) + i) = 0;
        }
*/

		// green background
		for(uint8_t i = 0; i < LEDS_COUNT; i++)
		{
			leds[i].green = 1;
			leds[i].red = 0;
			leds[i].blue = 0;
		}

		// if we done with this led, move to next
		if(current_val == 0) {
			current_led++;
			current_val = MAX_VAL;
		}

		// if this led was last one, start from 0
		if(current_led == LEDS_COUNT) current_led = 0;

		// decrease current led brightness
		current_val--;
		leds[current_led].red = current_val;

		// increase next led brightness
		if(current_led < (LEDS_COUNT - 1))
		{
			next_led = current_led + 1;
		} else {
			next_led = 0;
		}
		leds[next_led].red = MAX_VAL - current_val;

		// send data
        for(uint8_t i = 0; i < LEDS_COUNT; i++)
		{
			ws2812b_send_leds(leds, LEDS_COUNT);
		}
        ws2812b_send_reset();
        //delay_us(1000);
    }

}



/***********************************************************************/

/* Interrupt example
void tim4_isr(void) __interrupt(ITC_IRQ_TIM4_OVF) {

}
*/


/* Debug output

int putchar (int c)
{
    // this implementation is SDCC 3.8 specific
    // see sdcc stdio.h for details
    UART1->DR = (uint8_t) c;
    while (!(UART1->SR & UART1_FLAG_TXE));
    return c;
}
*/

// This is called by some of the SPL files on error.
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  (void)file;
  (void)line;

  /* User can add their own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
