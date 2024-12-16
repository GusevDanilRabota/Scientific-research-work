/* СТАНДАРТНЫЕ БИБЛИОТЕКИ C++*/
    #include <iostream>
    #include <stdio.h>
    #include <string.h>
    #include <stdint.h>

/* ДОПОЛНИТЕЛЬНЫЕ БИБЛИОТЕКИ RASPBERRY PI PICO*/
    #include <pico/stdlib.h>
    #include <hardware/spi.h>
    #include <pico/binary_info.h>




/* ВРЕМЕННАЯ КОНСТАНТА */
    #define DELAY_410NS asm volatile ("nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t")

/* ОПРЕДЕЛЕНИЕ ГЛОБАЛЬНЫХ ПЕРЕМЕННЫХ */
    /* ПЕРЕМЕННЫЕ ДЛЯ ЗАПИСИ И ПЕРЕДАЧИ */
        #define BUF_LEN 32
        #define SPI_RW_LEN 1
        u_int32_t out_buf [BUF_LEN], in_buf [BUF_LEN];

    /* УРОВНИ СИГНАЛОВ */    
        #define LOW = 0
        #define HIGH = 1

    /* ПАРАМЕТР ПЕРЕДАЧИ */
        #define GPIO_IN = 0
        #define GPIO_OUT = 1

    /* ОПРЕДЕЛЕНИЕ ПОРТОВ И ПАРАМЕТРОВ ПРОТОКОЛА */
        /* SPI ПОРТЫ */
            #define PORT_SPI spi0

            #define MISO_1 1
            #define MISO_2 6
            #define MISO_3 21

            #define SCK 6

        /* ОПРЕДЕЛЕНИЕ СКОРОСТИ ПЕРЕДАЧИ */
            #define SPEED_SPI 2000000 /* СКОРОСТЬ 2 ГГц */


//============================================================================================================================================================

/* ОПИСАНИЕ КЛАССА АНАЛОГО-ЦИФРОВОГО ПРЕОБРАЗОВАТЕЛЯ 24 РАЗРЯДОВ */
class ADC{
    /* КОМПАНЕНТЫ КЛАССА */
public:
    /* ПЕРЕМЕННЫЕ */
    std::string name;
    u_int8_t pin;
    bool signal_strength; /* LOW или HIGH */
    bool operating_mode; /* GPIO_IN или GPIO_OUT */

    /* ИНИЦИАЛИЗАЦИЯ ПОРТА */
    void set_pin_adc(u_int8_t pin_number, bool strength_signal, bool mode_operating){ /* ИНИЦИАЛИЗАЦИЯ ПИНА */
        gpio_set_function(pin_number, 1); /* 1 - GPIO_FUNC_SPI */
        gpio_set_dir(pin_number, mode_operating);
        gpio_put(pin_number, strength_signal);
    }

    u_int32_t read(u_int8_t pin_number){
        for(u_int32_t i = 0; i < BUF_LEN; ++i){
            out_buf[i] = 0;
            in_buf[i] = 0;
        }
        std::cout << "Reading data from SPI - " << name << "\n" << std::endl;
        spi_read_blocking(PORT_SPI, 0, in_buf, SPI_RW_LEN);
        std::cout << "Date received: %d\n", in_buf[0] <<
        printf ("Data received: %d\n", in_buf [0]);
    }

    /* КОНСТРУКТОР */
    ADC(std::string p_name; u_int8_t p_pin; bool p_signal_strength; bool p_operating_mode){
        name = p_name;
        pin = p_pin;
        signal_strength = p_signal_strength;
        operating_mode = p_operating_mode;
        set_pin_adc(p_pin, p_signal_strength, p_operating_mode); /* ВЫЗОВ МЕТОДА ИЗ КОНСТРУКТОРА ВОЗМОЖЕН ТАК КАК МЕТОД НЕ ВИРТУАЛЬНЫЙ И ФУНКЦИИ ИНИЦИАЛИЗАЦИИ ВОЗМОЖНО ВЫЗЫВАТЬ ИЗ КОНСТРУКТОРА */
    }
};




int main(){
    /* ИНИЦИАЛИЗАЦИЯ  */
        /* ПОРТОВ ПЛАТЫ Raspberry Pi Pico */
            stdio_init_all();
            spi_init(PORT_SPI, SPEED_SPI);

        /* ИНИЦИАЛИЗАЦИЯ ОБЪЕКТОВ КЛАССА */
            ADC ADC_1 = ADC("ADC_1", MISO_1, LOW, GPIO_IN);
            ADC ADC_2 = ADC("ADC_2", MISO_2, LOW, GPIO_IN);
            ADC ADC_3 = ADC("ADC_3", MISO_3, LOW, GPIO_IN);
            ADC SCLK  = ADC("SCLK",  SCK,    LOW, GPIO_OUT);

    while (true)
    {
        if(spi_is_readable(PORT_SPI)){ /* ПРОВЕРЯЕМ НАЛИЧИЕ ДАННЫХ */
            
        }
    }
    
    std::cout << "Completion of data reading and transmission"  << std::endl;
}