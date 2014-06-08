#ifndef USER_H
#define USER_H

#include <QObject>
#include <QTcpSocket>
#include <QtEndian>
#include <QHostAddress>
#include <QTimer>

#include <lib/vcard/vcard.h>

class User : public QObject
{
    Q_OBJECT
public:
    explicit User(vCard m_vcard, QString m_ip, quint16 m_port, QObject *parent);
    ~User();

    void sendData(QString type, QString data);
    QString peerAddress();
    quint16 peerPort();

    vCard vcard;
    QString ip;
    quint16 port;
    QString address;
    QString userName;

    QTcpSocket *client;
    quint16 msgLenght;
    QTimer *heartbeatTimer;

signals:
    void sendStatus(QString type, QString infos = "", QTcpSocket *m_socket = 0);

public slots:
    void connected();
    void dataReceived();
    void disconnect();
    void disconnected();
    void serverTimeout();
};

#endif // USER_H
