/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "DigitalInOut.h"
#include "PinNameAliases.h"
#include "ThisThread.h"
#include "Thread.h"
#include "cmsis_os2.h"

// Alias
#define Ds D4
#define STCP D3
#define SHCP D2

#define tiempo 500ms

// prototipos
void shift_data(char data);
void load_data(char data);
void clear_data(void);
void send_data(void); //funcion de hilo

//hilos y herramientas del sistema
Thread T_send_data(osPriorityNormal, 4096, NULL, NULL);

//Variables globales 
char data_int;

//pines y puertos 
DigitalOut Serial_data(Ds);
DigitalOut Serial_Clk(SHCP);
DigitalOut LatchClk(STCP);

void alternante(void) {
    char data[] = {0xAA, 0x55}; 
    while (true) {
        for (int i = 0; i < 2; i++) {
            load_data(data[i]);
            ThisThread::sleep_for(tiempo);
        }
    }
}

void desplazamiento_derecha(void) {
    char data = 0x01;
    while (true) {
        for (int i = 0; i < 24; i++) {
            load_data(data);
            data = data << 1;
            ThisThread::sleep_for(tiempo);
        }
        data = 0x01; 
    }
}

void desplazamiento_izquierda(void) {
    char data = 0x80;
    while (true) {
        for (int i = 0; i < 24; i++) {
            load_data(data);
            data = data >> 1;
            ThisThread::sleep_for(tiempo);
        }
        data = 0x80; 
    }
}

void extremos(void) {
    char data[] = {0x81, 0x42, 0x24, 0x18}; 
    while (true) {
        for (int i = 0; i < 4; i++) {
            load_data(data[i]);
            ThisThread::sleep_for(tiempo);
        }
    }
}

void barrido(void) {
    while (true) {
        char data = 0x01;
        for (int i = 0; i < 24; i++) {
            load_data(data);
            data = data << 1;
            ThisThread::sleep_for(tiempo);
        }
        data = 0x80;
        for (int i = 0; i < 24; i++) {
            load_data(data);
            data = data >> 1;
            ThisThread::sleep_for(tiempo);
        }
    }
}

void send_data(void) {
    alternante(); 
}

int main()
{
    //inicializar 
    Serial_data = 0;
    Serial_Clk = 0;
    LatchClk = 0;
    clear_data();
    //encender los hilos 
    T_send_data.start(send_data);
    while(true)
    {
        //Idle
    }
}

void shift_data(char data)
{
    for (int i=0; i<24;i++)
    {
        Serial_data = ((data&0x01) == 0x01) ? 1:0;
        Serial_Clk = 0;
        wait_us(1);
        Serial_Clk= 1;
        wait_us(1);
        data = data >> 1;
    }
}

void load_data(char data)
{
    LatchClk = 0;
    shift_data(data);
    LatchClk = 1;
}

void clear_data(void)
{
    char data = 0x00;
    data = 0x00;
    load_data(data);
}