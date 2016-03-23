#ifndef KEYBUTTON_H
#define KEYBUTTON_H
#include <QPushButton>
#include <QDebug>

class KeyButton : public QPushButton
{
    Q_OBJECT
public:
    explicit KeyButton(QWidget *parent = 0) : QPushButton(parent),
        pauseMsecs(400), intervalMsecs(30)
    {
        connect(this, SIGNAL(pressed()), this, SLOT(on_pressed()));
        connect(this, SIGNAL(released()), this, SLOT(on_released()));
        connect(this, SIGNAL(clicked()), this, SLOT(on_clicked()));
    }
    ~KeyButton(){};

private:
    QTimer *tm;
    long pauseMsecs;
    long intervalMsecs;

signals:
    void keyPressed(const QString &msg);
    void keyReleased(const QString &msg);
    void keyClicked(QString text);

public slots:
    void on_pressed() {
   // QMessageBox::warning(this,"hello","hello");
    }
    void on_released() { }
    void on_clicked() {
        qDebug() << this->text();
        emit keyClicked(this->objectName());   //发送按键ID
    }
};


#endif // KEYBUTTON_H
