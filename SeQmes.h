#ifndef SEQMES_H
#define SEQMES_H

#include <QtWidgets>
#include <QHostInfo>
#include <ctime>

#include "Client.h"
#include "server.h"
#include "userItem.h"

namespace Ui
{
    class SeQmes;
}

class SeQmes : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit SeQmes(QWidget *parent = 0);
    ~SeQmes();

    QSettings *settings;

public slots:
    void startServer();
    void readConfig();
    void readStyle();
    void readContacts();
    void displayConversation(QListWidgetItem*);
    void currentItemChanged(QListWidgetItem*current, QListWidgetItem*previous);
    void menuUsersList(const QPoint &pos);
    void sendMessage();
    void btnConnectPressed(); // from button Connection
    void receiveStatusClient(QString status, QString infos);
    void receiveStatusServer(QString status, QString infos);

private:
    Ui::SeQmes *ui;
    void alert();
	bool checkAddress(QString IP_Port);
    void connectToUser(QString ip, QString port, bool userQuery = false, vCard vcard = vCard());

	QString currentServerName;
	QString currentServerIp;
	QString currentServerPort;

    QString configServerIp;
    quint16 configServerPort;

    QString styleFile;

    QString IP;
    //Client *client;
    Server *server;

    QVBoxLayout *mainLayout;
    QWidget *mainContainer;
    QGridLayout *leftLayout;
    QWidget *leftContainer;
    QSplitter *messageSplitter;
    QGridLayout *rightLayout;
    QWidget *rightContainer;
    QSplitter* splitterContainer;
    QHBoxLayout *bottomLayout;
    QWidget *bottomContainer;

    QLabel *statusServer;
    QLabel *statusGlobal;
    QTextEdit *messagesBox;
    QTextEdit  *input_message;
    QString currentMessage;
    QLineEdit *input_address;
    QPushButton *btnConnect;

    QLabel *clientsLabel;
	QStringList *clientsStringList;
    QListWidget *clientsListWidget;

    QLabel *lbl_contacts;
    QList<Client*> *list_contacts;
    QListWidget *listWidget_contacts;

    QMenu *menuFile;
    QMenu *menuUsers;

    QAction *actionReloadConfig;
    QAction *actionExit;
    QAction *action_clientDisconnect;
    QAction *actionStartServer;
    QAction *actionStopServer;
    QAction *actionDisconnect;

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // SEQMES_H
