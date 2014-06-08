#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QtNetwork>
#include <lib/vcard/vcard.h>

#include <QLineEdit>

class Client : public QObject
{
    Q_OBJECT

public:
    Client(vCard m_vcard);
    ~Client();

    void connectToServer(QString serverIp, QString serverPort, vCard vcard = vCard()); // Connect to the server
	void sendData(QString data, QString type = "msg"); // Send message to the connected server

    vCard clientvCard;
    vCard *vcard;

    QString currentServerIp;
    QString currentServerPort;

	QString connectedTo; // ip:port where the client is connected
	QString connectedToIp; // ip where the client is connected
	QString connectedToPort; // port where the client is connected

public slots:
    void disconnect();
    void connected();
    void disconnected();
    void serverTimeout();
    void readyRead();

signals:
    void sendStatus(QString status, QString infos = "");

private:
    QTimer *heartbeatTimer;
    QTcpSocket *client;
};

#endif // CLIENT_H
