#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QtConcurrent>
#include <QFuture>
#include <myjob.h>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:
    void on_wather_pump_activate_clicked();
    void on_timer_Update();
    void on_threshold_of_irrigation_textChanged(const QString &arg1);
    void on_automatic_irrigation_enable_clicked(bool checked);
    void on_set_time_textEdited(const QString &arg1);
    void on_calendarWidget_clicked(const QDate &date);
    void on_enable_date_time_clicked(bool checked);

private:
    Ui::Dialog *ui;
    MyJob mJob;
    QTimer *timer;

    /*** irrigation parameters ***/
    int prag;
    bool automatic;
    /*** edit alarm time parameters***/
    int s_a, m_a, h_a;
    int d_a, mo_a, y_a;
    bool enable;
};

#endif // DIALOG_H
