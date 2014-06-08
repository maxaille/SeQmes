#ifndef USERSITEM_H
#define USERSITEM_H

#include <QtWidgets>
#include <lib/vcard/vcard.h>

class UserItem : public QWidget
{
    //Q_OBJECT
public:
    explicit UserItem(vCard m_vcard, QWidget *parent = 0);
    explicit UserItem(QString m_dispName, QString m_infos, QString m_adress, vCard m_vcard, bool m_hideAdress, QWidget *parent = 0);

    QLabel *username;
    QLabel *persoMsg;
    QLabel *adress;

    QLabel  *avatar;
    QPixmap *background;

    QHBoxLayout *mainLayout;
    QVBoxLayout *rightLayout;
    QWidget *rightContainer;
    
signals:
    
public slots:
    
};

#endif // USERSITEM_H
