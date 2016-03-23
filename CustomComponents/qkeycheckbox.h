#ifndef QKEYCHECKBOX_H
#define QKEYCHECKBOX_H

#include <QObject>
#include <QCheckBox>

class QKeyCheckBox : public QCheckBox
{
    Q_OBJECT
public:
     explicit QKeyCheckBox(QWidget *parent = 0) : QCheckBox(parent)
    {
        connect(this, SIGNAL(clicked()), this, SLOT(on_clicked()));
    }
    ~QKeyCheckBox()
    {

    }
private:
signals:
    void CheckBoxClicked(QCheckBox *checkBox, QString text);
public slots:
    void on_clicked()
    {
        CheckBoxClicked(this,this->objectName());
    }
};

#endif // QKEYCHECKBOX_H
