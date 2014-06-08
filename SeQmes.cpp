#include "SeQmes.h"

SeQmes::SeQmes(QWidget *parent) : QMainWindow(parent)
{
    settings = new QSettings("seqmes.ini",QSettings::IniFormat);
    readConfig(); // Read config file
    readStyle(); // Read css file
    server = new Server; // Create an instance of the server

    this->setMinimumSize(400,200);
    this->setBaseSize(550,800);

    ////////////////
    // Layouts & containers
    ////////////////
    mainLayout = new QVBoxLayout;
    mainLayout->setMargin(5);
    mainLayout->setContentsMargins(5,5,5,5);
    mainContainer = new QWidget;
    mainContainer->setLayout(mainLayout);

    leftLayout = new QGridLayout;
    leftLayout->setMargin(0);
    leftLayout->setContentsMargins(0,0,0,0);
    leftContainer = new QWidget(this);
    leftContainer->setLayout(leftLayout);
    leftContainer->setObjectName("leftContainer");
    leftContainer->setMinimumWidth(120);
    leftContainer->setMaximumWidth(300);

    messageSplitter = new QSplitter(Qt::Vertical,this);
    messageSplitter->setObjectName("messageSplitter");
    messageSplitter->setHandleWidth(1);

    rightLayout = new QGridLayout;
    rightLayout->setMargin(0);
    rightLayout->setContentsMargins(0,0,0,0);
    rightContainer = new QWidget(this);
    rightContainer->setObjectName("rightContainer");
    rightContainer->setLayout(rightLayout);

    splitterContainer = new QSplitter(this);
    splitterContainer->setObjectName("splitterLayout");
    splitterContainer->setHandleWidth(6);

    bottomLayout = new QHBoxLayout;
    bottomLayout->setMargin(0);
    bottomLayout->setContentsMargins(0,0,0,0);
    bottomContainer = new QWidget(this);

    ////////////////
    // Labels
    ////////////////
    clientsStringList = new QStringList;
    #ifdef CL_DEBUG
    clientsLabel = new QLabel("Clients");
    clientsListWidget = new QListWidget; // Lists of all clients connected
    clientsListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    #endif

    lbl_contacts = new QLabel(tr("No contacts"));
    lbl_contacts->setObjectName("lbl_Contacts");
    list_contacts = new QList<Client*>;
    listWidget_contacts = new QListWidget; // Lists of all users who we are connected to
    listWidget_contacts->setObjectName("listWidget_contacts");
    listWidget_contacts->setContextMenuPolicy(Qt::CustomContextMenu);
    listWidget_contacts->setFocusPolicy(Qt::NoFocus);

    statusGlobal = new QLabel;
    statusGlobal->setText(tr("Initialized"));

    statusServer = new QLabel;
    statusServer->setText(tr("Stopped"));

    bottomLayout->addWidget(statusGlobal);
    bottomLayout->addWidget(statusServer,0,Qt::AlignRight);
    bottomLayout->setMargin(0);
    bottomContainer->setMaximumHeight(14);
    bottomContainer->setLayout(bottomLayout);


    ////////////////
    // Inputs
    ////////////////
    messagesBox = new QTextEdit;
    messagesBox->setObjectName("messagesBox");
    messagesBox->setReadOnly(true);

    input_message = new QTextEdit ;
    input_message->installEventFilter(this);
    input_message->setReadOnly(false);
    input_message->setObjectName("input_message");
    input_message->setFrameShape(QFrame::StyledPanel);
    input_message->setFrameShadow(QFrame::Plain);
    input_message->setPlaceholderText(tr("Write a message"));

    // TODO: remove
    input_address = new QLineEdit;
    input_address->setObjectName("input_address");
    input_address->setText("127.0.0.1:" + QString::number(configServerPort));

    btnConnect = new QPushButton(tr("Connect"));

    ////////////////
    // Setting widget in layouts
    ////////////////
    leftLayout->addWidget(lbl_contacts,0,1);
    leftLayout->addWidget(listWidget_contacts,1,1);

    messageSplitter->addWidget(messagesBox);
    messageSplitter->addWidget(input_message);

    rightLayout->addWidget(messageSplitter,0,0,1,2);
    rightLayout->addWidget(input_address,2,0);
    rightLayout->addWidget(btnConnect,2,1);

    splitterContainer->addWidget(leftContainer);
    splitterContainer->addWidget(rightContainer);
    QList<int> sizes;
    sizes.push_back(150);
    sizes.push_back(400);
    #ifdef CL_DEBUG
    splitterContainer->addWidget(clientsListWidget);
    #endif
    splitterContainer->setSizes(sizes);
    splitterContainer->setStretchFactor(0,0);
    splitterContainer->setStretchFactor(1,1);

    mainLayout->addWidget(splitterContainer);
    mainLayout->addWidget(bottomContainer);

    this->setCentralWidget(mainContainer);

    ////////////////
	// Menu Bar
    ////////////////
    menuFile = menuBar()->addMenu(tr("SeQmes"));
    menuUsers = menuBar()->addMenu(tr("Server"));

    actionReloadConfig = new QAction(tr("&Reload config"), this);
    actionExit = new QAction(tr("&Exit"), this);
    actionStartServer = new QAction(tr("Start"), this);
    actionStopServer = new QAction(tr("Stop"), this);
    actionStopServer->setDisabled(true);
    action_clientDisconnect = new QAction(tr("Disconnect"),this);

    menuFile->addAction(actionExit);
    menuFile->addAction(actionReloadConfig);
    menuUsers->addAction(actionStartServer);
    menuUsers->addAction(actionStopServer);

    ////////////////
    // Connecting signals
    ////////////////
    //connect(input_message, SIGNAL(returnPressed()), this, SLOT(sendMessage()));
    connect(btnConnect, SIGNAL(clicked()), this, SLOT(btnConnectPressed()));
    connect(listWidget_contacts, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(menuUsersList(const QPoint &)));
    connect(listWidget_contacts, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(displayConversation(QListWidgetItem*)));
    connect(listWidget_contacts, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(currentItemChanged(QListWidgetItem*,QListWidgetItem*)));

    connect(actionExit, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(actionReloadConfig, SIGNAL(triggered()), this, SLOT(readConfig()));
    connect(actionReloadConfig, SIGNAL(triggered()), this, SLOT(readContacts()));
    connect(actionReloadConfig, SIGNAL(triggered()), this, SLOT(readStyle()));
    connect(actionStartServer, SIGNAL(triggered()), this, SLOT(startServer()));
    connect(actionStopServer, SIGNAL(triggered()), server, SLOT(stop()));

    connect(server, SIGNAL(sendStatus(QString,QString)), this, SLOT(receiveStatusServer(QString,QString)));

    readStyle();
    this->show(); // Show the window

    startServer(); // Start server
    readContacts(); // Read vcf dir

}


SeQmes::~SeQmes()
{
}

// void readConfig()
// parse seqmes.ini to get the config of seqmes
//
void SeQmes::readConfig()
{
    settings->beginGroup("Server");
    if(settings->value("port") == QVariant::Invalid)
    {
        settings->setValue("port","7331");
    }
    if(settings->value("ip") == QVariant::Invalid)
    {
        settings->setValue("ip","0.0.0.0");
    }
    configServerIp = settings->value("ip").toString();
    configServerPort = settings->value("port").toUInt();
    settings->endGroup();
}

void SeQmes::readStyle()
{
    QFile fichier("seqmes.css");
    if(fichier.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream flux(&fichier);
        styleFile = flux.readAll();
        this->setStyleSheet(styleFile);
    }
}

void SeQmes::readContacts()
{
    QList<vCard> vcards;
    QStringList listFilter;
    listFilter << "*.vcf";

    QDirIterator dirIterator("vcf", listFilter ,QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);

    while(dirIterator.hasNext())
    {
        vcards << vCard::fromFile(dirIterator.next());
    }



    if (!vcards.isEmpty())
    {
        for(int i = 0; i < vcards.count(); i++)
        {
            vCard vcard = vcards.at(i);
            vCardProperty adress = vcard.property("IP");
            vCardProperty port = vcard.property("PORT");

            QString currentAddress = adress.value() + ":" + port.value() ;
            if(checkAddress(currentAddress)) // Test if address doesn't look aberrant
            {
                connectToUser(currentServerName,currentServerPort,false,vcard);
            }
            else
            {
                // TODO : log infos
            }
        }
    }
}

void SeQmes::displayConversation(QListWidgetItem*)
{
    QFile conversation;
    conversation.setFileName("convs/maxaille.txt");
    if(conversation.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        QTextStream out(&conversation);

    }
}

void SeQmes::currentItemChanged(QListWidgetItem* current,QListWidgetItem* previous)
{
    if(current != 0 && current->listWidget()->count() != 0)
    {
        lbl_contacts->setHidden(true);
        listWidget_contacts->setStyleSheet("QListWidget#listWidget_contacts{border: 1px solid #DDD;}");
        qDebug(QString::number(current->listWidget()->count()).toLatin1());
    } else {
        lbl_contacts->setHidden(false);
        listWidget_contacts->setStyleSheet("QListWidget#listWidget_contacts{border-top:none;}");

    }
}

void SeQmes::menuUsersList(const QPoint &pos)
{
    QListWidgetItem *currentItem = listWidget_contacts->itemAt(pos);
    if(currentItem){
        int row = listWidget_contacts->row(currentItem);
        QMenu contextMenu(this);
        contextMenu.addAction(new QAction(currentItem->text(), this));
        // TODO: connect to copyboard
        contextMenu.addAction(action_clientDisconnect);
        connect(action_clientDisconnect, SIGNAL(triggered()), list_contacts->at(row) , SLOT(disconnect()));
        contextMenu.exec(listWidget_contacts->mapToGlobal(pos));
    }
    else
    {
        QMenu contextMenu(this);
        contextMenu.addAction(new QAction("test", this));
        contextMenu.exec(listWidget_contacts->mapToGlobal(pos));
    }
}

/*
	Start server depending of the ip:port in the config.json file
*/
void SeQmes::startServer(){
    server->start(configServerIp,configServerPort);
}

/*
	get address and create a Client who will try to connect to it
	check via checkAddress() if address seems correct
check if no client is already connected to address:port
*/
void SeQmes::btnConnectPressed()
{
	QString currentAddress = input_address->text(); 
    if(checkAddress(currentAddress)) // Test if address doesn't look aberrant
    {
        connectToUser(currentServerName,currentServerPort, true);
    }
}

void SeQmes::connectToUser(QString ip, QString port, bool userQuery, vCard vcard)
{
	QString currentAddress = ip + ":" + port;
	// check if no client is already connected to address:port
    for(int i=0;i<listWidget_contacts->count();i++)
    {
        if(((UserItem*)listWidget_contacts->itemWidget(listWidget_contacts->item(i)))->adress->text() == currentAddress)
		{
			if(userQuery)
                messagesBox->append(tr("Already connected !"));
			return;
		}
	}
    // create a Client and try to connect
    Client *client = new Client(vcard);
    connect(client, SIGNAL(sendStatus(QString,QString)), this, SLOT(receiveStatusClient(QString,QString)));
    if(vcard.count() == 0)
    {
        client->connectToServer(currentServerName,currentServerPort);
    }
    else
    {
        client->connectToServer(currentServerName,currentServerPort,vcard);
    }
}

/*
	Check if address seems correct or not
	Check IPv4/domain(convert it to IPv4)
	TODO: add checking of IPV6
*/
bool SeQmes::checkAddress(QString address) 
{
    QRegExp regexp_ipv4("(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)"); // 0.0.0.0 -> 255.255.255.255
    QRegExp regexp_domain("([a-zA-Z0-9]([a-zA-Z0-9\\-]{0,61}[a-zA-Z0-9])?\\.)+[a-zA-Z]{2,6}");  // Domain format
	QStringList serverInfo = address.split(":");
	if(serverInfo.size()!=2) return false; // If adresse:port isn't in correct format, return false

	currentServerName = serverInfo[0]; // Server name, or Ip, according to what is entered

	if(regexp_ipv4.exactMatch(currentServerName)) // If servername is an ipv4
	{
		currentServerIp = currentServerName; // Directly get ip from serverName
	}
	//TODO: add IPv6 
	else if(regexp_domain.exactMatch(currentServerName) && // If servername is a domain
			QHostInfo::fromName(currentServerName).addresses().count() == 1) // If domain return 1 IP
	{
		currentServerIp = QHostInfo::fromName(currentServerName).addresses().at(0).toString(); // Get this Ip
	}
	else return false;

	currentServerPort = serverInfo[1] ;

	//Test if port seems correct (num char and not outlier values)
    if(!(QRegExp("\\d{1,5}").exactMatch(currentServerPort) && 0 < currentServerPort.toInt() && currentServerPort.toInt() < 65535)) return false;

	//If no error, return true
	return true;
}

/*
	Get message and send it to the selected user
	Then, clear the message input
*/
void SeQmes::sendMessage()
{
    QString currentMessage = input_message->toPlainText();
    if(currentMessage == "/tohtml")
    {
        QFile outfile;
        outfile.setFileName("test.txt");
        outfile.open(QIODevice::Append);
        QTextStream out(&outfile);
        out << messagesBox->toHtml() << endl;
        return;
    }
    if(listWidget_contacts->selectedItems() != QList<QListWidgetItem*>() && currentMessage != "")
	{
        int row = listWidget_contacts->row(listWidget_contacts->selectedItems().at(0));
        Client *currentUser = list_contacts->at(row);
        currentUser->sendData(currentMessage.replace("<","&lt;").replace(">","&gt;"));
        messagesBox->append(tr("[Me] %0").arg(currentMessage));
        input_message->clear();
	}
}

void SeQmes::receiveStatusClient(QString status, QString infos)
{
	Client *client = (Client*)sender();
	if(status == "connected")
	{
		QString currentAddress = client->connectedTo;
        QString dispText = currentAddress;
        QString personnalMessage;
        list_contacts->append(client);
        if(client->clientvCard.count() != 0)
        {
            if(client->clientvCard.contains("NICKNAME") && client->clientvCard.property("NICKNAME").value() != "")
            {
                dispText = client->clientvCard.property("NICKNAME").value();
            }
            else if(client->clientvCard.contains("FN") && client->clientvCard.property("FN").value() != "")
            {
                dispText = client->clientvCard.property("FN").value();
            }
            else if(client->clientvCard.contains("N") && client->clientvCard.property("N").value() != "")
            {
                QStringList values = client->clientvCard.property("N").values();
                dispText = values.at(vCardProperty::Firstname) + " " + values.at(vCardProperty::Lastname);
            }

            if(client->clientvCard.contains("NOTE") && client->clientvCard.property("NOTE").value() != "")
            {
                personnalMessage = client->clientvCard.property("NOTE").value();
            }
        }
        UserItem *user = new UserItem(dispText,personnalMessage,client->connectedTo,client->clientvCard,false,listWidget_contacts);
        QListWidgetItem *item = new QListWidgetItem();
        item->setSizeHint(QSize(140,45));
        user->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        listWidget_contacts->addItem(item);
        listWidget_contacts->setItemWidget(item,user);

        if(list_contacts->count()>1)
		{
            statusGlobal->setText(tr("Connected to %1 users").arg(list_contacts->count()));
		}
		else 
		{
            statusGlobal->setText(tr("Connected to %1").arg(client->connectedToIp));
		}
        listWidget_contacts->setCurrentRow((listWidget_contacts->count()-1));
		if(server->status) // Check if server is started and not connect to itself
        {
            client->sendData("connect:" + client->connectedToIp + ":" + server->listeningPort, "qry");
		}
		//action_clientDisconnect->setEnabled(true); // Utilit� ???
	}
	else if(status == "pending")
	{
        statusGlobal->setText(tr("Connecting to %1...").arg(infos));
	}
	else if(status == "disconnected")
	{
		quint16 index = -1;
        if(client->connectedTo == "")
        {
            statusGlobal->setText(tr("Unable to connect to %1").arg(infos));
            return;
        }
        for(int i=0;i<listWidget_contacts->count();i++)
        {
            if(((UserItem*)listWidget_contacts->itemWidget(listWidget_contacts->item(i)))->adress->text() == client->connectedTo)
			{
                index = i;
                list_contacts->removeAt(index);
                listWidget_contacts->takeItem(index);
                statusGlobal->setText(tr("%1 disconnected").arg(client->connectedToIp));
				break;
			}

        }
	}
}

void SeQmes::receiveStatusServer(QString status, QString infos)
{
    if(status == "started")
    {
        statusServer->setText(tr("Started on port %1").arg(infos));
        actionStartServer->setDisabled(true);
        actionStopServer->setEnabled(true);
    }
    else if(status == "stopped")
    {
        statusServer->setText(tr("Stopped"));
        actionStopServer->setDisabled(true);
        actionStartServer->setEnabled(true);
    }
    else if(status == "error")
    {
        messagesBox->append(infos);
    }
    else if(status == "data") // Format: ip:port:type:infos(:next)
    {
        QStringList splitedInfos = infos.split(":");
        if(splitedInfos.count() > 3)
		{
			QString fromIp = splitedInfos[0];
            // QString fromPort = splitedInfos[1]; // useless until you want to display port
			QString type = splitedInfos[2];
			if(type == "msg")
			{
                // do not broke message with colonne
                QString dataMessage = "";
                for(int i=3;i<splitedInfos.length();i++)
                {
                    dataMessage += splitedInfos[i];
                    if(i!=splitedInfos.length()-1)
                    {
                        dataMessage += ":";
                    }
                }
                dataMessage.replace("<","&lt;").replace(">","&gt;");
                QString message = tr("[%0] %1").arg(fromIp,dataMessage); // Text to display
                //QString message = tr("[%0] %1").arg(fromIp,":"+fromPort,dataMessage); // Add port
                messagesBox->append(message);
                alert();
			}
			else if(type == "qry")
            {
                QString query = splitedInfos[3];
				if(query == "connect"  && splitedInfos.count() == 6)
				{
					QString ip = splitedInfos[4];
					QString port = splitedInfos[5];
					QString address = ip + ":" + port;
					if(checkAddress(address)) // Test if address doesn't look aberrant
					{
                        connectToUser(ip,port);
					}
				}
			}
        }
    }
    else if(status == "newconnection")
    {
        clientsStringList->append(infos);
#ifdef CL_DEBUG
        clientsListWidget->addItem(infos);
#endif
        messagesBox->append(tr("%1 connected").arg(infos));
    }
    else if(status == "disconnection")
    {
#ifdef CL_DEBUG
        quint16 index = clientsStringList->indexOf(infos);
        clientsListWidget->takeItem(index);
#endif
        clientsStringList->removeOne(infos);
        messagesBox->append(tr("%1 disconnected").arg(infos));
    }
}

bool SeQmes::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if(obj == input_message)
        {
            switch (keyEvent->key()) {
                case Qt::Key_Escape:
                break;
                case Qt::Key_Return:
                sendMessage();
                return true;
                break;
            }
        }
        return QObject::eventFilter(obj, event);
    }
    else return false;
}

void SeQmes::alert()
{
    QApplication::alert(this);
}
