#include "server.h"

Server::Server()
{
	status = false;
    heartbeatTimer = new QTimer;
    connect(heartbeatTimer, SIGNAL(timeout()),
            this, SLOT(timerTimeout()));
    heartbeatTimer->start(8000);
    clientsList = new QList<QTcpSocket *>();
}

Server::~Server()
{

}

void Server::start(QString m_ip, quint16 m_port)
{
    server = new QTcpServer;
    if (!server->listen(QHostAddress(m_ip), m_port))
    {
        emit sendStatus("error", tr("Unable to listen on port %0").arg(QString::number(m_port)));
    }
    connect(server, SIGNAL(newConnection()),
            this, SLOT(newConnection()));
    emit sendStatus("started" , tr("Server started on port %0").arg(QString::number(server->serverPort())));
    status = true;
    listeningIp = m_ip;
    listeningPort = QString::number(m_port);
    msgLenght = 0;
}

void Server::stop()
{
    // Disconnect and remove all clients
    foreach (QTcpSocket *client, *clientsList)
    {
        client->abort();
    }
    qDeleteAll(*clientsList);
    server->close();
	status = false;
    emit sendStatus("stopped", tr("Server stopped"));
}

void Server::newConnection()
{
    // Get then append client(s) to the client's list
    while (server->hasPendingConnections()) {
        QTcpSocket *client = server->nextPendingConnection();
        connect(client, SIGNAL(readyRead()),
                this, SLOT(dataReceived()));
        connect(client, SIGNAL(disconnected()),
                this, SLOT(disconnection()));
        clientsList->append(client);
        emit sendStatus("newconnection",
                        tr("User connected: %0:%1").arg(client->peerAddress().toString(),
                                                        QString::number(client->peerPort())));
    }
}

void Server::dataReceived()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket == 0)
        return;

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
    in >> data;

    emit sendStatus("data", data, socket);

    msgLenght = 0;
}

void Server::sendData(QTcpSocket *m_socket, QString m_type, QString m_infos)
{
    if(clientsList->contains(m_socket))
    {
        QByteArray packet;
        QDataStream out(&packet, QIODevice::WriteOnly);

        out << (quint16) 0;
        out << m_type + ":" + m_infos;
        out.device()->seek(0);
        out << (quint16) (packet.size() - sizeof(quint16));

        m_socket->write(packet);
    }
}

void Server::disconnection()
{
    QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());

        if (!client)
            return;

        clientsList->removeAll(client);
        client->deleteLater();
        emit sendStatus("disconnection", tr("%0:%1 disconnected").arg(client->peerAddress().toString(),
                                                                      QString::number(client->peerPort())));
}

void Server::timerTimeout()
{
    foreach(QTcpSocket *client, *clientsList) {
        client->write("ping");
    }
}
