#include "SPI_ws2812.h"

void app_main()
{
	CRGB LED_color = {{.b = 0xff, .r = 0x00, .g = 0x00}};
	initSPIws2812();
	LED_color_set(LED_color);
}