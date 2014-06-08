#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QtNetwork>

class Server : public QObject
{
    Q_OBJECT

public:
    Server();
    ~Server();

    void sendData(QTcpSocket *m_socket, QString m_type, QString m_infos);

	bool status; // True if server is started, false else
	QString listeningIp; // Ip where qtcpserver is listening to
	QString listeningPort;// Port where qtcpserver is listening to
    QList<QTcpSocket *> *clientsList; // List of all connected client

    QTcpServer *server;

public slots:
    void start(QString m_ip, quint16 m_port); // Start server, listen on ip:port
    void stop(); // Disconnect all clients and stop listening
    void newConnection(); // Called when new client's connected
    void dataReceived(); // Called when datas is received
    void disconnection(); // Called when client disconnect
    void timerTimeout(); // Send a ping to each clients when the heartbeat timer clock

signals:
    void sendStatus(QString type, QString infos = "", QTcpSocket *m_socket = 0);

private:
    QTimer *heartbeatTimer;
    quint16 msgLenght;
    
};

#endif // SERVEUR_H
