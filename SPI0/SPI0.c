/* Подключение требуемых директорий */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"


/* Назначение портов */
#define SPI_PORT spi0
#define PIN_MISO_1 1
#define PIN_MISO_2 6
#define PIN_MISO_3 21
#define PIN_SCK  4


/* Скорость передачи данных */
#define FREQUENCY 1000000 /* 1000000Гц -> 1 МГц */


/* Временые константы для задания длительности импульса и задержки*/
#define DELAY_82NS asm volatile ("nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t")
#define DELAY_410NS 5*DELAY_82NS


/* Переменные для хранения данных с датчиков (АЦП) */
long ADCread1, ADCread2, ADCread3
float pga_divider = 1,0 /* Значение коэффициента усиления */


/* Одиночный испульс */
void PinUpDown(){}


int main()
{
    stdio_init_all();

    /* Инициализирует экземпляры SPI */
    spi_init(SPI_PORT, FREQUENCY);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
