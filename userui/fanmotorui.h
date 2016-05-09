#ifndef FANMOTORUI_H
#define FANMOTORUI_H

#include <QWidget>


namespace Ui {
class FanMotorUi;
}

class FanMotorUi : public QWidget
{
    Q_OBJECT

public:
    explicit FanMotorUi(QWidget *parent = 0);
    ~FanMotorUi();

private:
    Ui::FanMotorUi *ui;

};

#endif // FANMOTORUI_H
