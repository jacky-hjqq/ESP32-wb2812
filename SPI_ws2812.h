#ifndef MAIN_SPI_WS2812_H_
#define MAIN_SPI_WS2812_H_
#include <stdio.h>
#include <string.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"

typedef struct CRGB
{
    union
    {
        struct
        {
            union
            {
                uint8_t b;
                uint8_t blue;
            };
            union
            {
                uint8_t r;
                uint8_t red;
            };
            union
            {
                uint8_t g;
                uint8_t green;
            };
        };
        uint8_t raw[3];
        uint32_t num;
    };
} CRGB;

void initSPIws2812();
void LED_color_set(CRGB color);

#endif /* MAIN_SPI_WS2812_H_ */