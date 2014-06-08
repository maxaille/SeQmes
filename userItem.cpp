#include "userItem.h"

UserItem::UserItem(vCard m_vcard, QWidget *parent)
{

}

UserItem::UserItem(QString m_dispName,QString m_infos,QString m_adress,vCard m_vcard, bool m_hideAdress = false, QWidget *parent):
    QWidget(parent)
{
    username = new QLabel(m_dispName);
    username->setObjectName("lblUserame");
    username->setFont(QFont("Arial", 10));
    username->setMargin(0);
    //username->setContentsMargins(0, 0, 0, 0);

    persoMsg = new QLabel(m_infos);
    persoMsg->setObjectName("lblPersoMsg");
    persoMsg->setFont(QFont("Arial", 8));
    persoMsg->setContentsMargins(0, 0, 0, 0);
    persoMsg->setStyleSheet("color:grey;");

    adress = new QLabel(m_adress);
    adress->setObjectName("lblAdress");
    adress->setFont(QFont("Arial", 8));
    adress->setStyleSheet("color:#6666FF;");
    if(m_hideAdress)
    {
        adress->hide();
        username->setProperty("adressHiden",true);
    }

    rightLayout = new QVBoxLayout;
    rightLayout->addWidget(username);
    rightLayout->addWidget(persoMsg);
    rightLayout->addWidget(adress);
    rightLayout->setAlignment(Qt::AlignTop);
    rightLayout->setSpacing(0);
    rightLayout->setMargin(0);
    rightLayout->setContentsMargins(0,0,0,0);
    rightContainer = new QWidget();
    rightContainer->setLayout(rightLayout);

    QLabel *label = new QLabel;
    // background is a 40*40 transparent image
    QPixmap background(40,40);
    background.load(":/images/background.gif");
    QPainter painter(&background);
    QPixmap avatar;

    if(m_vcard.contains("PHOTO"))
    {
        QString image = m_vcard.property("PHOTO").value();
        if(QByteArray::fromBase64(image.toUtf8()).toHex().startsWith("89504e470d0a1a0a"))
        {
            avatar.loadFromData(QByteArray::fromBase64(image.toUtf8()), "PNG");
        }
        else if(QByteArray::fromBase64(image.toUtf8()).toHex().startsWith("ffd8"))
        {
            avatar.loadFromData(QByteArray::fromBase64(image.toUtf8()), "JPG");
        }
        float width = avatar.width();
        float height = avatar.height();
        QSize size(0,0);
        if(width<height)
        {
            size.setWidth((int)(width/(height/40)));
            size.setHeight((int)(height/(height/40)));
        }
        else
        {
            size.setWidth((int)(width/(width/40)));
            size.setHeight((int)(height/(width/40)));
        }
        avatar = avatar.scaled(size,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    }
    if(avatar.isNull())
    {
        // load default avatar
        avatar.load(":/images/qt.png");
    }
    int VCenter = 0;
    int HCenter = 0;
    if(avatar.height() < 40)
    {
        VCenter = (int)((40 - avatar.height())/2);
    }
    if(avatar.width() < 40)
    {
        HCenter = (int)((40 - avatar.width())/2);
    }
    painter.drawPixmap(HCenter,VCenter,avatar.width(),avatar.height(),avatar);

    QPen myPen(Qt::gray);
    painter.setPen(myPen);
    painter.drawLine(0,0, 0,39);
    painter.drawLine(0,0, 39,0);
    painter.drawLine(39,0, 39,39);
    painter.drawLine(0,39, 39,39);

    label->setPixmap(background);

    mainLayout = new QHBoxLayout;
    mainLayout->addWidget(label);
    mainLayout->addWidget(rightContainer);
    mainLayout->setAlignment(Qt::AlignLeft);
    mainLayout->setSpacing(3);
    mainLayout->setMargin(2);

    setLayout(mainLayout);
}
