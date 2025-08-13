# 🚗 Projeto GPS com MSP430

Este projeto utiliza um **microcontrolador MSP430** e um **módulo GPS Neo-6M** para capturar, processar e exibir informações de localização e velocidade em tempo real.  
Ele foi desenvolvido como aplicação prática para o aprendizado de **sistemas embarcados**, **protocolos de comunicação serial** e **processamento de dados de telemetria**.

## 📌 Objetivos
- Capturar dados de latitude, longitude e velocidade a partir do módulo GPS.
- Exibir as informações processadas em um display LCD.
- Implementar comunicação via UART entre o MSP430 e o módulo GPS.
- Trabalhar conceitos de parsing de dados NMEA.

## 🛠 Tecnologias e Componentes
- **Microcontrolador:** MSP430 (modelo MSP430F5529)
- **Módulo GPS:** u-blox Neo-6M
- **Display:** LCD 16x2 com interface I2C (PCF8574T)
- **Linguagem:** C
- **Ambiente de desenvolvimento:** Code Composer Studio

## ⚙️ Funcionamento
1. O módulo GPS envia as frases NMEA via UART para o MSP430.
2. O firmware filtra as sentenças relevantes (`GPRMC` e `GPGGA`).
3. São extraídas informações de **latitude**, **longitude** e **velocidade**.
4. Os dados processados são exibidos no LCD.

## 📡 Esquema de Ligações

| Componente | Pino MSP430 | Pino Módulo GPS/LCD |
|------------|-------------|---------------------|
| GPS TX     | P3.4 (UCA1RXD) | TX do GPS |
| GPS RX     | P3.5 (UCA1TXD) | RX do GPS |
| LCD SDA    | P4.1         | SDA (PCF8574T) |
| LCD SCL    | P4.2         | SCL (PCF8574T) |
| VCC        | 3.3V         | Alimentação GPS/LCD |
| GND        | GND          | GND comum |

> ⚠️ Importante: o módulo GPS opera a 3,3V no MSP430, sem necessidade de conversão de nível lógico.

## 📂 Estrutura do Repositório
