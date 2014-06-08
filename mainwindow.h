#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#include <QMainWindow>
#include <QtWidgets>
#include <QHostInfo>
#include <ctime>

#include "server.h"
#include "user.h"
#include "itemuser.h"
#include "connectto.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSettings *settings;
    void setUi();
    void readStyle();
    void readConfig();
    void selectUser();

    void sendMessage();
    void saveConv(ItemUser *m_user);

    Server *server;
    QList<User *> *list_users;
    User *curUser;

    QLabel *lbl_statusBar;

protected:
    bool eventFilter(QObject *m_obj, QEvent *m_event);

public slots:
    void startServer();
    void stopServer();
    void receiveServerStatus(QString m_type, QString m_infos, QTcpSocket *m_socket = 0);
    void receiveUserStatus(QString m_type, QString m_infos, QTcpSocket *m_socket = 0);
    void onlistW_users_customContextMenuRequested(const QPoint &m_pos);
    void onActionCopyAddress_clicked();
    void onActionConnectTo_clicked();
    void onConnectTo_Closed(int result);
    void onItemSelected(QListWidgetItem* m_current,QListWidgetItem* m_previous);
};

#endif // MAINWINDOW_H
