/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "main.h"

/* Private defines -----------------------------------------------------------*/
typedef struct
{
    uint8_t green;
    uint8_t red;
    uint8_t blue;

} ws2812b_frame_t;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
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

void ws2812b_send_frame(ws2812b_frame_t *frame, uint8_t size)
{
    for(uint8_t i = 0; i < size; i++)
    {
        // G7..G0 R7..R0 B7..B0, MSB first
        ws2812b_send_byte(frame[i].green);
        ws2812b_send_byte(frame[i].red);
        ws2812b_send_byte(frame[i].blue);
    }
    return;
}

void main(void)
{


    // init clock to 16 MHz
    // HSIDIV = 1, CPUDIV = 1
    CLK->CKDIVR = 0;

    // init GPIO
    GPIOB->DDR |= LED_PIN | WS2812B_PIN;

    #define FRAME_SIZE 3
    ws2812b_frame_t frame[FRAME_SIZE] = {{0, 0, 0}, {0, 0, 0}, { 0, 0, 0}};

    uint8_t val = 0;
    uint8_t dir = 0;
    uint8_t shift = 0;
    #define VAL_MAX 16
    #define VAL_MIN 0
    ws2812b_send_reset();
    while(1) {
        if(dir == 0 && val >  VAL_MIN) val--;
        if(dir > 0  && val <  VAL_MAX) val++;
        if(dir > 0  && val >= VAL_MAX) dir = 0;
        if(dir == 0 && val <= VAL_MIN) { dir = 1; shift++; }

        // sort of memset
        for(uint8_t i = 0; i < sizeof(frame); i++)
        {
            *(((uint8_t *) frame) + i) = 0;
        }
        frame[(shift + 0) % 3].green = val;
        frame[(shift + 1) % 3].red = val;
        frame[(shift + 2) % 3].blue = val;

        ws2812b_send_frame(frame, FRAME_SIZE);
        ws2812b_send_frame(frame, FRAME_SIZE);
        ws2812b_send_frame(frame, FRAME_SIZE);
        ws2812b_send_frame(frame, FRAME_SIZE);
        ws2812b_send_reset();
        delay_us(60000);
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
