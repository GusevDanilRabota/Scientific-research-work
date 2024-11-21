#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

/* Определения SPI. Используется SPI_0 и назначен следующим выводам GPIO. Выводы можно изменить
    #define PIN_CS   17
    #define PIN_MOSI 19 */
#define SPI_PORT spi0
#define PIN_MISO_1 1
#define PIN_MISO_2 6
#define PIN_MISO_3 21
#define PIN_SCK  4

int main()
{
    stdio_init_all();

    /* Инициализация SPI. В этом примере будет использоваться SPI на частоте 1 МГц
        gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
        gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI); */
    spi_init(SPI_PORT, 1000*1000);
    gpio_set_function(PIN_MISO_1, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MISO_2, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MISO_3, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    
    /* Выбор микросхемы активен при низком напряжении, поэтому мы инициализируем его в состоянии высокого напряжения */
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);

    while (true) {
        sleep_ms(1000);
    }
}
