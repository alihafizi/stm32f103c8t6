#include "stm32f10x.h"

void systemclockinit(void);
void usart1init(void);

int main(){
	systemclockinit();
	usart1init();
	
	GPIOA->CRH &= ~GPIO_CRH_CNF9 & ~ GPIO_CRH_CNF10;	
	GPIOA->CRH |= GPIO_CRH_CNF10_0 | GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9;
	
	uint8_t data;
	
	while (1){
		while (!(USART1->SR & USART_SR_RXNE));
		data = USART1->DR;
		while (!(USART1->SR & USART_SR_TXE));	
		USART1->DR = data;
		
	}
	
}

void usart1init(void){
	USART1->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
	USART1->BRR = 0x1D4C;
}


void systemclockinit(void){

	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN;
	
	RCC->CR |= RCC_CR_HSEON;
	while (!(RCC->CR & RCC_CR_HSEON));
	RCC->CR |= RCC_CR_PLLON;
	while (!(RCC->CR & RCC_CR_PLLRDY));
	
	RCC->CFGR |= RCC_CFGR_PLLSRC_HSE;
	RCC->CFGR |= RCC_CFGR_PLLMULL9;
	RCC->CFGR |= RCC_CFGR_SW_PLL;
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
	
	//LSE Enable
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_DBP;
	RCC->BDCR |= RCC_BDCR_LSEON;
	while (!(RCC->BDCR & RCC_BDCR_LSERDY));
	RCC->BDCR |= RCC_BDCR_RTCSEL_LSE;
	RCC->BDCR |= RCC_BDCR_RTCEN;
}