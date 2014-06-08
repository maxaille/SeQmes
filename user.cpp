#include "user.h"

User::User(vCard m_vcard, QString m_ip, quint16 m_port, QObject *parent):
    QObject(parent)
{
    vcard = m_vcard;
    ip = m_ip;
    port = m_port;
    address = QString("%0:%1").arg(ip, QString::number(port));
    if(vcard.contains("NICKNAME"))
    {
        userName = vcard.property("NICKNAME").value();
    }

    client = new QTcpSocket;
    connect(client, SIGNAL(connected()), this, SLOT(connected()));

    heartbeatTimer = new QTimer;
    connect(heartbeatTimer, SIGNAL(timeout()), this, SLOT(serverTimeout()));

    client->connectToHost(ip, port);
    emit sendStatus("pending", client->peerAddress().toString() + ":" + client->peerPort());
    heartbeatTimer->start(3000);

}

User::~User()
{

}

///////////////////
/// MAIN FUNCTIONS
void User::sendData(QString type, QString data)
{
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);

    out << (quint16) 0;
    out << type + ":" + data;
    out.device()->seek(0);
    out << (quint16) (packet.size() - sizeof(quint16));

    client->write(packet);
}


//////////
/// SLOTS
void User::connected()
{
    heartbeatTimer->start(10000);
    connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(client, SIGNAL(readyRead()), this, SLOT(dataReceived()));
    emit sendStatus("connected", tr("Connected to %0").arg(userName));
}

void User::dataReceived()
{
    heartbeatTimer->start(10000);

    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket == 0)
        return;
    QString data = socket->readAll();
/*
    QDataStream in(socket);

    if (msgLenght == 0)
    {
        if (socket->bytesAvailable() < (int)sizeof(quint16))
             return;

        in >> msgLenght;
    }

    if (socket->bytesAvailable() < msgLenght)
        return;

    QString data;
    in >> data;*/

    emit sendStatus("data", data);

    msgLenght = 0;
}

void User::disconnected()
{
    heartbeatTimer->stop();
    client->abort();
    emit sendStatus("disconnected", tr("%0 has disconnected").arg(userName));
}

void User::disconnect()
{
    heartbeatTimer->stop();
    client->abort();
}

void User::serverTimeout()
{
    heartbeatTimer->stop();
    //emit sendStatus("disconnected", tr("%0 timed out").arg(userName));
    client->abort();
}

QString User::peerAddress()
{
    return client->peerAddress().toString();
}

quint16 User::peerPort()
{
    return client->localPort();
}
