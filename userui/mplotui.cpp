#include "mplotui.h"
#include "ui_mplotui.h"

#include "qcustomplot/qcustomplot.h"

MPlotUi::MPlotUi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MPlotUi)
{
    ui->setupUi(this);
    setupRealtimeDataDemo(ui->customPlot1);
    setupRealtimeDataDemo(ui->customPlot2);
    setupRealtimeDataDemo(ui->customPlot3);
}

MPlotUi::~MPlotUi()
{
    delete ui;
}

void MPlotUi::setupRealtimeDataDemo(QCustomPlot *customPlot)
{
    // include this section to fully disable antialiasing for higher performance:
    customPlot->setNotAntialiasedElements(QCP::aeAll);
    QFont font;
    font.setStyleStrategy(QFont::NoAntialias);
    customPlot->xAxis->setTickLabelFont(font);
    customPlot->yAxis->setTickLabelFont(font);
    customPlot->legend->setFont(font);


    customPlot->addGraph(); // blue line
    customPlot->graph(0)->setPen(QPen(Qt::blue));
    customPlot->graph(0)->setName("graph1^*^");
    customPlot->addGraph(); // red line
    customPlot->graph(1)->setPen(QPen(Qt::red));
    customPlot->addGraph(); // green line
    customPlot->graph(2)->setPen(QPen(Qt::green));


    customPlot->addGraph(); // blue dot
    customPlot->graph(3)->setPen(QPen(Qt::blue));
    customPlot->graph(3)->setLineStyle(QCPGraph::lsNone);
    customPlot->graph(3)->setScatterStyle(QCPScatterStyle::ssDisc);
    customPlot->addGraph(); // red dot
    customPlot->graph(4)->setPen(QPen(Qt::red));
    customPlot->graph(4)->setLineStyle(QCPGraph::lsNone);
    customPlot->graph(4)->setScatterStyle(QCPScatterStyle::ssDisc);
    customPlot->addGraph(); // green dot
    customPlot->graph(5)->setPen(QPen(Qt::green));
    customPlot->graph(5)->setLineStyle(QCPGraph::lsNone);
    customPlot->graph(5)->setScatterStyle(QCPScatterStyle::ssDisc);

    customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    customPlot->xAxis->setDateTimeFormat("mm:ss");
    customPlot->xAxis->setAutoTickStep(false);
    customPlot->xAxis->setTickStep(1);
    //  customPlot->axisRect()->setupFullAxesBox();

    // setup legend:
    customPlot->legend->setVisible(true);
    customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignLeft);
    customPlot->legend->setBrush(QColor(80, 80, 80, 200));
    QPen legendPen;
    legendPen.setColor(QColor(130, 130, 130, 200));
    customPlot->legend->setBorderPen(legendPen);
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    customPlot->legend->setTextColor(Qt::white);
    customPlot->legend->removeItem(customPlot->legend->itemCount()-1);
    customPlot->legend->removeItem(customPlot->legend->itemCount()-1);
    customPlot->legend->removeItem(customPlot->legend->itemCount()-1);

    // set some pens, brushes and backgrounds:
    customPlot->xAxis->setBasePen(QPen(Qt::white, 1));
    customPlot->yAxis->setBasePen(QPen(Qt::white, 1));
    customPlot->xAxis->setTickPen(QPen(Qt::white, 1));
    customPlot->yAxis->setTickPen(QPen(Qt::white, 1));
    customPlot->xAxis->setSubTickPen(QPen(Qt::white, 1));
    customPlot->yAxis->setSubTickPen(QPen(Qt::white, 1));
    customPlot->xAxis->setTickLabelColor(Qt::white);
    customPlot->yAxis->setTickLabelColor(Qt::white);
    customPlot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    customPlot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    customPlot->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    customPlot->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    customPlot->xAxis->grid()->setSubGridVisible(true);
    customPlot->yAxis->grid()->setSubGridVisible(true);
    customPlot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
    customPlot->yAxis->grid()->setZeroLinePen(Qt::NoPen);
    customPlot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    customPlot->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(80, 80, 80));
    plotGradient.setColorAt(1, QColor(50, 50, 50));
    customPlot->setBackground(plotGradient);
    QLinearGradient axisRectGradient;
    axisRectGradient.setStart(0, 0);
    axisRectGradient.setFinalStop(0, 350);
    axisRectGradient.setColorAt(0, QColor(80, 80, 80));
    axisRectGradient.setColorAt(1, QColor(30, 30, 30));
    customPlot->axisRect()->setBackground(axisRectGradient);



    // make left and bottom axes transfer their ranges to right and top axes:
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

    //  // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
    //  connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    //  dataTimer.start(0); // Interval 0 means to refresh as fast as possible
}

void MPlotUi::realtimeDataSlot(FanMotorController motorctr)
{

    double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    uint value0 = motorctr.m_speedRef;
    uint value1 = motorctr.m_speedFbk;
    uint value2 = 0;

    // add data to lines:
    ui->customPlot1->graph(0)->addData(key, value0);
    ui->customPlot1->graph(1)->addData(key, value1);
    ui->customPlot1->graph(2)->addData(key, value2);
    // set data of dots:
    ui->customPlot1->graph(3)->clearData();
    ui->customPlot1->graph(3)->addData(key, value0);
    ui->customPlot1->graph(4)->clearData();
    ui->customPlot1->graph(4)->addData(key, value1);
    ui->customPlot1->graph(5)->clearData();
    ui->customPlot1->graph(5)->addData(key, value2);
    // remove data of lines that's outside visible range:
    if(ui->customPlot1->graph(0)->data()->count()>300)
        ui->customPlot1->graph(0)->removeData(ui->customPlot1->graph(0)->data()->firstKey());
    if(ui->customPlot1->graph(1)->data()->count()>300)
        ui->customPlot1->graph(1)->removeData(ui->customPlot1->graph(1)->data()->firstKey());
    if(ui->customPlot1->graph(2)->data()->count()>300)
        ui->customPlot1->graph(2)->removeData(ui->customPlot1->graph(2)->data()->firstKey());
    // rescale value (vertical) axis to fit the current data:
    ui->customPlot1->graph(0)->rescaleValueAxis(false);
    ui->customPlot1->graph(1)->rescaleValueAxis(true);
    ui->customPlot1->graph(2)->rescaleValueAxis(true);

    ui->customPlot1->xAxis->setRange(ui->customPlot1->graph(0)->data()->firstKey(), ui->customPlot1->graph(0)->data()->lastKey()+0.1);
    ui->customPlot1->replot();

}
