#include "widget.h"
#include "ui_widget.h"

#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <QMessageBox>
#include <QFileDialog>
#include <QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{

    ui->setupUi(this);
    this->setWindowTitle("橙火调试助手");//设置窗口名
    ui->textEdit->setReadOnly(true);
    ui->textEdit_2->setReadOnly(true);

    m_serialPort = new QSerialPort();

    URAT_update = new QTimer(this);
    connect(URAT_update,&QTimer::timeout,this,&Widget::update_urat);
    URAT_update->start();

    T_send = new QTimer(this);
    connect(T_send, &QTimer::timeout, this, &Widget::get_T);
    T_send->start(30);

    auto_send = new QTimer(this);
    connect(auto_send, &QTimer::timeout, this, &Widget::on_pushButton_2_clicked);
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

        ui->comboBox->setEnabled(false);
        ui->comboBox_2->setEnabled(false);
        ui->comboBox_3->setEnabled(false);
        ui->comboBox_4->setEnabled(false);
        ui->comboBox_5->setEnabled(false);

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

        if(ui->comboBox_5->currentText().toStdString() == "无"){
            m_serialPort->setParity(QSerialPort::NoParity); //无校验位
            qDebug() << "无";
        }
        else if (ui->comboBox_5->currentText().toStdString() == "奇"){
            m_serialPort->setParity(QSerialPort::EvenParity); //无校验位
            qDebug() << "奇";
        }
        else{
            m_serialPort->setParity(QSerialPort::MarkParity); //无校验位
            qDebug() << "偶";
        }

        connect(m_serialPort,SIGNAL(readyRead()),this,SLOT(receiveInfo()));
        QObject::connect(m_serialPort,&QSerialPort::readyRead,this,&Widget::Serial_readAll);
    }
    else{
        m_serialPort->close();
        ui->pushButton->setText("打开串口");
        ui->comboBox->setEnabled(true);
        ui->comboBox_2->setEnabled(true);
        ui->comboBox_3->setEnabled(true);
        ui->comboBox_4->setEnabled(true);
        ui->comboBox_5->setEnabled(true);
    }
}

void Widget::on_pushButton_2_clicked()
{
    QByteArray buf; //二进制
    if (update_HEX_ASCII == 0){
        QByteArray str = ui->lineEdit->text().toUtf8();

        if(m_serialPort->isOpen())
        {
            m_serialPort->write(str);
            ui->textEdit_2->append(str);
        }
    }
    else{
        QByteArray str = ui->lineEdit->text().toUtf8();
        buf = str.toHex().toUpper();
        buf = str.toHex().toUpper();
        for (int nIndex = 3; nIndex < str.size();nIndex += 3) {
                buf = buf.insert(nIndex-1," ");
        }
        if(m_serialPort->isOpen())
        {
            m_serialPort->write(buf);
            ui->textEdit_2->append(buf);
        }
    }
}

void Widget::Serial_readAll(){
    QByteArray buf; //二进制
    buf=m_serialPort->readAll();
    if (input_HEX_ASCII == 0){
        if (stop_flag == 0){
            if (note_flag == 1){
                QDateTime current_time = QDateTime::currentDateTime();
                QString currentTime = current_time.toString("yyyy-MM-dd hh:mm:ss");
                ui->textEdit->append(currentTime);
            }
            if(!buf.isEmpty())
            {
                QString str=QString::fromLocal8Bit(buf);
                if(str.endsWith(" THE END "))
                {
                    ui->textEdit->append("I get");
                }
                ui->textEdit->append(str);
            }
            buf.clear();
        }
    }
    else{
        if(stop_flag == 0){
            if (note_flag == 1){
                QDateTime current_time = QDateTime::currentDateTime();
                QString currentTime = current_time.toString("yyyy-MM-dd hh:mm:ss");
                ui->textEdit->append(currentTime);
            }
            if(!buf.isEmpty())
            {
                QString str=QString::fromLocal8Bit(buf);
                str = buf.toHex().toUpper();
                str = buf.toHex().toUpper();
                for (int nIndex = 3; nIndex < str.size();nIndex += 3) {
                        str = str.insert(nIndex-1," ");
                }
                if(str.endsWith(" THE END "))
                {
                    ui->textEdit->append("I get");
                }
                ui->textEdit->append(str);
            }
            buf.clear();
        }
    }
}

void Widget::on_clear_input_btn_clicked()
{
    ui->textEdit->clear();
}

void Widget::on_radioButton_clicked()
{
    input_HEX_ASCII = 0;
}

void Widget::on_radioButton_2_clicked()
{
    input_HEX_ASCII = 1;
}


void Widget::on_radioButton_3_clicked()
{
    update_HEX_ASCII = 0;
}

void Widget::on_radioButton_4_clicked()
{
    update_HEX_ASCII = 1;
}

void Widget::get_T(){
    T_string = ui->lineEdit_2->text();
    t_num = T_string.toInt();
}

void Widget::on_checkBox_4_clicked()
{
    if (auto_sent_flag == 0){
        auto_sent_flag = 1;
        auto_send->start(t_num);
        ui->lineEdit_2->setEnabled(false);
    }
    else{
        auto_sent_flag = 0;
        auto_send->stop();
        ui->lineEdit_2->setEnabled(true);
    }
}

void Widget::on_checkBox_clicked()
{
    if (stop_flag == 0){
        stop_flag = 1;
    }
    else{
        stop_flag = 0;
    }
}

void Widget::on_checkBox_2_clicked()
{
    if(note_flag == 0){
        note_flag = 1;
    }
    else{
        note_flag = 0;
    }
}

void Widget::on_pushButton_4_clicked()
{
    QString textFile = QFileDialog::getSaveFileName(this,tr("Save txt"),
                                                            "",tr("text (*.txt)")); //选择路径
     //将文本框数据取出并按行排列
    QFile file(textFile);//文件命名
    if (!file.open(QFile::WriteOnly | QFile::Text))     //检测文件是否打开
    {
        QMessageBox::information(this, "Error Message", "Please Select a Text File!");
        return;
    }
    QTextStream out(&file);                 //分行写入文件
    out << ui->textEdit->toPlainText();
}

void Widget::on_checkBox_3_clicked()
{
    if(new_line_input == 0){
        new_line_input = 1;
        QMessageBox::information(this, "Error", "未开发此功能!\n具体请咨询2111实验室");
    }
    else{
        new_line_input = 0;
        QMessageBox::information(this, "Error", "未开发此功能!\n具体请咨询2111实验室");
    }
}
