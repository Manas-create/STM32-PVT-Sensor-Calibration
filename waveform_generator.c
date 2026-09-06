#include "stm32c0xx.h"
#include <stdio.h>

void Delay_us(uint32_t us) {
    for (volatile uint32_t i = 0; i < us * 2; i++); 
}

void UART_Print(const char *str) {
    while (*str) {
        while (!(USART2->ISR & 0x0080)); 
        USART2->TDR = *str++;
    }
}

int main(void) {
    // Enable Clocks
    RCC->IOPENR |= (1 << 0);   
    RCC->APBENR1 |= (1 << 17); 
    
    // Setup UART for Serial Monitor
    GPIOA->MODER &= ~(0xF << 4);
    GPIOA->MODER |= (0xA << 4);
    GPIOA->AFR[0] |= (0x11 << 8);
    USART2->BRR = 12000000 / 115200;                 
    USART2->CR1 = (1 << 0) | (1 << 2) | (1 << 3); 
    
    // Setup PA5(CLK), PA6(MISO), PA7(MOSI) for SPI, and PA1(PWM)
    GPIOA->MODER &= ~((3U << 10) | (3U << 12) | (3U << 14) | (3U << 2));
    GPIOA->MODER |= ((1U << 10) | (1U << 12) | (1U << 14) | (1U << 2));

    UART_Print("========================================\r\n");
    UART_Print("[SYS] Generating Synchronized Waveforms\r\n");
    UART_Print("========================================\r\n");
    
    while(1) {
        // 1. Turn PWM (PA1) HIGH
        GPIOA->BSRR = (1U << 1);  
        Delay_us(50); 
        
        // 2. Execute SPI burst (0x4000)
        uint16_t mosi_payload = 0x4000;       
        uint16_t simulated_miso = 0xAAAA;     
        
        for(int bit = 15; bit >= 0; bit--) {
            // Write MOSI (PA7)
            if((mosi_payload >> bit) & 1) GPIOA->BSRR = (1U << 7);  
            else                          GPIOA->BSRR = (1U << 23); 
            
            // Force Simulated MISO (PA6)
            if((simulated_miso >> bit) & 1) GPIOA->BSRR = (1U << 6);  
            else                            GPIOA->BSRR = (1U << 22); 
            
            Delay_us(10);
            
            // Toggle Clock (PA5)
            GPIOA->BSRR = (1U << 5);  // HIGH
            Delay_us(10);
            GPIOA->BSRR = (1U << 21); // LOW
            Delay_us(10);
        }
        
        Delay_us(50); 
        
        // 3. Turn PWM (PA1) LOW
        GPIOA->BSRR = (1U << 17); 
        
        UART_Print("[TEST] Waveform generated successfully.\r\n");
        
        // Wait 1 second
        for(int d=0; d<100; d++) Delay_us(10000); 
    }
}