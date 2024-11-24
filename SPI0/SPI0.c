/* Подключение требуемых директорий */
#include <stdio.h>
#include "pico/stdlib.h"
/* pico/stdlib.h включает в себя  hardware_uart, hardware_gpio, pico_binary_info, pico_runtime, pico_ platform, pico_printf, 
pico_stdio, pico_standart_link, pico_util*/
#include "hardware/spi.h"


/* Назначение портов SPI*/
#define SPI_PORT spi0
#define PIN_MISO_1 1
#define PIN_MISO_2 6
#define PIN_MISO_3 21
#define PIN_SCK  4


/* Назначение портов UART (для USB)*/
/*
#define PIN_RX 22
#define PIN_TX 21
*/


/* Скорость передачи данных */
#define FREQUENCY 1000000 /* 1000000Гц -> 1 МГц */


/* Временые константы для задания длительности импульса и задержки*/
#define DELAY_82NS asm volatile ("nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t")
#define DELAY_410NS 5*DELAY_82NS


/* Переменные для хранения данных с датчиков (АЦП) */
long ADCread1, ADCread2, ADCread3;
float pga_divider = 1,0; /* Значение коэффициента усиления */


/* Одиночный испульс синхронизации*/
void PinUpDown(){
    gpio_put(PIN_SCK, HIGH);
    DELAY_410NS;
    gpio_put(PIN_SCK, LOW);
    DELAY_410NS;
}


/* Плдключение платы */
void setup(){

}


int main()
{
    stdio_init_all();

    sleep_ms(2*1000);
    printf("Institute of Metal Physics. Magnetic control device\n");

    /* Запуск последовательной связи */
    spi_init(SPI_PORT, FREQUENCY); /* Инициализирует экземпляры SPI. Переводит SPI в известное состояние и включает его. Необходимо вызывать перед другими функциями */
    /* ожидание появления последовательной свяи */
    
    /* Инициализирует экземпляры SPI */
    gpio_set_function(PIN_MISO_1, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MISO_2, GPIO_FUNC_SIO);
    gpio_set_function(PIN_MISO_3, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK,    GPIO_FUNC_SPI);

    /* Инициализирует экземпляры UART */
    /*
    gpio_set_function(PIN_RX, GPIO_FUNC_UART);
    gpio_set_function(PIN_TX, GPIO_FUNC_UART);
    */    

    /* Выбор микросхемы активен при низком напряжении, поэтому мы инициализируем его в состоянии высокого напряжения */
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
    return 0;
}
