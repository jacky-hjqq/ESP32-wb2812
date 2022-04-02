/*
 * SPI_ws2812.c
 *
 *  Created on: 02-Nov-2020
 *      Author: Dhananjay Khairnar
 */

#include "SPI_ws2812.h"
#define LED_MAX_NBER_LEDS 1
#define LED_DMA_BUFFER_SIZE ((LED_MAX_NBER_LEDS * 16 * (24 / 4))) + 1
#define LED_PIN GPIO_NUM_19

typedef struct
{
    spi_host_device_t host;
    spi_device_handle_t spi;
    int dma_chan;
    spi_device_interface_config_t devcfg;
    spi_bus_config_t buscfg;
} SPI_settings_t;

static SPI_settings_t SPI_settings = {
    .host = HSPI_HOST,
    .dma_chan = 2,
    .buscfg = {
        .miso_io_num = -1,
        .mosi_io_num = LED_PIN,
        .sclk_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = LED_DMA_BUFFER_SIZE},
    .devcfg = {.clock_speed_hz = 3.2 * 1000 * 1000, // Clock out at 3.2 MHz
               .mode = 0,                           // SPI mode 0
               .spics_io_num = -1,                  // CS pin
               .queue_size = 1,                     // Not sure if needed
               .command_bits = 0,
               .address_bits = 0}};

uint32_t *ledDMAbuffer;

void initSPIws2812()
{
    esp_err_t err;

    err = spi_bus_initialize(SPI_settings.host, &SPI_settings.buscfg, SPI_settings.dma_chan);
    ESP_ERROR_CHECK(err);

    // Attach the Accel to the SPI bus
    err = spi_bus_add_device(SPI_settings.host, &SPI_settings.devcfg, &SPI_settings.spi);
    ESP_ERROR_CHECK(err);

    ledDMAbuffer = heap_caps_malloc(LED_DMA_BUFFER_SIZE, MALLOC_CAP_DMA); // Critical to be DMA memory.
}

void fillBuffer(uint32_t *bufLed, int Count)
{
    for (int i = 0; i < Count; i++)
    {
        ledDMAbuffer[i] = bufLed[i];
    }
}

void LED_color_set(CRGB color)
{
    uint32_t data_buff[4];
    uint8_t byte;
    uint8_t flag = 0x80; // 1000 0000

    for (int i = 0; i < 4; i++)
    {
        if (i % 4 == 0)
            byte = color.g;
        else if (i % 4 == 1)
            byte = color.r;
        else if (i % 4 == 2)
            byte = color.b;
        else
            byte = 0x00;

        int j = 8;

        while (flag)
        {
            data_buff[i] |= (1 << (j * 4 - 1));
            if (byte & flag)
            {
                data_buff[i] |= (1 << (j * 4 - 2));
            }
            flag >>= 1;
            j--;
        }
        flag = 0x80;
    }

    fillBuffer(data_buff, 4);
    esp_err_t err;
    spi_transaction_t t;

    memset(&t, 0, sizeof(t));
    t.length = LED_DMA_BUFFER_SIZE * 8; // length is in bits
    t.tx_buffer = ledDMAbuffer;

    err = spi_device_transmit(SPI_settings.spi, &t);
    ESP_ERROR_CHECK(err);
}
