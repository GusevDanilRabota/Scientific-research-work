/* Основные библиотеки */
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/uart.h"




/* Определение SPI */
#define PORT_SPI spi0

#define MISO 1 /* Принимаемый сигнал. CPHA = 0 - чтение идёт по переднему фронту */

#define SCn1 2 /* Пин выбора ведомого */
#define SCn2 7
#define SCn3 24

#define SCK  6 /* CPOL = 0 - исходное значение сигнала синхронизации - LOW */




/* Определение UART */
#define PORT_UART uart0

#define RX 22 /* Вывод для передачи данных на внешние устройства */
#define TX 21 




/* Определение скорости передачи */
#define SPEED_SPI 2000000 /* 2 000 000 Гц -> 2 МГц */
#define SPEED_UART 115200 /* 2 000 000 Гц -> 2 МГц */




/* Временые константы для задания длительности импульса */
#define DELAY_410NS asm volatile ("nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t 
                                    nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t 
                                    nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t 
                                    nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t 
                                    nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t")
/* Может отличаться от расчитанного, но подругому задержки в 0.5 мс не достичь */




/* Определение глобальных переменных */
int32_t ADCread = 0; /*Объявление переменных для хранения данных с 24 разрядного АЦП в 32 разрядной переменной */
int32_t result = 0;

float pga_divider = 128.0;

/* byte registerValue = 0b00111100; */




/* Защитный импульс */
void PROTECTIVE_IMPULSE(){
    gpio_put(SCK, HIGH);
    DELAY_410NS;
    gpio_put(SCK, LOW);
    DELAY_410NS;
}




/* Инициализация */
void SATTING_PORT_AND_PIN(){
    stdio_init_all();

    spi_init(PORT_SPI, SPEED_SPI);
    uart_init(PORT_UART, SPEED_UART);

    /* Инициализирует экземпляры SPI */
    gpio_set_function(MISO, GPIO_FUNC_SPI);
        gpio_set_dir(MISO, GPIO_IN);
        gpio_put(MISO, LOW);

    gpio_set_function(SCn1, GPIO_FUNC_SIO);
        gpio_set_dir(SCn1, GPIO_OUT);
        gpio_put(SCn1, HIGH);
    gpio_set_function(SCn2, GPIO_FUNC_SIO);
        gpio_set_dir(SCn1, GPIO_OUT);
        gpio_put(SCn1, HIGH);
    gpio_set_function(SCn3, GPIO_FUNC_SIO);
        gpio_set_dir(SCn1, GPIO_OUT);
        gpio_put(SCn1, HIGH);

    gpio_set_function(SCK, GPIO_FUNC_SPI);
        gpio_set_dir(SCK, GPIO_OUT);
        gpio_put(SCK, LOW);
    
    /* Инициализирует экземпляры SPI */
    gpio_set_funcction(RX, UART_FUNCSEL_NUM(PORT_UART, RX));
    gpio_set_funcction(TX, UART_FUNCSEL_NUM(PORT_UART, TX));

    printf("RP2040 - CS1237 ADC");
}




/* Чтение данных с АЦП*/
int32_t SPI_READ_DATA(SC){

    gpio_put(SC, LOW); /* Установка низкого уровня для считывания */

    if (spi_is_readable (spi_default)) {
        printf ("Reading data from SPI...\n");

        for (int i = 0; i < 24; i++){       
            gpio_put(SCK, HIGH);
            DELAY_410NS;

            result |= spi_read_blocking (PORT_SPI, 0, in_buf, 1); << (23 - i); //Read a bit and shift

            gpio_put(SCK, LOW);
            DELAY_410NS;
        }

        printf ("Data received: %d\n", in_buf [0]);
    }

    gpio_put(SC, HIGH);

    PROTECTIVE_IMPULSE();
    PROTECTIVE_IMPULSE();
    PROTECTIVE_IMPULSE();

    return result;
}




int main(){
    int32_t adcValue = 0;


    SATTING_PORT_AND_PIN();
    
    while (true) {
        for(int SC: {2, 7, 24}){
            printf("work with SC: %s", SC);

            ADCread = SPI_READ_DATA(SC);
            adcValue = ADCread;

            if(adcValue >> 23 == 1){ /* Если 24-й бит (знак) равен 1, то число отрицательное */
                adcValue -= 16777216; /* "зеркальное отображение" около нуля */
            }

            float voltageValue = (1250.0 / pga_divider) * ((float)adcValue / (8388607.0));

            printf("RAW-ADC reading: %s", adcValue);
            printf("Voltage reading: %s", voltageValue);
        } 

        printf("End of the cycle!\n");
        sleep_ms(4000);
    }
    return 0;
}
