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

private:
    Ui::SerialPortUi *ui;
};

#endif // SERIALPORTUI_H
