#ifndef CONNECTTO_H
#define CONNECTTO_H

#include <QDialog>

namespace Ui {
class connectTo;
}

class connectTo : public QDialog
{
    Q_OBJECT

public:
    explicit connectTo(QWidget *parent = 0);
    ~connectTo();

    QString address();

private:
    Ui::connectTo *ui;
};

#endif // CONNECTTO_H
