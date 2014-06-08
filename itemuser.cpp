#include "itemuser.h"
#include "ui_itemuser.h"

ItemUser::ItemUser(vCard m_vcard, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::itemUser)
{
    ui->setupUi(this);
    // Parsing the vcard
    vcard = m_vcard;
    if(vcard.contains("NICKNAME") &&
       vcard.contains("IP") &&
       vcard.contains("PORT"))
    {
        ui->lbl_username->setText(vcard.property("NICKNAME").value());
        if(vcard.contains("NOTE"))
            ui->lbl_note->setText(vcard.property("NOTE").value());
        ui->lbl_adress->setText(tr("%0:%1").arg(vcard.property("IP").value(),
                                                vcard.property("PORT").value()));
    }
    //////////
    // Picture
    QPixmap avatar;
    // background is a 40*40 transparent image
    QPixmap background(35,35);
    background.load(":/images/background.gif");
    QPainter painter(&background);
    if(vcard.contains("PHOTO"))
    {
        QString imageString = vcard.property("PHOTO").value();
        if(imageString.length() < 2^(15))
        {
            if(QByteArray::fromBase64(imageString.toUtf8()).toHex().startsWith("89504e470d0a1a0a"))
            {
                avatar.loadFromData(QByteArray::fromBase64(imageString.toUtf8()), "PNG");
            }
            else if(QByteArray::fromBase64(imageString.toUtf8()).toHex().startsWith("ffd8"))
            {
                avatar.loadFromData(QByteArray::fromBase64(imageString.toUtf8()), "JPG");
            }
            int width = avatar.width();
            int height = avatar.height();
            QSize size(0,0);
            if(width<height)
            {
                size.setWidth((int)(width/(float)(height/35)));
                size.setHeight((int)(height/(float)(height/35)));
            }
            else
            {
                size.setWidth((int)(width/(float)(width/35)));
                size.setHeight((int)(height/(float)(width/35)));
            }
            avatar = avatar.scaled(size,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        }
    }
    if(avatar.isNull())
    {
        // load default avatar
        avatar.load(":/images/defaultUser.png");
    }
    int VCenter = 0;
    int HCenter = 0;
    if(avatar.height() < 35)
    {
        VCenter = (int)((35 - avatar.height())/2);
    }
    if(avatar.width() < 35)
    {
        HCenter = (int)((35 - avatar.width())/2);
    }
    painter.drawPixmap(HCenter,VCenter,avatar.width(),avatar.height(),avatar);

    QPen myPen(Qt::gray);
    painter.setPen(myPen);
    painter.drawLine(0,0, 0,34);
    painter.drawLine(0,0, 34,0);
    painter.drawLine(34,0, 34,34);
    painter.drawLine(0,34, 34,34);

    ui->lbl_image->setPixmap(background);
}

ItemUser::~ItemUser()
{
    delete ui;
}


///////////
/// Accessor
QString ItemUser::userName()
{
    return ui->lbl_username->text();
}

QString ItemUser::address()
{
    return ui->lbl_adress->text();
}
