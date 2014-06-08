#include "Client.h"

Client::Client(vCard m_vcard)
{
    client = new QTcpSocket;
    vcard = new vCard(m_vcard);
    heartbeatTimer = new QTimer;
    connect(heartbeatTimer, SIGNAL(timeout()), this, SLOT(serverTimeout()));
}

Client::~Client()
{
}

void Client::connectToServer(QString serverIp, QString serverPort, vCard vcard)
{
    if(vcard.count() != 0)
    {
        clientvCard = vcard;
    }
	client->abort();
	connect(client, SIGNAL(connected()), this, SLOT(connected()));
	client->connectToHost(serverIp, serverPort.toInt());
    currentServerIp = serverIp;
    currentServerPort = serverPort;
    emit sendStatus("pending",currentServerIp + ":" + currentServerPort);
	heartbeatTimer->start(3000);
}

void Client::disconnect()
{
    heartbeatTimer->stop();
    client->abort();
}

void Client::connected()
{
	connectedTo = client->peerAddress().toString() + ":" + QString::number(client->peerPort());
	connectedToIp = client->peerAddress().toString();
	connectedToPort = QString::number(client->peerPort());

    heartbeatTimer->start(10000);
    connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(client, SIGNAL(readyRead()), this, SLOT(readyRead()));
    emit sendStatus("connected");
}

void Client::disconnected()
{
    heartbeatTimer->stop();
    emit sendStatus("disconnected");
}

void Client::sendData(QString data, QString type)
{
    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);

    out << (quint16) 0;
    out << type + ":" + data;
    out.device()->seek(0);
    out << (quint16) (paquet.size() - sizeof(quint16));

    client->write(paquet);
}

void Client::readyRead()
{
    heartbeatTimer->start(10000);
}

void Client::serverTimeout()
{
    heartbeatTimer->stop();
    emit sendStatus("disconnected", currentServerIp + ":" + currentServerPort);
    client->abort();
}
