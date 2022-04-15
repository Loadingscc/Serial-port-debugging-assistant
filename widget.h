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

    void on_pushButton_2_clicked();

    void on_clear_input_btn_clicked();

    void Serial_readAll();

private:
    Ui::Widget *ui;
    QSerialPort* m_serialPort;
    QTimer *URAT_update;
};
#endif // WIDGET_H
