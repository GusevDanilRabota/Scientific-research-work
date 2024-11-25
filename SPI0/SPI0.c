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
#define DELAY_410NS asm volatile ("nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t 
                                    nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t 
                                    nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t 
                                    nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t 
                                    nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t")
/* Может отличаться от расчитанного, но подругому задержки в 0.5 мс не достичь */




/* Определение глобальных переменных */
long ADCread; /* Объявление переменных для хранения данных с 24 разрядного АЦП в 32 разрядной переменной */
float pga_divider = 1,0; /* Значение коэффициента усиления */




/* Инициализация пинов SPI */
void SETTING_PIN_SPI(){
    /* Задать пинам требуемый функционал */

    spi_init(SPI_PORT, SPEED); /* Инициализирует экземпляры SPI. Переводит SPI в известное состояние и включает его. Необходимо вызывать перед другими функциями */

    /* Инициализирует экземпляры SPI */
    gpio_set_function(MISO, GPIO_FUNC_SPI);
        gpio_set_dir(MISO, GPIO_IN) /* Пин MISO должен работать на вход (принимать сигнал) */
    gpio_set_function(SCn1, GPIO_FUNC_SIO);
        gpio_set_dir(SCn1, GPIO_OUT);
        gpio_put(SCn1, HIGH); /* Задать начальный уровень HIGH */
    gpio_set_function(SCn2, GPIO_FUNC_SIO);
        gpio_set_dir(SCn1, GPIO_OUT);
        gpio_put(SCn1, HIGH); /* Задать начальный уровень HIGH */
    gpio_set_function(SCn3, GPIO_FUNC_SIO);
        gpio_set_dir(SCn1, GPIO_OUT);
        gpio_put(SCn1, HIGH); /* Задать начальный уровень HIGH */
    gpio_set_function(SCK, GPIO_FUNC_SPI);
        gpio_set_dir(SCK, GPIO_OUT);
        gpio_put(SCK, LOW); /* Задать начальный уровень LOW. Пин SCK должен работать на выход */
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
    gpio_put(SCK, HIGH); /* Задать высокий уровень. Пин SCK должен работать на выход */
    DELAY_410NS; /* Удержать импульс на заданную длительность */
    gpio_put(SCK, LOW); /* Задать низкий уровень */
    DELAY_410NS; /* Удержать импульс на заданную длительность */
}




/* Защитная последовательность из 3 импульсов */
void Z_POLSE(){
    SCK_PULSE();
    SCK_PULSE();
    SCK_PULSE();
}




/* Чтение данных с АЦП */
SPI_READ_DATA(SCn){
    gpio_put(SCn, LOW);
    for(){ /* В регистре не хватит места, поэтому используем 32 разрядную переменную */
        gpio_put(SCK, HIGH);
        DELAY_410NS;

        result |= digitalRead(DOUT_DRDY) << (23 - i);

        gpio_put(SCK, LOW);
        DELAY_410NS;
    }
    /* Передача данных */
    gpio_put(SCn, HIGH);
}
/*
Считывание данных с датчика по SPI предполагает следующую последовательность действий:

Установка низкого логического уровня на линии SS.
Загрузка данных в регистр SPDR.
    Offset:0x008
    Description SSPDR 
        Bits   Name      Type
        31:16  Reserved
        15:0   Date      RWF
Ожидание окончания передачи (проверка флага SPIF).
Сохранение принятых данных (чтение SPDR), если требуется.
Возврат на второй шаг, если переданы не все данные.
Установка высокого логического уровня на линии SS.

Некоторые функции для работы с SPI:
SPI_Write (unsigned char *data, unsigned char lenght). Запускает передачу данных.
SPI_Read (unsigned char *data, unsigned char lenght). Считывает данные.
Перед началом работы нужно определить, с какого бита должен начинаться сдвиг — со старшего или с младшего. Это регулируется функцией PI.setBitOrder(). 
Также необходимо выбрать уровень, на котором должна находиться линия SCK при отсутствии тактового импульса, и скорость передачи данных 
    (функции SPI.setDataMode() и SPI.setClockDivider()).

Для разных устройств и платформ процесс считывания данных по SPI может отличаться, поэтому рекомендуется обратиться к документации конкретного устройства.
*/




int main()
{
    stdio_init_all();
    printf("Institute of Metal Physics. Magnetic control device\n");

    /* Инициализация пинов */
    SETTING_PIN_SPI();
    SETTING_PIN_UART();
    
    /* Для уверенности задаём пинам уровни: SCK - LOW, SC - HIGH */ 
    gpio_put(SCK, LOW);
    gpio_put(SCn1, HIGH);
    gpio_put(SCn2, HIGH); 
    gpio_put(SCn3, HIGH); 

    /* Главный цикл */
    while(TRUE){ /* Надо потом переделать на цикл for - мы подаём синусоидальный сигнал конечной длительности и когда он закончится 
                нет смысла продолжать выполнение программы */
        for(int SC: {2, 7, 24}){
            printf("Work with SCn", SC);
            /*  */
            /* Считывание данных с датчика */
            SPI_READ_DATA(SC);
            /* Передача данных по USB */
        }
    }

    return 0;
}


/*
Считывание данных с датчика по SPI предполагает следующую последовательность действий:

Установка низкого логического уровня на линии SS.
Загрузка данных в регистр SPDR.
Ожидание окончания передачи (проверка флага SPIF).
Сохранение принятых данных (чтение SPDR), если требуется.
Возврат на второй шаг, если переданы не все данные.
Установка высокого логического уровня на линии SS.

Некоторые функции для работы с SPI:
SPI_Write (unsigned char *data, unsigned char lenght). Запускает передачу данных.
SPI_Read (unsigned char *data, unsigned char lenght). Считывает данные.
Перед началом работы нужно определить, с какого бита должен начинаться сдвиг — со старшего или с младшего. Это регулируется функцией PI.setBitOrder(). 
Также необходимо выбрать уровень, на котором должна находиться линия SCK при отсутствии тактового импульса, и скорость передачи данных 
    (функции SPI.setDataMode() и SPI.setClockDivider()).

Для разных устройств и платформ процесс считывания данных по SPI может отличаться, поэтому рекомендуется обратиться к документации конкретного устройства.
*/