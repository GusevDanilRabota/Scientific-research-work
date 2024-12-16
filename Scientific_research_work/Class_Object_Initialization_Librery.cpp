/* СТАНДАРТНЫЕ БИБЛИОТЕКИ C++*/
    #include <iostream>
    #include <stdio.h>
    #include <string.h>
    #include <stdint.h>

/* ДОПОЛНИТЕЛЬНЫЕ БИБЛИОТЕКИ RASPBERRY PI PICO*/
    #include <pico/stdlib.h>
    #include <hardware/spi.h>
    #include <pico/binary_info.h>
//================================================================================================================================================================================================================




/* ВРЕМЕННАЯ КОНСТАНТА */
    #define DELAY_410NS asm volatile ("nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t")

/* ОПРЕДЕЛЕНИЕ ГЛОБАЛЬНЫХ ПЕРЕМЕННЫХ */
    /* ПЕРЕМЕННЫЕ ДЛЯ ЗАПИСИ И ПЕРЕДАЧИ */
        #define BUF_LEN 32
        #define SPI_RW_LEN 1
        u_int32_t Rx_buf [BUF_LEN], Tx_buf [BUF_LEN];

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
//================================================================================================================================================================================================================




/* ОПИСАНИЕ КЛАССА АНАЛОГО-ЦИФРОВОГО ПРЕОБРАЗОВАТЕЛЯ 24 РАЗРЯДОВ */
class ADC{
private: /* ДЕЛАЕМ ИХ ДОСТУПНЫМИ ТОЛЬКО ВНУТРИ КЛАССА, ЧТОБЫ ИСПОЛЬЗОВАТЬ ТОЛЬКО ЧЕРЕЗ КОНСТРУКТОР И НЕ ВЫЗВАТЬ ОШИБОК ПРИ НЕПРАВЕЛЬНОЙ ИНИЦИАЛИЗАЦИИ*/
    std::string name;
    u_int8_t pin;

    /* ИНИЦИАЛИЗАЦИЯ ПОРТА */
    void set_pin_adc(u_int8_t pin_number, bool strength_signal, bool mode_operating){ /* ИНИЦИАЛИЗАЦИЯ ПИНА */
        gpio_set_function(pin_number, 1); /* 1 - GPIO_FUNC_SPI */
        gpio_set_dir(pin_number, mode_operating);
        gpio_put(pin_number, strength_signal);
    }

public:
    bool flag = true;
    bool signal_strength; /* LOW или HIGH */
    bool operating_mode; /* GPIO_IN или GPIO_OUT */
    ADC(std::string p_name; u_int8_t p_pin; bool p_signal_strength; bool p_operating_mode);
    u_int32_t read(u_int8_t pin_number)
};

/* КОНСТРУКТОР */
ADC::ADC(std::string p_name; u_int8_t p_pin; bool p_signal_strength; bool p_operating_mode){ /* ВСЁРАВНО МОГУТ БЫТЬ НЕКОРРЕКТНЫЕ ЗНАЧЕНИЯ ПРИ ВВОДЕ ПАРАМЕТРОВ КОНСТРУКТОРА */
    name = p_name;
    while (flag){
        if (p_pin >= 0 && p_pin <= 40)
            pin = p_pin;
            flag = false;
        else
            std::cout << "Invalid pin number" << "\n" << std::endl;
    }
    (p_pin >= 0 && p <= 40)
        pin = p_pin;
    signal_strength = p_signal_strength;
    operating_mode = p_operating_mode;
    set_pin_adc(p_pin, p_signal_strength, p_operating_mode); /* ВЫЗОВ МЕТОДА ИЗ КОНСТРУКТОРА ВОЗМОЖЕН ТАК КАК МЕТОД НЕ ВИРТУАЛЬНЫЙ И ФУНКЦИИ ИНИЦИАЛИЗАЦИИ ВОЗМОЖНО ВЫЗЫВАТЬ ИЗ КОНСТРУКТОРА */
}

/* ФУНКЦИЯ ЧТЕНИЯ */
u_int32_t ADC::read(u_int8_t pin_number){ /* Принятые данные сохраняются во внутреннем буфере Rx, в то время как при передаче данные предварительно сохраняются во внутренний буфер Tx,после чего они передаются. 
Доступ на чтение к регистру SPI_DR возвратит буферизированное значение Rx, в то время как доступ на запись к SPI_DR сохранит записанные данные в буфер Tx.*/
/* Однонаправленный master, только прием (BIDIMODE=0, RXONLY=1). Процесс начнется, как только SPE=1, активированный приемник начнет принимать данные через ножку MISO и последовательно вдвигать из в регистр сдвига. После окончания приема данные параллельно загружаются в регистр SPI_DR (буфер Rx). */
/* Обработка передачи и приема данных. Флаг TXE (Tx buffer empty, буфер передачи пуст) установится, когда записанные в буфер Tx данные были перенесены в регистр сдвига. Факт установки этого показывает, что внутренний буфер Tx готов к загрузке следующей порции данных. В момент установки TXE может генерироваться прерывание, если установлен бит TXEIE в регистре SPI_CR2. Очистка бита TXE выполняется путем записи в регистр SPI_DR.
Примечание: программа должна обеспечить состояние лог. 1 флага TXE перед любой попыткой записи в буфер Tx (регистр SPI_DR). Иначе операция записи перезапишет данные буфера передачи до того, как они были реально отправлены. Флаг RXNE (Rx buffer not empty, буфер приема не пуст) установится на последнем тактовом перепаде приема бит, когда данные переносятся из регистра сдвига в буфер Rx (регистр SPI_DR). Факт установки этого флага показывает, что принятые данные готовы к чтению через регистр SPI_DR. В момент установки флага RXNE может быть сгенерировано прерывание, если установлен бит RXNEIE в регистре SPI_CR2. Очистка бита RXNE выполняется путем чтения регистра SPI_DR.
Для некоторых конфигураций может использоваться флаг BSY во время последней транзакции данных, чтобы ждать завершения транзакции. */
    for(u_int32_t i = 0; i < BUF_LEN; ++i){
        Rx_buf[i] = 0;
        Tx_buf[i] = 0;
    }
    std::cout << "Reading data from SPI - " << name << "\n" << std::endl;
    spi_read_blocking(PORT_SPI, 0, in_buf, SPI_RW_LEN);
    std::cout << "Date received: %d\n", in_buf[0] <<
    printf ("Data received: %d\n", in_buf [0]);
}
//================================================================================================================================================================================================================




int main(){
    /* ИНИЦИАЛИЗАЦИЯ  */
        /* ПОРТОВ ПЛАТЫ Raspberry Pi Pico */
            stdio_init_all();
            spi_init(PORT_SPI, SPEED_SPI);

        /* ОБЪЕКТОВ КЛАССА */
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