#include "myjob.h"

MyJob::MyJob(QObject *parent) : QObject(parent)
{
    if(wiringPiSetup () == -1) exit (1);
    fd = wiringPiI2CSetup(PCF8591);
    pinMode(6, OUTPUT); // wather pump
    pinMode(29, OUTPUT); // diode

    //We need 3 output pins to control the Max7219: Data, Clock and Load
    pinMode(DATA, OUTPUT);
    pinMode(CLOCK, OUTPUT);
    pinMode(LOAD, OUTPUT);

    /*** MAX7219 module pratameters ***/
    MAX7219Send(INTENSITY, 8);
    MAX7219Send(DECODE_MODE, 0xFF);
    auto timerr = new QTimer(this);
    //connect(tmr,SIGNAL(timeout()),this,SLOT(promeni_lcd()));
    connect(timerr,&QTimer::timeout,this,&MyJob::change_lcd);
    timerr->start(3000);

    /***  PCF8563 RTC module ***/
    rtc=wiringPiI2CSetup(0x51); // I2C address
    QTime tm;
    tm = QTime::currentTime();
    h = tm.hour();
    m = tm.minute();
    s = tm.second();
    init8563_Store[2] = (unsigned char)changeIntToHex(h);
    init8563_Store[1] = (unsigned char)changeIntToHex(m);
    init8563_Store[0] = (unsigned char)changeIntToHex(s);
    QDate dt;
    dt = QDate::currentDate();
    d = dt.daysInMonth();
    mo = dt.month();
    y = dt.year()-2000;
    init8563_Store[3] = (unsigned char)changeIntToHex(d);
    init8563_Store[4] = (unsigned char)changeIntToHex(mo);
    init8563_Store[5] = (unsigned char)changeIntToHex(y);
    P8563_init();
    P8563_settime();
    tmr = new QTimer(this);
    connect(tmr,SIGNAL(timeout()),this,SLOT(P8563_Readtime()));
    tmr->start(1000);

    i = 0;
}

MyJob::~MyJob(){}


double MyJob::temperature()
{
    FILE *ft;
    char tekst[100];
    ft=fopen("/sys/bus/w1/devices/28-020a92466bd5/w1_slave","r");
    if(ft==NULL) exit(1);
    int i=0;
    for(i=0;i<22;i++) //only temperature
    fscanf(ft,"%s", tekst);
    fclose(ft);
    //obrisati „t=”
    for(i=0;i<10;i++) tekst[i]=tekst[i+2];
    int temp=atoi(tekst);
    double tem=(double)temp/1000;
    return tem;
}

void MyJob::P8563_Readtime()
{
    unsigned char time[7];
    WriteBuf[0] = SEC;
    int p;
    wiringPiI2CWrite(rtc, SEC);
    p=read(rtc,time, 7);

    g8563_Store[0] = time[0] & 0x7f;//sec
    g8563_Store[1] = time[1] & 0x7f;//min
    g8563_Store[2] = time[2] & 0x3f;//hour

    g8563_Store[3] = time[3] & 0x3f;//sec
    g8563_Store[4] = time[4] & 0x1f;//min
    g8563_Store[5] = time[5] & 0xff;//hour

    led_sec = changeHexToInt(g8563_Store[0]);
    led_min = changeHexToInt(g8563_Store[1]);
    led_hour = changeHexToInt(g8563_Store[2]);

    dayy = changeHexToInt(g8563_Store[3]);
    monthh = changeHexToInt(g8563_Store[4]);
    yearr = changeHexToInt(g8563_Store[5]);
}

void MyJob::P8563_init()
{
    unsigned char i;
    for(i=0;i<=5;i++)
    g8563_Store[i]=init8563_Store[i];

    P8563_settime();

    /*** initialisation RTC ***/
    WriteBuf[0] = 0x0;
    WriteBuf[1] = 0x00; //normal work mode

    wiringPiI2CWriteReg8(rtc, 0x00, 0x00);
}

void MyJob::P8563_settime()
{
    wiringPiI2CWriteReg8(rtc, 0x02, g8563_Store[0]);
    wiringPiI2CWriteReg8(rtc, 0x03, g8563_Store[1]);
    wiringPiI2CWriteReg8(rtc, 0x04, g8563_Store[2]);
    wiringPiI2CWriteReg8(rtc, 0x05, g8563_Store[3]);
    wiringPiI2CWriteReg8(rtc, 0x07, g8563_Store[4]);
    wiringPiI2CWriteReg8(rtc, 0x08, g8563_Store[5]);
}
void MyJob::print_on_displey_sec(int value)
{
    int p = 0;
    int j = 0;
    for(int i=0; i<value; i++)
    {
      p++;
      if(p==10)
      {
        p=0;
        j++;
      }
   }
   MAX7219Send(1,p);
   MAX7219Send(2,j);
}
void MyJob::print_on_displey_min(int value)
{
    int p = 0;
    int j = 0;
    for(int i=0; i<value; i++)
    {
      p++;
      if(p==10)
      {
        p=0;
        j++;
      }
   }
   MAX7219Send(4,p);
   MAX7219Send(5,j);
}
void MyJob::print_on_displey_hour(int value)
{
    int p = 0;
    int j = 0;
    for(int i=0; i<value; i++)
    {
      p++;
      if(p==10)
      {
        p=0;
        j++;
      }
   }
   MAX7219Send(7,p);
   MAX7219Send(8,j);
}
void MyJob::pump()
{
    digitalWrite(6, LOW);
    delay(5000);
    digitalWrite(6, HIGH);
}
/*void MyJob::diodaa()
{
    digitalWrite(29, HIGH);
    delay(5000);
    digitalWrite(29, LOW);
}*/
int MyJob::read_moisure()
{
    int adcVal;

    wiringPiI2CReadReg8(fd, PCF8591 + 3);
    adcVal = wiringPiI2CReadReg8(fd, PCF8591 + 3);
    moisure_value = moisure_sensor_value(adcVal);

    return moisure_value;
}
int MyJob::change_lcd()
{
    i++;
    if(i == 2)
        i = 0;
    return i;
}
int MyJob::get_sec(){return  led_sec;}
int MyJob::get_min(){return  led_min;}
int MyJob::get_hour(){return  led_hour;}
int MyJob::get_day(){return  dayy;}
int MyJob::get_month(){return  monthh;}
int MyJob::get_year(){return  yearr;}

void MyJob::print_all()
{
    MAX7219Send(SHUTDOWN, 0);
    if(change_lcd() == 0)
    {
        print_on_display(read_moisure());
    }
    else
    {
        MAX7219Send(SHUTDOWN, 0);
        MAX7219Send(SCAN_LIMIT, 7);
        print_on_displey_sec(led_sec);
        MAX7219Send(3, 0x0A);
        print_on_displey_min(led_min);
        MAX7219Send(6, 0x0A);
        print_on_displey_hour(led_hour);
        MAX7219Send(SHUTDOWN, 1);
    }
    MAX7219Send(SHUTDOWN, 1);
}
int MyJob::moisure_sensor_value(int adcVal)
{
    int value;
    if(adcVal < 55)
        adcVal = 55;
    value = 100 - (adcVal - 55)/2;

    return value;
}
void MyJob::print_on_display(int value)
{
    int p = 0;
    int j = 0;
    int k = 0;
    for(int i=0; i<value; i++)
    {
      p++;
      if(p==10)
      {
        p=0;
        j++;
      }
      if(j==10)
      {
        j=0;
        k++;
      }
   }
   MAX7219Send(SCAN_LIMIT, 0);
   MAX7219Send(1,p);
   if(j!=0 || (j==0 && k!=0))
   {
       MAX7219Send(SCAN_LIMIT, 1);
       MAX7219Send(2,j);
    }
   if(k!=0)
    {
       MAX7219Send(SCAN_LIMIT, 2);
       MAX7219Send(3,k);
    }
}
void MyJob::Send16bits(unsigned short output)
{
    unsigned char i;

      for (i=16; i>0; i--)
      {
        unsigned short mask = 1 << (i - 1); // calculate bitmask

        digitalWrite(CLOCK, 0);  // set clock to 0
        /*** Send one bit on the data pin ***/
        if (output & mask)
          digitalWrite(DATA, 1);
        else
          digitalWrite(DATA, 0);

        digitalWrite(CLOCK, 1);  // set clock to 1
      }
}
void MyJob::MAX7219Send(unsigned char reg_number, unsigned char dataout)
{
    digitalWrite(LOAD, 1);  // set LOAD 1 to start
    Send16bits((reg_number << 8) + dataout);   // send 16 bits ( reg number + dataout )
    digitalWrite(LOAD, 0);  // LOAD 0 to latch
    digitalWrite(LOAD, 1);  // set LOAD 1 to finish
}
