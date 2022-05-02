#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pushButton_clicked();
    void update_urat();
    void get_T();

    void on_pushButton_2_clicked();

    void on_clear_input_btn_clicked();

    void Serial_readAll();

    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

    void on_radioButton_3_clicked();

    void on_radioButton_4_clicked();

    void on_checkBox_4_clicked();

    void on_checkBox_clicked();

    void on_checkBox_2_clicked();

    void on_pushButton_4_clicked();

    void on_checkBox_3_clicked();

private:
    Ui::Widget *ui;
    QSerialPort* m_serialPort;
    QString T_string;
    QTimer *URAT_update;
    QTimer *T_send; // 自动发送周期
    QTimer *auto_send;
    int t_num;
    int input_HEX_ASCII = 0; // 0的时候为接受设置中的ASCII选项，否则为HEX
    int update_HEX_ASCII = 0;  // 0的时候为发送设置中的ASCII选项，否则为HEX
    int auto_sent_flag = 0;  // 自动发送
    int stop_flag = 0; // 暂停
    int note_flag = 0; // 日志模式
    int new_line_input = 0; // 发送新行
};
#endif // WIDGET_H
