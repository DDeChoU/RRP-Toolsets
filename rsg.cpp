﻿#include "rsg.h"
#include "ui_rsg.h"
#include "QComboBox"
#include "QFile"
#include "QDebug"
#include "QMessageBox"
#include "QFileDialog"
#include "QJsonObject"
#include "QJsonDocument"
#include "QJsonArray"
#include "QJsonParseError"

RSG::RSG(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RSG)
{
    ui->setupUi(this);
    QStringList title_col;
    ui->tableWidget->setColumnCount(7);
    title_col << "Task Id" << "WCET(ms)" << "Deadline(ms)" << "Period/Minimum Separation(ms)" << "Jitter(ms)" << "Offset(ms)" << "Periodic/sporadic";
    ui->tableWidget->setHorizontalHeaderLabels(title_col);
    ui->tableWidget->setColumnWidth(3, 200);
}

RSG::~RSG()
{
    delete ui;
}

void RSG::on_taskEnterButton_clicked()
{
    int res;
    Task_Dialog td;
    //takes user's input
    res = td.exec();
    if(res == QDialog::Rejected)
    {
        return;
    }


    name = td.Name();
    wcet = td.WCET();
    deadline = td.Deadline();
    period = td.Period();
    jitter = td.Jitter();
    offset = td.Offset();
    isperiodic = td.IsPeriodic();

    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    int temp = ui->tableWidget->rowCount() - 1;

    QComboBox *dropdown = new QComboBox();
    dropdown->addItem("Periodic");
    dropdown->addItem("Sporadic");

    //write to the table column after column
    ui->tableWidget->setItem(temp, Name, new QTableWidgetItem(name));
    ui->tableWidget->setItem(temp, WCET, new QTableWidgetItem(wcet));
    ui->tableWidget->setItem(temp, Deadline, new QTableWidgetItem(deadline));
    ui->tableWidget->setItem(temp, Period, new QTableWidgetItem(period));
    ui->tableWidget->setItem(temp, Jitter, new QTableWidgetItem(jitter));
    ui->tableWidget->setItem(temp, Offset, new QTableWidgetItem(offset));

    //combobox setting
    ui->tableWidget->setCellWidget(temp, Periodic_or_sporadic, dropdown);
    if(isperiodic == "true")
    {
        dropdown->setCurrentIndex(0);
    }
    else
    {
        dropdown->setCurrentIndex(1);
    }

}

void RSG::on_taskRemoveButton_clicked()
{
    ui->tableWidget->removeRow(ui->tableWidget->currentRow());
}

void RSG::saveFile()
{
    //get number of rows in the table
    item_counter = ui->tableWidget->rowCount();
    qDebug() << item_counter;

    QFile file(curSaveFile);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(
                    this,
                    "TextEditor",
                    tr("Cannot write file %1./nError: %2")
                    .arg(curSaveFile)
                    .arg(file.errorString()));

    }

    QJsonObject mainObj;
    //writes data in json format one after another
    for(int i = 0; i < item_counter; i++)
    {
        QJsonObject jsonObject;
        jsonObject.insert("ID",ui->tableWidget->item(i,Name)->text());
        jsonObject.insert("WCET",ui->tableWidget->item(i,WCET)->text());
        jsonObject.insert("Deadline",ui->tableWidget->item(i,Deadline)->text());
        jsonObject.insert("Period",ui->tableWidget->item(i,Period)->text());
        jsonObject.insert("Jitter",ui->tableWidget->item(i,Jitter)->text());
        jsonObject.insert("Offset",ui->tableWidget->item(i,Offset)->text());

        QComboBox* temp_drop = static_cast<QComboBox*>(ui->tableWidget->cellWidget(i, Periodic_or_sporadic));
        isperiodic = temp_drop->currentText();
        jsonObject.insert("Periodic/Sporadic", isperiodic);

        mainObj.insert(QString::number(i + 1),jsonObject);
    }

    QJsonObject jsonObject;
    jsonObject.insert("item_counter", item_counter);
    mainObj.insert("number",jsonObject);

    QJsonDocument jsonDoc;
    jsonDoc.setObject(mainObj);

    file.write(jsonDoc.toJson());
    file.close();
}

void RSG::on_saveButton_clicked()
{
    QString filename1 = QFileDialog::getSaveFileName(
                this,
                "TextEditor - save as",
                "",
                "Json File (*.json);;All Files (*.*)");
    curSaveFile = filename1;
    if(!filename1.isEmpty())
    {
        saveFile();
    }
    else
    {
        return;
    }
}

void RSG::on_loadButton_clicked()
{
    QString filename2 = QFileDialog::getOpenFileName(
                this,
                "TextEditor - Open",
                "",
                "Json File (*.json);;All Files (*.*)");
    curOpenFile = filename2;
    if(!filename2.isEmpty())
    {
        loadFile();
    }
    else
    {
        return;
    }
}

void RSG::loadFile()
{
    QFile file(curOpenFile);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(
                    this,
                    "TextEditor",
                    tr("Cannot open file %1./nError: %2")
                    .arg(curOpenFile)
                    .arg(file.errorString()));
    }

    QByteArray allData = file.readAll();
    file.close();

    QJsonParseError json_error;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &json_error));

    if(json_error.error != QJsonParseError::NoError)
    {
        qDebug() << "error";
        return;
    }

    int item_size = 0;

    QJsonObject rootObj = jsonDoc.object();
    //getting item size
    if(rootObj.contains("number"))
    {
        item_size = rootObj.value("number").toObject()["item_counter"].toInt();
    }

    ui->tableWidget->setRowCount(0);
    //reading items
    for(int i = 0; i <item_size; i++)
    {
        name = rootObj.value(QString::number(i + 1)).toObject()["ID"].toString();
        wcet = rootObj.value(QString::number(i + 1)).toObject()["WCET"].toString();
        deadline = rootObj.value(QString::number(i + 1)).toObject()["Deadline"].toString();
        period = rootObj.value(QString::number(i + 1)).toObject()["Period"].toString();
        jitter = rootObj.value(QString::number(i + 1)).toObject()["Jitter"].toString();
        offset = rootObj.value(QString::number(i + 1)).toObject()["Offset"].toString();
        //combobox reading
        isperiodic = rootObj.value(QString::number(i + 1)).toObject()["Periodic/Sporadic"].toString();

        //adding to the tablewidget
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        int temp = ui->tableWidget->rowCount() - 1;

        QComboBox *dropdown = new QComboBox();
        dropdown->addItem("Periodic");
        dropdown->addItem("Sporadic");

        //set column by column
        ui->tableWidget->setItem(temp, Name, new QTableWidgetItem(name));
        ui->tableWidget->setItem(temp, WCET, new QTableWidgetItem(wcet));
        ui->tableWidget->setItem(temp, Deadline, new QTableWidgetItem(deadline));
        ui->tableWidget->setItem(temp, Period, new QTableWidgetItem(period));
        ui->tableWidget->setItem(temp, Jitter, new QTableWidgetItem(jitter));
        ui->tableWidget->setItem(temp, Offset, new QTableWidgetItem(offset));

        //set up the combobox
        ui->tableWidget->setCellWidget(temp, Periodic_or_sporadic, dropdown);
        if(isperiodic == "Periodic")
        {
            dropdown->setCurrentIndex(0);
        }
        else
        {
            dropdown->setCurrentIndex(1);
        }
    }
}

void RSG::fraction_add(long& a_deno, long& a_no, long b_deno, long b_no)
{
    long temp_deno = a_deno*b_no + a_no*b_deno;
    long temp_no = a_no * b_no;
    a_deno = temp_deno;
    a_no = temp_no;
    reduce(a_deno, a_no);
}

void RSG::reduce(long& a_deno, long& a_no)
{
    long temp_deno = a_deno;
    long temp_no = a_no;
    //temp_no should always be no smaller than temp_deno
    //get the gcd, store it in temp_no
    while(temp_deno!=0)
    {
        long temp = temp_no%temp_deno;
        temp_no = temp_deno;
        temp_deno = temp;
    }

    a_deno /= temp_no;
    a_no /= temp_no;

}

void RSG::on_getconfigButton_clicked()
{
    //get the number of rows
    item_counter = ui->tableWidget->rowCount();
    long af_upper = 0, af_lower = 1;
    //calculate the sum of ceil(wcet)/floor(min(period, deadline))
    for(int i = 0; i < item_counter; i++)
    {
        double wcet = ui->tableWidget->item(i, WCET)->text().toDouble();
        double period = ui->tableWidget->item(i, Period)->text().toDouble();
        double deadline = ui->tableWidget -> item(i, Deadline)->text().toDouble();
        long temp_upper = qCeil(wcet);
        long temp_lower = qFloor(qMin(period, deadline));
        fraction_add(af_upper, af_lower, temp_upper, temp_lower);
    }
    //set the availability factor in the UI
    ui->afUpperEdit->setText(QString::number(af_upper));
    ui->afLowerEdit->setText(QString::number(af_lower));
}
