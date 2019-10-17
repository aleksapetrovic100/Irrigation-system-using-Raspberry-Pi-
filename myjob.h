#ifndef MYJOB_H
#define MYJOB_H

#include <QObject>
#include <QThread>
#include <QWidget>
#include <QTime>
#include <QTimer>
#include <QDebug>
#include <QtGlobal>

#include "wiringPi.h"
#include<wiringPiI2C.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>

#define changeHexToInt(hex) ((((hex)>>4) *10 ) + ((hex)%16) )
#define changeIntToHex(i) (((i/10) << 4) + i%10)

//adrese registara
#define SEC 0x02
#define MIN 0x03
#define SAT 0x04
#define DAY 0x05
#define MONTH 0x07
#define YEAR 0x08

/***    adc register ***/
const char PCF8591 = 0x48;

/*** MAX7219 pins ***/
#define DATA        0 // GPIO 17 (WiringPi pin num 0)  header pin 11
#define CLOCK       3 // GPIO 22 (WiringPi pin num 3)   header pin 15
#define LOAD        4 // GPIO 23 (WiringPi pin num 4)   header pin 16

/*** MAX7219 registers ***/
#define DECODE_MODE   0x09
#define INTENSITY     0x0a
#define SCAN_LIMIT    0x0b
#define SHUTDOWN      0x0c
#define DISPLAY_TEST  0x0f

class MyJob : public QObject
{
    Q_OBJECT
public:
    explicit MyJob(QObject *parent = nullptr);
    ~MyJob();

    int read_moisure();
    int moisure_sensor_value(int);
    void pump();
    double temperature();
    static void Send16bits (unsigned short);
    static void MAX7219Send (unsigned char, unsigned char);
    void print_all();
    void print_on_display(int);
    void print_on_displey_sec(int);
    void print_on_displey_min(int);
    void print_on_displey_hour(int);
    int get_sec();
    int get_min();
    int get_hour();
    int get_day();
    int get_month();
    int get_year();

private slots:
    void P8563_Readtime();

private:
    void P8563_init();
    void P8563_settime();
    int change_lcd();

    QTimer *tmr;
    QTimer *timerr;

    unsigned char WriteBuf[6];
    unsigned char ReadBuf;
    unsigned char g8563_Store[6];
    unsigned char init8563_Store[6];

    int fd;
    int moisure_value;
    int i;
    int s, m, h;
    int d,mo,y;
    int led_sec, led_min, led_hour;
    int dayy, monthh, yearr;
    int rtc;
    int do24;
};

#endif // MYJOB_H
