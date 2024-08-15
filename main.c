#include "stm32f10x.h"
#include "stdio.h"
void USART1_Init(void) {
    // Enable clocks for USART1 and GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN;

    // Configure PA9 (TX) as alternate function push-pull
    GPIOA->CRH &= ~GPIO_CRH_CNF9;
    GPIOA->CRH |= GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9;

    // Configure USART1
    USART1->BRR = 0x1D4C;  // Set baud rate to 9600 (assuming 72MHz clock)
    USART1->CR1 |= USART_CR1_UE | USART_CR1_TE;  // Enable USART and TX
}

void USART1_SendChar(char c) {
    while (!(USART1->SR & USART_SR_TXE));  // Wait until TX is empty
    USART1->DR = c;  // Send character
}

void USART1_SendString(char* str) {
    while (*str) {
        USART1_SendChar(*str++);
    }
}

void ADC1_Init(void) {
    // Enable clocks for ADC1 and GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN | RCC_APB2ENR_IOPAEN;

    // Configure PA0 (ADC Channel 0) as analog input
    GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);

    // Configure ADC1
    ADC1->CR2 |= ADC_CR2_ADON;  // Turn on ADC1
    ADC1->SQR3 |= (0 << ADC_SQR3_SQ1);  // Select channel 0
    ADC1->CR2 |= ADC_CR2_CAL;  // Start calibration
    while (ADC1->CR2 & ADC_CR2_CAL);  // Wait for calibration to complete
}

uint16_t ADC1_Read(void) {
    ADC1->CR2 |= ADC_CR2_SWSTART;  // Start conversion
    while (!(ADC1->SR & ADC_SR_EOC));  // Wait for conversion to complete
    return ADC1->DR;  // Return the converted value
}

float LM35_ReadTemperature(void) {
    uint16_t adcValue = ADC1_Read();
    float voltage = adcValue * 3.3 / 1024;  // Convert ADC value to voltage
    float temperature = voltage * 100;  // Convert voltage to temperature
    return temperature;
}

int main(void) {
    USART1_Init();
    ADC1_Init();

    char buffer[50];

    while (1) {
        float temperature = LM35_ReadTemperature();
        sprintf(buffer, "Temperature: %.2f C\r\n", temperature);
        USART1_SendString(buffer);
        
        for (volatile int i = 0; i < 1000000; i++);  // Delay
    }
}
