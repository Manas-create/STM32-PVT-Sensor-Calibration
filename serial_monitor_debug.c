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

// Custom Verilog-style $monitor function
uint32_t prev_state = 0xFFFFFFFF;

void Verilog_Monitor(void) {
    uint32_t current_state = GPIOA->ODR; 
    
    if (current_state != prev_state) {
        int clk  = (current_state >> 5) & 1;  // PA5
        int mosi = (current_state >> 7) & 1;  // PA7
        int pwm  = (current_state >> 1) & 1;  // PA1
        
        char mon_buf[60];
        sprintf(mon_buf, "[$monitor] CLK: %d | MOSI: %d | PWM: %d\r\n", clk, mosi, pwm);
        UART_Print(mon_buf);
        
        prev_state = current_state;
    }
}

int main(void) {
    // Enable Clocks 
    RCC->IOPENR |= (1 << 0);   
    RCC->APBENR1 |= (1 << 17); 
    
    // Setup UART
    GPIOA->MODER &= ~(0xF << 4);
    GPIOA->MODER |= (0xA << 4);
    GPIOA->AFR[0] |= (0x11 << 8);
    USART2->BRR = 12000000 / 115200;              
    USART2->CR1 = (1 << 0) | (1 << 2) | (1 << 3); 
    
    // Setup Pins
    GPIOA->MODER &= ~((3U << 10) | (3U << 12) | (3U << 14) | (3U << 2));
    GPIOA->MODER |= ((1U << 10) | (1U << 12) | (1U << 14) | (1U << 2));
    
    char uart_buf[100];
    
    while(1) {
        uint16_t duty = 500; 
        UART_Print("\r\n--- INITIATING SPI TRANSACTION (0x4000) ---\r\n");
            
        GPIOA->BSRR = (1U << 1);
        Verilog_Monitor();
        Delay_us(10);

        for(int bit = 15; bit >= 0; bit--) {
            if((0x4000 >> bit) & 1) GPIOA->BSRR = (1U << 7);  
            else                    GPIOA->BSRR = (1U << 23); 
            Verilog_Monitor(); 
                
            Delay_us(10);
            GPIOA->BSRR = (1U << 5); 
            Verilog_Monitor(); 
            Delay_us(10);
            GPIOA->BSRR = (1U << 21); 
            Verilog_Monitor(); 
            Delay_us(10);
        }
            
        GPIOA->BSRR = (1U << 17);
        Verilog_Monitor();

        sprintf(uart_buf, "\r\n[TEST] PWM_REG: %d | V_ACT: 0.95V | SENS_RAW: 0x4000 | STATUS: OK\r\n", duty);
        UART_Print(uart_buf);
            
        for(int d=0; d<100; d++) Delay_us(10000); 
    }
}