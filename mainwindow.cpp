#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    list_users = new QList<User *>();
    server = new Server();
    connect(server, SIGNAL(sendStatus(QString,QString, QTcpSocket *)),
            this, SLOT(receiveServerStatus(QString, QString, QTcpSocket *)));

    setUi();
    readStyle();
    readConfig();
    this->show();
    selectUser();
    startServer();
}

MainWindow::~MainWindow()
{
    if(ui->listW_users->count() > 0)
    {
        QListWidgetItem *curItem = ui->listW_users->selectedItems().first();
        ItemUser *itemUser = (ItemUser *)ui->listW_users->itemWidget(curItem);
        saveConv(itemUser);
    }
    delete ui;
}

////////////////////////////////////
/// Initialization/config functions
void MainWindow::setUi()
{
    ui->setupUi(this);

    lbl_statusBar = new QLabel("Ready");
    ui->statusbar->addPermanentWidget(lbl_statusBar);
    QList<int> centralHsizes;
    centralHsizes << 150 << 600;
    ui->split_central->setSizes(centralHsizes);
    ui->split_central->setCollapsible(1, 0);
    ui->txtEdit_inputMessage->installEventFilter(this);
}

void MainWindow::readConfig()
{
    settings = new QSettings("seqmes.ini",QSettings::IniFormat);
    if(settings->value("Server/ip") == QVariant::Invalid)
    {
        settings->setValue("Server/ip","0.0.0.0");
    }
    if(settings->value("Server/port") == QVariant::Invalid)
    {
        settings->setValue("Server/port","7331");
    }
    /*ui->statusbar->showMessage(tr("Server listen on %0:%1").arg(
                                   settings->value("Server/ip").toString(),
                                   settings->value("Server/port").toString()));*/
}

void MainWindow::readStyle()
{
    QFile styleFile("seqmes.css");
    if(styleFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        this->setStyleSheet(styleFile.readAll());
    }
}

void MainWindow::selectUser()
{
    /// TODO: Block program if user.vcf not present
    /// TODO: select user, not default user
    if(!QFile::exists("user.vcf"))
    {
        qWarning() << "VCF file not present !";
        return;
    }
    vCard curUserVcard = vCard::fromFile("user.vcf").last();
    curUser = new User(curUserVcard,
                       settings->value("Server/ip").toString(),
                       settings->value("Server/port").toInt(),
                       this);
}

bool MainWindow::eventFilter(QObject *m_obj, QEvent *m_event)
{
    if (m_event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(m_event);
        if(m_obj == ui->txtEdit_inputMessage)
        {
            switch (keyEvent->key()) {
                case Qt::Key_Escape:
                    ui->txtEdit_inputMessage->clearFocus();
                    break;
                case Qt::Key_Return:
                    if(keyEvent->modifiers().testFlag(Qt::ShiftModifier))
                    {
                        return QObject::eventFilter(m_obj, m_event);
                    }
                    sendMessage();
                    return true;
                break;
            }
        }
        return QObject::eventFilter(m_obj, m_event);
    }
    else return false;
}


///////////////////
/// Main functions
void MainWindow::sendMessage()
{
    QString currentMessage = ui->txtEdit_inputMessage->toPlainText();
    if(!currentMessage.isEmpty() &&
       ui->listW_users->selectedItems().count() == 1)
    {
        User *user = list_users->at(ui->listW_users->row(ui->listW_users->selectedItems().first()));
        user->sendData("msg", currentMessage);
        ui->txtEdit_messages->append(tr("[%0]: %2").arg(curUser->userName,
                                                        currentMessage));
        ui->txtEdit_inputMessage->clear();
    }
}

void MainWindow::saveConv(ItemUser *m_user)
{
    if(!QDir("convs").exists())
    {
        QDir().mkdir("convs");
    }
    QFile convFile("convs/" + m_user->userName()+".cnv");
    if (!convFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream stream( &convFile );
    stream.setCodec("UTF-8");
    stream << ui->txtEdit_messages->toHtml();
    convFile.close();
}

//////////
/// Slots
void MainWindow::startServer()
{
    if(!server->status)
    {
        server->start(settings->value("Server/ip").toString(),
                  settings->value("Server/port").toInt());
    }
}

void MainWindow::stopServer()
{
    if(server->status)
    {
        server->stop();
    }
}

void MainWindow::receiveServerStatus(QString m_type, QString m_infos, QTcpSocket *m_socket)
{
    qWarning() << m_type << m_infos;
    if(!(m_type == "started" || m_type == "stopped"))
    {
        QList<QString> listInfos = m_infos.split(":");
        if(m_type == "data")
        {
            if(listInfos[0] == "msg")
            {
                User *user = 0;
                QString ip = m_socket->peerAddress().toString();
                quint16 port = m_socket->peerPort();
                for(int i = 0; i < list_users->count(); i++)
                {
                    if(list_users->at(i)->peerAddress() == ip &&
                       list_users->at(i)->peerPort() == port)
                    {
                        user = list_users->at(i);
                    }
                }
                if(user == 0)
                    return;
                ui->txtEdit_messages->append(tr("[%0]: %1").arg(user->userName,
                                                                m_infos.right(4)));
            }
            else if(listInfos[0] == "query")
            {
                //vCard vcard;
                //QString ip = listInfos[0];
                //quint16 port = server->listeningPort.toInt();
                //User *user = new User(vcard, ip, port, this);
                if(listInfos[1] == "vcf")
                {
                    QString curVcf = curUser->vcard.toByteArray();
                    //m_socket->write("vcf:abcd");
                    server->sendData(m_socket, "vcf", "");
                }
            }

        }
    }
    lbl_statusBar->setText(m_infos);
}

void MainWindow::receiveUserStatus(QString m_type, QString m_infos, QTcpSocket *m_socket)
{
    qWarning() << m_type << m_infos;
    User *user = (User *)sender();
    ItemUser *userItem = new ItemUser(curUser->vcard);
    if(m_type == "connected")
    {
        user->sendData("query", "vcf");
    }
    else if(m_type == "data")
    {
        user->vcard = vCard::fromByteArray(m_infos.toUtf8()).first();
        list_users->append(user);
        userItem->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        QListWidgetItem *item = new QListWidgetItem();
        item->setSizeHint(userItem->size());
        ui->listW_users->addItem(item);
        ui->listW_users->setItemWidget(item, userItem);
        ui->listW_users->setCurrentItem(item);
    }
    else if(m_type == "disconnected")
    {
        quint16 row = list_users->indexOf(user);
        saveConv(userItem);
        if(row > 0)
        {
            ui->listW_users->setCurrentRow(row-1);
        }
        else
        {
            ui->listW_users->clearSelection();
        }
        ui->listW_users->takeItem(row);
        list_users->removeAt(row);
    }
    statusBar()->showMessage(m_infos, 2000);
}

void MainWindow::onlistW_users_customContextMenuRequested(const QPoint &m_pos)
{
    QListWidgetItem *currentItem = ui->listW_users->itemAt(m_pos);
    QMenu *contextMenu = new QMenu(this);
    if(currentItem)
    {
        int row = ui->listW_users->row(currentItem);
        contextMenu->addAction(tr("Copy address"), this, SLOT(onActionCopyAddress_clicked()))->setProperty("address", list_users->at(row)->address);
        contextMenu->addAction(tr("Disconnect"), list_users->at(row), SLOT(disconnect()));
    }
    else
    {
        contextMenu->addAction(tr("Connect..."), this, SLOT(onActionConnectTo_clicked()));
    }
    contextMenu->exec(ui->listW_users->mapToGlobal(m_pos));
    contextMenu->deleteLater();
}

void MainWindow::onActionCopyAddress_clicked()
{
    QString address = sender()->property("address").toString();
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(address);
}

void MainWindow::onActionConnectTo_clicked()
{
    connectTo *popUp = new connectTo(this);
    popUp->show();
    connect(popUp, SIGNAL(finished(int)),
            this, SLOT(onConnectTo_Closed(int)));
}

void MainWindow::onConnectTo_Closed(int result)
{
    /// TODO: get the vcf first
    connectTo *popUp = (connectTo *)sender();
    if(result == QDialog::Accepted)
    {
        QStringList splitedAddress = popUp->address().split(":");
        QString ip = popUp->address();
        quint16 port = 7331;
        if(splitedAddress.count() == 2)
        {
            ip = splitedAddress.first();
            port = splitedAddress.last().toInt();
        }
        User *newUser = new User(vCard::fromFile("user.vcf").last(),
                                 ip,
                                 port,
                                 this);
        connect(newUser, SIGNAL(sendStatus(QString, QString, QTcpSocket *)),
                this, SLOT(receiveUserStatus(QString, QString, QTcpSocket *)));
    }
    popUp->deleteLater();
}

void MainWindow::onItemSelected(QListWidgetItem* m_current,QListWidgetItem* m_previous)
{
    if(m_previous != 0)
    {
        ItemUser *oldWidgetUser = (ItemUser*)(ui->listW_users->itemWidget(m_previous));
        if(oldWidgetUser != 0)
            saveConv(oldWidgetUser);
    }
    if(m_current != 0)
    {
        ItemUser *curWidgetUser = (ItemUser*)(ui->listW_users->itemWidget(m_current));
        if(!QDir("convs").exists())
        {
            QDir().mkdir("convs");
        }
        QFile convFile("convs/" + curWidgetUser->userName()+".cnv");
        if (!convFile.open(QIODevice::ReadWrite | QIODevice::Text))
            return;
        QString conv = convFile.readAll();
        ui->txtEdit_messages->setHtml(conv);
        convFile.close();
    }
}
