/****************************************************************************
**
**
**
****************************************************************************/

#include "mainwindow/mainwindow.h"
#include <QApplication>
#include <QTextCodec>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setOrganizationName("ZJU");
    QApplication::setOrganizationDomain("https:");
    QApplication::setApplicationName("Console");
    QApplication::setWindowIcon(QIcon(":/images/skin/images/ico.ico"));

    MainWindow *main = MainWindow::getInstance();
    main->setAttribute(Qt::WA_DeleteOnClose);
    main->show();
    return app.exec();
}
