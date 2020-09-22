﻿#ifndef TASK_DIALOG_H
#define TASK_DIALOG_H

#include <QDialog>
#include <task.h>
#include <QDebug>
#include <QMap>

namespace Ui {
class Task_Dialog;
}

class Task_Dialog : public QDialog
{
    Q_OBJECT

public:

    void addTask();

    QString Name() const;

    float WCET() const;

    float Deadline() const;

    float Period() const;

    float Jitter() const;

    float Offset() const;

    bool IsPeriodic() const;

    QString execution(double, double, double, double, double, bool);

    QString get_info();

    explicit Task_Dialog(QWidget *parent = nullptr);
    ~Task_Dialog();

private slots:
    void on_OkbuttonBox_accepted();

    void on_OkbuttonBox_rejected();

private:
    Ui::Task_Dialog *ui;

    QString text;

    QMap<QString, QString> maps;
};

#endif // TASK_DIALOG_H
