/* Подключение требуемых директорий */
#include <stdio.h>
#include "pico/stdlib.h" /* pico/stdlib.h включает в себя  hardware_uart, hardware_gpio, pico_binary_info, pico_runtime, pico_ platform, pico_printf, 
    pico_stdio, pico_standart_link, pico_util*/
#include "hardware/spi.h"
#include "hardware/uart.h"
#include "hardware/base.h"




/* Определение пинов*/
#define PORT_SPI spi0
#define PORT_UART uart0

#define MISO 1 /* CPOL = 0 - исходное значение сигнала синхронизации - LOW */

#define SCn1 2 /* Сигнал выбора АЦП */
#define SCn2 7
#define SCn3 24

#define SCK  6 /* Принимаемый сигнал. CPHA = 0 - чтение идёт по переднему фронту */

#define RX 22 /* Вывод для передачи данных на внешние устройства */
#define TX 21 




/* Определение скорости передачи */
#define SPEED 2000000 /* 2000000Гц -> 2 МГц */




/* Временые константы для задания длительности импульса */
#define DELAY_82NS asm volatile ("nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t")
#define DELAY_410NS 5*DELAY_82NS




/* Определение глобальных переменных */
long ADCread; /* Объявление переменных для хранения данных с 24 разрядного АЦП в 32 разрядной переменной */
float pga_divider = 1,0; /* Значение коэффициента усиления */




/* Инициализация пинов SPI */
void SETTING_PIN_SPI(){
    /* Задать пинам требуемый функционал */

    spi_init(SPI_PORT, SPEED); /* Инициализирует экземпляры SPI. Переводит SPI в известное состояние и включает его. Необходимо вызывать перед другими функциями */

    /* Инициализирует экземпляры SPI */
    gpio_set_function(MISO, GPIO_FUNC_SPI);
    /* Пин MISO должен работать на вход (принимать сигнал) */
    gpio_set_function(SCn1, GPIO_FUNC_SIO);
    /* Задать начальный уровень HIGH */
    gpio_set_function(SCn2, GPIO_FUNC_SIO);
    /* Задать начальный уровень HIGH */
    gpio_set_function(SCn3, GPIO_FUNC_SIO);
    /* Задать начальный уровень HIGH */
    gpio_set_function(SCK, GPIO_FUNC_SPI);
    /* Задать начальный уровень LOW. Пин SCK должен работать на выход */
}




/* Инициализация пинов UART */
void SETTING_PIN_UART(){
    /* Задать пинам требуемый функционал */

    spi_init(PORT_UART, 115200); /* Инициализирует экземпляры SPI. Переводит SPI в известное состояние и включает его. Необходимо вызывать перед другими функциями */

    gpio_set_funcction(RX, UART_FUNCSEL_NUM(uart0, 1));
    gpio_set_funcction(TX, UART_FUNCSEL_NUM(uart0, 0));
}




/* Одиночный испульс синхронизации*/
void SCK_PULSE(){
    /* Задать высокий уровень. Пин SCK должен работать на выход */
    /* Удержать импульс на заданную длительность */
    /* Задать низкий уровень */
    /* Удержать импульс на заданную длительность */
}




/* Защитная последовательность из 3 импульсов */
void Z_POLSE(){
    SCK_PULSE();
    SCK_PULSE();
    SCK_PULSE();
}




/* Чтение данных с АЦП */
long readADC(){
    long result = 0;

    Z_POLSE(); /* Защитная  последовательность идёт спереди, так как мы используеи CPHA = 0 */

    for(int i = 0; i < 24; i++){
        /**/
        result |= /* Функци считывания */ << (23 - i)
    }

    return result;
}




int main()
{
    stdio_init_all();
    printf("Institute of Metal Physics. Magnetic control device\n");

    /* Инициализация пинов */
    SETTING_PIN_SPI();
    SETTING_PIN_UART();
    
    /* Для уверенности задаём пинам уровни: SCK - LOW, SC - HIGH */  

    /* Главный цикл */
    for(pass; pass; pass){
        for(для каждого SCn){
            /*  */
            /* Задаём SCn уровень - LOW */
            Z_POLSE();
            /* Считывание данных с датчика */
            /* Передача данных по USB */
            /* Задаём SCn уровень - HIGH */
        }
    }

    return 0;
}
