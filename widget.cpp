#include "widget.h"
#include "ui_widget.h"

#include <QDebug>
#include <QTimer>
#include <QMessageBox>
#include <QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->textEdit->setReadOnly(true);
    ui->textEdit_2->setReadOnly(true);

    m_serialPort = new QSerialPort();

    URAT_update = new QTimer(this);
    connect(URAT_update,&QTimer::timeout,this,&Widget::update_urat);
    URAT_update->start();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::update_urat(){
    // 检测可使用串口，并显示在combox中
    ui->comboBox->clear();
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
    {
        QSerialPort serial;
        serial.setPort(info);
        //如果某个串口打开，读取正常，统统关闭
        if(serial.open(QIODevice::ReadWrite))
        {
            ui->comboBox->addItem(info.portName());
            serial.close();
        }
    }
}

void Widget::on_pushButton_clicked()
{
    if(ui->pushButton->text() == tr("打开串口"))
    {
        qDebug() << "打开串口";
        URAT_update->stop();
        m_serialPort->setPortName(ui->comboBox->currentText());//当前选择的串口名字
        if(!m_serialPort->open(QIODevice::ReadWrite))//用ReadWrite 的模式尝试打开串口
        {
            QMessageBox::warning(this, tr("警告"),tr("连接失败"), QMessageBox::Yes);
            URAT_update->start();
            return;
        }
        qDebug()<<"串口打开成功!";
        ui->pushButton->setText("关闭串口");
        if (ui->pushButton->text() == "打开串口"){
            URAT_update->start();
        }
        switch(ui->comboBox_2->currentText().toInt()){
            case 1200: m_serialPort->setBaudRate(QSerialPort::Baud1200,QSerialPort::AllDirections); break;
            case 2400: m_serialPort->setBaudRate(QSerialPort::Baud2400,QSerialPort::AllDirections); break;
            case 4800: m_serialPort->setBaudRate(QSerialPort::Baud4800,QSerialPort::AllDirections); break;
            case 9600: m_serialPort->setBaudRate(QSerialPort::Baud9600,QSerialPort::AllDirections); break;
            case 19200: m_serialPort->setBaudRate(QSerialPort::Baud19200,QSerialPort::AllDirections); break;
            case 38400: m_serialPort->setBaudRate(QSerialPort::Baud38400,QSerialPort::AllDirections); break;
            case 57600: m_serialPort->setBaudRate(QSerialPort::Baud57600,QSerialPort::AllDirections); break;
            case 115200: m_serialPort->setBaudRate(QSerialPort::Baud115200,QSerialPort::AllDirections); break;
        }

        switch(ui->comboBox_3->currentText().toInt()){
            case 8: m_serialPort->setDataBits(QSerialPort::Data8); break;
            case 7: m_serialPort->setDataBits(QSerialPort::Data7); break;
            case 6: m_serialPort->setDataBits(QSerialPort::Data6); break;
        }

        switch(ui->comboBox_4->currentText().toInt()){
            case 1: m_serialPort->setStopBits(QSerialPort::OneStop); break;
            case 2: m_serialPort->setStopBits(QSerialPort::TwoStop); break;
        }

        m_serialPort->setFlowControl(QSerialPort::NoFlowControl);//无流控制
        m_serialPort->setParity(QSerialPort::NoParity); //无校验位


        connect(m_serialPort,SIGNAL(readyRead()),this,SLOT(receiveInfo()));
        QObject::connect(m_serialPort,&QSerialPort::readyRead,this,&Widget::Serial_readAll);
    }
    else{
        m_serialPort->close();
        ui->pushButton->setText("打开串口");
    }
}

void Widget::on_pushButton_2_clicked()
{
        QByteArray str = ui->lineEdit->text().toUtf8();

        if(m_serialPort->isOpen())
        {
            m_serialPort->write(str);
            ui->textEdit_2->append(str);
        }
        else
        {
            QMessageBox::warning(this, tr("警告"),tr("发送失败"), QMessageBox::Yes);
        }
}

void Widget::Serial_readAll(){
    QByteArray buf; //二进制
    buf=m_serialPort->readAll();
    if(!buf.isEmpty())
    {
        QString str=buf;
        if(str.endsWith(" THE END "))
        {
            ui->textEdit->append("I get");
        }
        ui->textEdit->append(str);
    }
    buf.clear();
}

void Widget::on_clear_input_btn_clicked()
{
    ui->textEdit->clear();
}
