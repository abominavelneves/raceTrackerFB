#include "i2c_lcd.h"
#include <msp430.h>
#include <stdint.h>

// Funções internas para o I2C
void i2c_init(void);
void i2c_write(uint8_t address, uint8_t *data, uint8_t length);
void lcd_send_nibble(uint8_t nibble, uint8_t is_data);
void lcd_send_byte(uint8_t byte, uint8_t is_data);

// Variáveis globais para o estado do I2C e do backlight
uint8_t lcd_backlight_state = 0x08; // Backlight é o bit 3 (0b00001000)

void i2c_init(void) {
    // Configura os pinos P4.1 e P4.2 para a função I2C
    P4SEL |= BIT1 + BIT2;

    // Configuração do módulo I2C (UCB1)
    UCB1CTL1 |= UCSWRST;                      // Habilita reset de software
    UCB1CTL0 = UCMST + UCMODE_3 + UCSYNC;      // Mestre I2C, modo síncrono
    UCB1CTL1 = UCSSEL_2 + UCSWRST;             // Usa SMCLK, mantém reset
    UCB1BR0 = 10;                             // fSCL = SMCLK/10 ~= 100kHz (com SMCLK = 1MHz)
    UCB1BR1 = 0;
    UCB1CTL1 &= ~UCSWRST;                     // Libera o reset de software
}

void i2c_write(uint8_t address, uint8_t *data, uint8_t length) {
    UCB1I2CSA = address;                      // Define o endereço do escravo
    UCB1CTL1 |= UCTR + UCTXSTT;               // Modo de transmissão, gera START

    while (!(UCB1IFG & UCTXIFG));             // Espera o buffer de TX estar pronto

    int i;
    for (i = 0; i < length; i++) {
        UCB1TXBUF = data[i];
        while (!(UCB1IFG & UCTXIFG));
    }

    UCB1CTL1 |= UCTXSTP;                      // Gera condição de STOP
    UCB1IFG &= ~UCTXIFG;
    while (UCB1CTL1 & UCTXSTP);               // Espera o STOP ser completado
}

void lcd_send_nibble(uint8_t nibble, uint8_t is_data) {
    uint8_t byte_to_send = (nibble << 4) | (is_data ? 0x01 : 0x00) | lcd_backlight_state;
    i2c_write(LCD_I2C_ADDR, &byte_to_send, 1);
    // Pulso no pino Enable (EN)
    byte_to_send |= 0x04; // EN = 1
    i2c_write(LCD_I2C_ADDR, &byte_to_send, 1);
    __delay_cycles(200);
    byte_to_send &= ~0x04; // EN = 0
    i2c_write(LCD_I2C_ADDR, &byte_to_send, 1);
    __delay_cycles(500);
}

void lcd_send_byte(uint8_t byte, uint8_t is_data) {
    lcd_send_nibble(byte >> 4, is_data);   // Envia o nibble superior
    lcd_send_nibble(byte & 0x0F, is_data); // Envia o nibble inferior
}

void lcd_init(void) {
    i2c_init();
    __delay_cycles(50000); // Atraso de inicialização

    // Sequência de inicialização para o modo 4 bits
    lcd_send_nibble(0x03, 0);
    __delay_cycles(5000);
    lcd_send_nibble(0x03, 0);
    __delay_cycles(200);
    lcd_send_nibble(0x03, 0);
    __delay_cycles(200);
    lcd_send_nibble(0x02, 0); // Define para modo 4 bits

    lcd_send_command(0x28); // Função: 4 bits, 2 linhas, fonte 5x8
    lcd_send_command(0x0C); // Display ON, cursor OFF, blink OFF
    lcd_send_command(0x06); // Modo de entrada: incrementa cursor
    lcd_send_command(0x01); // Limpa o display
    __delay_cycles(2000);
}

void lcd_send_command(uint8_t command) {
    lcd_send_byte(command, 0);
}

void lcd_send_data(uint8_t data) {
    lcd_send_byte(data, 1);
}

void lcd_print(char *str) {
    while (*str) {
        lcd_send_data(*str++);
    }
}

void lcd_set_cursor(uint8_t col, uint8_t row) {
    uint8_t address = (row == 0) ? 0x80 + col : 0xC0 + col;
    lcd_send_command(address);
}

void lcd_clear(void) {
    lcd_send_command(0x01);
    __delay_cycles(2000); // Limpar o display leva mais tempo
}

void lcd_backlight(uint8_t on) {
    lcd_backlight_state = on ? 0x08 : 0x00;
    // Envia um byte vazio para atualizar o estado do backlight
    uint8_t dummy = lcd_backlight_state;
    i2c_write(LCD_I2C_ADDR, &dummy, 1);
}
