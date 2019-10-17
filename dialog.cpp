#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    /*** SET TIMER ***/
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_timer_Update()));
    timer->start(1000);

    /*** DEFAULT: AUTOMATIC IRRIGATION IS DISABLED***/
    automatic = false;
    enable = false;
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_wather_pump_activate_clicked()
{
    QFuture<void> test = QtConcurrent::run(&this->mJob, &MyJob::pump); //enable concurrent execution of funtion pumpaa
}

void Dialog::on_timer_Update()
{
    int vlaga = mJob.read_moisure();

    if(automatic)
    {
        if(vlaga < prag)
            QFuture<void> test = QtConcurrent::run(&this->mJob, &MyJob::pump);
    }

    QFuture<void> test2 = QtConcurrent::run(&this->mJob, &MyJob::print_all); //enable concurrent execution of funtion ispis_na_dislej

    /*** get P8563 time module parameters ***/
    QString s_str, m_str, h_str;
    s_str = QString::number(mJob.get_sec());
    m_str = QString::number(mJob.get_min());
    h_str = QString::number(mJob.get_hour());

    /*** print on dialog.ui ***/
    ui->soil_moisture->display(vlaga);
    ui->temerature->display(mJob.temperature());
    ui->time->setText(h_str +  QString(":") + m_str +QString(":") + s_str);

    if(enable)
    {
        if((mJob.get_hour() == h_a) && (mJob.get_min() == m_a) && (mJob.get_sec() == s_a) && mJob.get_day() == d_a)
            mJob.pump();
        //if((mJob.get_day() == d_a) && (mJob.get_month() == mo_a) && (mJob.get_year() == y_a))
            //mJob.pump();
     }
}

void Dialog::on_threshold_of_irrigation_textChanged(const QString &arg1)
{
     prag = arg1.toInt();
}

void Dialog::on_automatic_irrigation_enable_clicked(bool checked)
{
    if(checked)
        automatic = true;
    else
        automatic = false;
}

void Dialog::on_set_time_textEdited(const QString &arg1)
{
    QTime tm;
    tm = QTime::fromString(arg1, "hh:mm:ss");
    h_a = tm.hour();
    m_a = tm.minute();
    s_a = tm.second();
}

void Dialog::on_calendarWidget_clicked(const QDate &date)
{
    d_a = date.daysInMonth();
    mo_a = date.month();
    y_a = date.year()-2000;
}

void Dialog::on_enable_date_time_clicked(bool checked)
{
    if(checked)
        enable = true;
    else
        enable = false;
}
