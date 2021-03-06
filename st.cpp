﻿#include "st.h"
#include "ui_st.h"
#include "QFile"
#include "QDebug"
#include "QMessageBox"
#include "QFileDialog"
#include "QJsonObject"
#include "QJsonDocument"
#include "QJsonArray"
#include "QJsonParseError"

ST::ST(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ST)
{
    ui->setupUi(this);
    QStringList title_col;
    ui->tableWidget->setColumnCount(7);
    title_col << "Task #" << "WCET(ms)" << "Deadline(ms)" << "Period/Minimum Separation(ms)" << "Jitter(ms)" << "Offset(ms)" << "Periodic/sporadic";
    ui->tableWidget->setHorizontalHeaderLabels(title_col);
    ui->tableWidget->setColumnWidth(3, 200);
}

ST::~ST()
{
    delete ui;
}

void ST::on_taskEnterButton_clicked()
{
    int res;
    St_Dialog td;
    //get user's inputs
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

    //set items column by column
    ui->tableWidget->setItem(temp, Name, new QTableWidgetItem(name));
    ui->tableWidget->setItem(temp, WCET, new QTableWidgetItem(wcet));
    ui->tableWidget->setItem(temp, Deadline, new QTableWidgetItem(deadline));
    ui->tableWidget->setItem(temp, Period, new QTableWidgetItem(period));
    ui->tableWidget->setItem(temp, Jitter, new QTableWidgetItem(jitter));
    ui->tableWidget->setItem(temp, Offset, new QTableWidgetItem(offset));

    //combox box setup
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

void ST::on_taskRemoveButton_clicked()
{
    ui->tableWidget->removeRow(ui->tableWidget->currentRow());
}

void ST::on_saveButton_clicked()
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

void ST::saveFile()
{
    //get the number of rows in the table
    item_counter = ui->tableWidget->rowCount();
    qDebug() << item_counter;

    QFile file(curSaveFile);
    if(!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Cannot open file";
        return;
    }

    QJsonObject mainObj;

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

    QJsonObject specification;
    specification.insert("partitionWCET", ui->wcetTextEdit->toPlainText());
    specification.insert("partitionPeriod", ui->periodTextEdit->toPlainText());
    mainObj.insert("Specification", specification);

    QJsonDocument jsonDoc;
    jsonDoc.setObject(mainObj);

    file.write(jsonDoc.toJson());
    file.close();
}

void ST::on_loadButton_clicked()
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

void ST::loadFile()
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

        //adding to the tablewidge
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

        //combo box setup
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
    if(rootObj.contains("Specification"))
    {
        ui->wcetTextEdit->setPlainText(rootObj.value("Specification").toObject()["partitionWCET"].toString());
        ui->periodTextEdit->setPlainText(rootObj.value("Specification").toObject()["partitionPeriod"].toString());
    }
}


void ST::on_getSTButton_clicked()
{
    item_counter = ui->tableWidget->rowCount();
    double af = 0;
    //calculate the smallest availability factor required to make it schedulable
    for(int i = 0; i < item_counter; i++)
    {
        double wcet = ui->tableWidget->item(i, WCET)->text().toDouble();
        double period = ui->tableWidget->item(i, Period)->text().toDouble();
        double deadline = ui->tableWidget -> item(i, Deadline)->text().toDouble();
        double temp = qCeil(wcet) / static_cast<double>(qFloor(qMin(period, deadline)));
        af += temp;
    }
    double inputAf = ui->wcetTextEdit->toPlainText().toDouble()/ui->periodTextEdit->toPlainText().toDouble();
    //set schedulable or not accordingly
    if(af <= inputAf)
    {
        ui->schedulableComboBox->setCurrentIndex(0);
    }
    else
    {
        ui->schedulableComboBox->setCurrentIndex(1);
    }
}
