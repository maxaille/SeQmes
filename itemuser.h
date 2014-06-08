#ifndef ITEMUSER_H
#define ITEMUSER_H

#include <QWidget>
#include <QPainter>

#include "lib/vcard/vcard.h"

namespace Ui {
class itemUser;
}

class ItemUser : public QWidget
{
    Q_OBJECT

public:
    explicit ItemUser(vCard m_vcard, QWidget *parent = 0);
    ~ItemUser();

    QString userName();
    QString address();

    vCard vcard;

private:
    Ui::itemUser *ui;
};

#endif // ITEMUSER_H
