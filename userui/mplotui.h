#ifndef MPLOTUI_H
#define MPLOTUI_H

#include <QWidget>

#include "fanmotor/fanmotor.h"

class QCustomPlot;

namespace Ui {
class MPlotUi;
}

class MPlotUi : public QWidget
{
    Q_OBJECT

public:
    explicit MPlotUi(QWidget *parent = 0);
    ~MPlotUi();

public slots:
    void realtimeDataSlot(FanMotorController motorctr);
private:
    Ui::MPlotUi *ui;
    void setupRealtimeDataDemo(QCustomPlot *customPlot);
};

#endif // MPLOTUI_H
