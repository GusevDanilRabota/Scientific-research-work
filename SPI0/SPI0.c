/* Подключение требуемых директорий */
#include <stdio.h>
#include "hardware/spi.h"
#include "hardware/uart.h"
#include "hardware/base.h"
#include "pico/stdlib.h" /* pico/stdlib.h включает в себя  hardware_uart, hardware_gpio, pico_binary_info, pico_runtime, pico_ platform, pico_printf, 
    pico_stdio, pico_standart_link, pico_util*/




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
#define SPEED 2000000 /* 2 000 000 Гц -> 2 МГц */




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
io_rw_32 *some_reg;
long result = 0;
byte register_value = getRegister(); //Variable that stores the config register value. Fill it up with the current reg value
int byteMask = 0b00000000; //Masking byte for writing only 1 register at a time




/* Инициализация пинов SPI */
void SETTING_PIN_SPI(){
    /* Задать пинам требуемый функционал */

    spi_init(SPI_PORT, SPEED); /* Инициализирует экземпляры SPI. Переводит SPI в известное состояние и включает его. Необходимо вызывать перед другими функциями */

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
}




/* Инициализация пинов UART */
void SETTING_PIN_UART(){
    /* Задать пинам требуемый функционал */

    spi_init(PORT_UART, 115200);

    gpio_set_funcction(RX, UART_FUNCSEL_NUM(uart0, 1));
    gpio_set_funcction(TX, UART_FUNCSEL_NUM(uart0, 0));
}




/* Защитная последовательность из 3 импульсов */
void PROTECTIVE_IMPULSE(){
    for(int i; i < 3; i++){
        gpio_put(SCK, HIGH);
        DELAY_410NS;
        gpio_put(SCK, LOW);
        DELAY_410NS;
    }
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




/* Чтение данных с АЦП - вторая версия */
long SPI_READ_DATA(SC){
    gpio_put(SC, LOW); /* Установка низкого логического уровня на линии SS */

    for (int i = 0; i < 24; i++){//Read the 24-bits        
        gpio_put(SCK, HIGH);
        DELAY_410NS;

        result |= digitalRead(SC) << (23 - i); //Read a bit and shift

        gpio_put(SCK, HIGH);
        DELAY_410NS;
    }

    PROTECTIVE_IMPULSE();
    PROTECTIVE_IMPULSE();
    PROTECTIVE_IMPULSE();
}



int getRegister(SC) //Reads all registers. The actual values will be fetched with other, simple functions
{  
    byte registerValue; //Variable that stores the config register value

    //Shift out 27 (24+3) bits
    ADCread = readADC(); //32-bit variable that stores the whole ADC reading
    
    gpio_set_dir(SC, GPIO_OUT); //After the 27th SCLK pulse, set DOUT to OUTPUT

    PROTECTIVE_IMPULSE();
    PROTECTIVE_IMPULSE()

    for (uint8_t i = 0; i < 7; i++) //SCLK 30-36, sending READ word
    {
        gpio_put(SCK, HIGH);
        DELAY_410NS;
        digitalWrite(DOUT_DRDY, ((0x56 >> (6 - i)) & 0b00000001)); //0x56 - READ
        gpio_put(SCK, LOW);
        DELAY_410NS;
    }    

    PROTECTIVE_IMPULSE();
    
    //After the 37th SCLK pulse switch the direction of DOUT. 
    gpio_set_dir(SC, GPIO_IN); //we read, so dout becomes INPUT

    registerValue = 0; //Because we are reading

    for (uint8_t i = 0; i < 8; i++) //38-45 SCLK pulses
    {
        gpio_put(SCK, HIGH);
        DELAY_410NS;
        registerValue |= digitalRead(DOUT_DRDY) << (7 - i); //read out and shift the values into the register_value variable        
        gpio_put(SCK, LOW);
        DELAY_410NS;
    }

    // send 1 clock pulse, to set the Pins of the ADCs to output and pull high
    PROTECTIVE_IMPULSE();

    // At the 46th SCLK, switch DRDY / DOUT to output and pull up DRDY / DOUT. 
    gpio_set_dir(SC, GPIO_IN); //Ready to receive the DRDY to perform a new acquisition

    return registerValue;
}



void setRegister(int registertowrite, int valuetowrite, SC)
{
    //"Arbitrary" register numbers
    //0 - Channel
    //1 - PGA
    //2 - Speed
    //3 - REF

    //Config register structure
    //bit 0-1 : Channel. 00 - A, 01 - reserved, 10 - Temperature, 11 - internal short (maybe for offset calibration?)
    //bit 2-3 : PGA. 00 - 1, 01 - 2, 10 - 64, 11 - 128
    //bit 4-5 : speed. 00 - 10 Hz, 01 - 40 Hz, 10 - 640 Hz, 11 - 1280 Hz
    //bit 6   : Reference. Default is enabled which is 0.
    //bit 7   : reserved, don't touch
    //----------------------------------------------------------
    
    switch (registertowrite)
    {
        case 0: //channel
        byteMask = 0b11111100; //when using & operator, we keep all, except channel bits
        register_value = register_value & byteMask; //Update the register_value with mask. This deletes the first two
        
            switch (valuetowrite)
            {
            case 0: // A // W0 0
                register_value = register_value | 0b00000000; //Basically keep everything as-is
                printf("Channel = 0");
                break;
            case 1: // Reserved // W0 1
                //dont implement it!
                printf("Channel = Reserved, invalid!");
                break;
            case 2: //Temperature // W0 2
                register_value = register_value | 0b00000010;
                printf("Channel = Temp");
                break;
            case 3: //Internal short //W0 3
                register_value = register_value | 0b00000011;
                printf("Channel = Short");
                break;
            }      
        break;

        case 1: //PGA
            byteMask = 0b11110011; //when using & operator, we keep all, except channel bits
            register_value = register_value & byteMask; //Update the register_value with mask. This deletes the first two

            switch (valuetowrite)
            {
            case 0: // PGA 1 //W1 0
                register_value = register_value | 0b00000000; //Basically keep everything as-is
                pga_divider = 1;
                printf("PGA = 1");
                break;
            case 1: // PGA 2 //W1 1
                register_value = register_value | 0b00000100;
                pga_divider = 2;
                printf("PGA = 2");
                break;
            case 2: //PGA 64 //W1 2
                register_value = register_value | 0b00001000;
                pga_divider = 64;
                printf("PGA = 64");
                break;
            case 3: //PGA 128 //W1 3
                register_value = register_value | 0b00001100;
                pga_divider = 128;
                printf("PGA = 128");
                break;
            }
            break;
            //-------------------------------------------------------------------------------------------------------------

        case 2: //DRATE
            byteMask = 0b11001111; //when using & operator, we keep all, except channel bits
            register_value = register_value & byteMask; //Update the register_value with mask. This deletes the first two

            switch (valuetowrite)
            {
            case 0: // 10 Hz //W2 0
                register_value = register_value | 0b00000000; //Basically keep everything as-is
                printf("DRATE = 10 Hz");
                break;
            case 1: // 40 Hz //W2 1
                register_value = register_value | 0b00010000;
                printf("DRATE = 40 Hz");
                break;
            case 2: //640 Hz //W2 2
                register_value = register_value | 0b00100000;
                printf("DRATE = 640 Hz");
                break;
            case 3: //1280 Hz //W2 3
                register_value = register_value | 0b00110000;
                printf("DRATE = 1280 Hz");
                break;
            }
            break;
            //-------------------------------------------------------------------------------------------------------------
        case 3: //VREF
            if (valuetowrite == 0) //W3 0
            {
                bitWrite(register_value, 6, 0); //Enable
                printf("VREF ON");
            }
            else if (valuetowrite == 1) //W3 1
            {
                bitWrite(register_value, 6, 1); //Disable
                printf("VREF OFF");
            }
            else {}//Other values wont trigger anything
            break;
    }

    //Shift out 27 (24+3) bits
    ADCread = readADC(); //32-bit variable that stores the whole ADC reading

    gpio_set_dir(SC, GPIO_OUT); //After the 27th SCLK pulse, set DOUT to OUTPUT

    PROTECTIVE_IMPULSE();
    PROTECTIVE_IMPULSE();

    for (uint8_t i = 0; i < 7; i++) //SCLK 30-36, sending READ word
    {
        gpio_put(SCK, HIGH);
        DELAY_410NS;
        digitalWrite(DOUT_DRDY, ((0x65 >> (6 - i)) & 0b00000001)); //0x65 - WRITE
        gpio_put(SCK, LOW);
        DELAY_410NS;
    }

    PROTECTIVE_IMPULSE();

    for (uint8_t i = 0; i < 8; i++) //38-45 SCLK pulses
    {
        gpio_put(SCK, HIGH);
        DELAY_410NS;
        digitalWrite(DOUT_DRDY, ((register_value >> (7 - i)) & 0b00000001)); //Write the register values        
        gpio_put(SCK, LOW);
        DELAY_410NS;
    }

    // send 1 clock pulse, to set the Pins of the ADCs to output and pull high
    PROTECTIVE_IMPULSE();

    // At the 46th SCLK, switch DRDY / DOUT to output and pull up DRDY / DOUT. 
    gpio_set_dir(SC, GPIO_IN);
}




void main()
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
             //   нет смысла продолжать выполнение программы */
        for(int SC: {2, 7, 24}){
            printf("Work with SCn", SC);
            if (Serial.available() > 0){
                char commandCharacter = Serial.read();
                switch (commandCharacter){
                    case 'R':
                    {
                        byte regvalues = getRegister();
                        Serial.print("Register: 0b"); 
                        for (int i = 7; i >= 0; i--){Serial.print((regvalues >> i) & 1);}
                        Serial.println();
                        break;
                    }
                        

                    case 'A':
                    {
                        while (Serial.read() != 's'){
                            long adcValue = readADC();
                            if (adcValue >> 23 == 1){adcValue = adcValue - 16777216;}
                            float voltageValue = (1250.0 / pga_divider) * ((float)adcValue / (8388607.0));
                            Serial.print("RAW-ADC reading: ");
                            Serial.println(adcValue);
                            Serial.print("Voltage reading: ");
                            Serial.println(voltageValue, 10);
                            break;                 
                        }
                    }

                    case 'W':
                    {
                        while (!Serial.available());
                        int registerToWrite = Serial.parseInt();
                        delay(100);
                        while (!Serial.available());
                        int valueToWrite = Serial.parseInt();
                        delay(100);
                        setRegister(registerToWrite, valueToWrite);
                        break;
                    }
                }
            }
        }        
    }
}