/* Подключение требуемых директорий */
#include <stdio.h>
#include <iostream>
#include <string.h>
#include "pico/binary_info.h"
#include "pico/until/datetime.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"

/* Назначение портов */
#define SPI_PORT spi0 /* По идее SPI0 должен работать с 2 устройствами, но на плате почему-то для SPI0 - 3 группы контактов для устройств, а на SPI1 - 2 */
#define PIN_MISO_1 1
#define PIN_MISO_2 6
#define PIN_MISO_3 21
#define PIN_SCK  4

/* Константа для создания импульса заданной длительности */
#define DELAY_82NS asm volatile('nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p') 

/* Частота процессора */
#define FREQUENCY 1000*1000 /* 1 МГц */

/* Создание переменных для хранения данных 24 разрядного сигнала с АЦП -> записываем в 32 разрядную переменную */
long ADCread1, ADCread2, ADCread1

/* Значение коэффициента усиления */
float pga_divider = 1.0

/* Создание задержки 455 мс, согласно требованиям документации */
void costomDelay455ms(){ /* Время задержки будет незначительно больше из-за входа в цикл */
    for(int i = 0; i < 5; ++i){
        DELAY_82NS;
    }

    /* Если нужна большая скорость выполнения, то закоментировать цикл и разкоментировать фрагмент ниже */
    /*
    asm volatile('nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p')
    asm volatile('nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p')
    asm volatile('nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p')
    asm volatile('nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p')
    asm volatile('nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p nop\n\p')
    */
}

/* Одиночный импульс */
void PinUpDown(Pin){
    digitalWrite(Pin, HIGH);
    costomDelay455ms();
    digitalWrite(Pin, LOW);
    costomDelay455ms();
}

/* Подключение платы */
void setup(DOUT_DRDY){
    /* Запуск последовательной связи */
    Serial.begin(115200);
    /* Ожидает появления последовательной связи */
    while (!Serial)
    {
        /* Просто прокручивается, пока не установится связь */
    }
    delay(3000);

    /* Стандартный вывод, для контроля пользователем */
    std::cout << "RP2040-CS1237 ADC - Curious Scintist, Custon code" << endl;

    pinMode(DOUT_DRDY, INOUT);
}

int main()
{
    stdio_init_all();

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 1000*1000);
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
