#include "mplotui.h"
#include "ui_mplotui.h"

MPlotUi::MPlotUi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MPlotUi)
{
    ui->setupUi(this);
}

MPlotUi::~MPlotUi()
{
    delete ui;
}
