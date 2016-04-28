#ifndef MPLOTUI_H
#define MPLOTUI_H

#include <QWidget>

namespace Ui {
class MPlotUi;
}

class MPlotUi : public QWidget
{
    Q_OBJECT

public:
    explicit MPlotUi(QWidget *parent = 0);
    ~MPlotUi();

private:
    Ui::MPlotUi *ui;
};

#endif // MPLOTUI_H
