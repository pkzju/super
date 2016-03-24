#ifndef SERIALPORTUI_H
#define SERIALPORTUI_H

#include <QDialog>

namespace Ui {
class SerialPortUi;
}

class SerialPortUi : public QDialog
{
    Q_OBJECT

public:
    explicit SerialPortUi(QWidget *parent = 0);
    ~SerialPortUi();

private slots:
    void on_pushButton_Open_clicked();

    void on_pushButton_Close_clicked();

    void on_pushButton_Clear_clicked();

    void on_pushButton_Send_clicked();

    void radioButton_CRC_clicked();

private:
    Ui::SerialPortUi *ui;
};

#endif // SERIALPORTUI_H
