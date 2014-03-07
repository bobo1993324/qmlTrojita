#include "qmltrojita.h"
EmailApplication::EmailApplication(int& argc, char** argv, QSettings * settings)
    : QApplication(argc, argv),
      m_settings(settings),
      m_view(new QQuickView()),
      mailCurrentIndex(-1)
{
    //    defaultSetting();
    //    setupModels();
    registerQml();
    //    model->reloadMailboxList();
}
int EmailApplication::exec(){
    createView();
    return QApplication::exec();
}
void EmailApplication::createView(){
    m_view->setSource(QUrl::fromLocalFile("qml/ubuntu-emailclient-app.qml"));

    connect(m_view->rootObject()->findChild<QQuickItem *>("settingPage"),
            SIGNAL(addingAccount(QVariant)), this, SLOT(addingAccount(QVariant)));
    connect(m_view->rootObject()->findChild<QQuickItem *>("settingPage"),
            SIGNAL(setCurrentAccount(int)), this, SLOT(setCurrentAccount(int)));

    connect(m_view->rootObject()->findChild<QQuickItem *>("settingPage"),
            SIGNAL(removingAccount(int)), this, SLOT(removingAccount(int)));

    connect(m_view->rootObject()->findChild<QQuickItem *>("foldersPage"),
            SIGNAL(mailBoxClicked(QString)), this, SLOT(mailBoxClicked(QString)));

    connect(m_view->rootObject()->findChild<QQuickItem *>("messagesPage"),
            SIGNAL(messageClicked(int)), this, SLOT(messageClicked(int)));
    connect(m_view->rootObject()->findChild<QQuickItem *>("messagesPage"),
            SIGNAL(goOnline()), this, SLOT(goOnline()));
    connect(m_view->rootObject()->findChild<QQuickItem *>("messagesPage"),
            SIGNAL(goOffline()), this, SLOT(goOffline()));

    m_view->setResizeMode(QQuickView::SizeRootObjectToView);
    m_view->show();
}

void EmailApplication::registerQml(){
    //TODO user reconnect Context Models
    trojitaMailBoxModel=new TrojitaMailBoxModel();
    trojitaMessagesModel = new TrojitaMessagesModel();
    trojitaMessageDetails = new TrojitaMessageDetails();
    trojitaSendMail = new TrojitaSendMail();
    trojitaAlert = new TrojitaAlert();
    trojitaNetwork = new TrojitaNetwork();

    connect(trojitaMailBoxModel, SIGNAL(clickMailBox(QString)), this, SLOT(mailBoxClicked(QString)));

    qmlRegisterType<TrojitaAttachment,1>("QmlTrojita", 0, 1, "TrojitaAttachment");
    qmlRegisterType<MailAddress>("QmlTrojita", 0, 1, "MailAddress");
    m_view->rootContext()->setContextProperty("TROJITA_MAILBOX", trojitaMailBoxModel);
    m_view->rootContext()->setContextProperty("TROJITA_MESSAGES", trojitaMessagesModel);
    m_view->rootContext()->setContextProperty("TROJITA_MESSAGE_DETAILS", trojitaMessageDetails);
    m_view->rootContext()->setContextProperty("TROJITA_SEND_MAIL", trojitaSendMail);
    m_view->rootContext()->setContextProperty("TROJITA_ALERT", trojitaAlert);
    m_view->rootContext()->setContextProperty("TROJITA_NETWORK", trojitaNetwork);
}

void EmailApplication::setCurrentAccount(int idx){
    //reconnect context model
    qDebug() << "setCurrent to " << idx;
    model = mailBackendList[idx]->model;
    mboxModel = mailBackendList[idx]->mboxModel;
    msgListModel = mailBackendList[idx]->msgListModel;
    trojitaMailBoxModel->setMailBoxModel(mailBackendList[idx]->mboxModel);
    trojitaMessagesModel->setMsgListModel(mailBackendList[idx]->msgListModel);
    trojitaSendMail->setModel(mailBackendList[idx]->model);
    trojitaSendMail->setSettings(mailBackendList[idx]->m_settings);
    trojitaNetwork->setModel(mailBackendList[idx]->model);
}

void EmailApplication::mailBoxClicked(QString name){
    qDebug() << name << " clicked";
    //find the index of name in mboxModel
    msgListModel->setMailbox(name);

    trojitaMessagesModel->setMailBoxName(name);
}

// Listen to the event: message is clicked from uid
void EmailApplication::messageClicked(int uid){
    qDebug() << "message " << uid << " clicked";
    //search in msgListModel for the QModelIndex corresponds to uid
    for(int i=0;i<msgListModel->rowCount();i++){
        if(msgListModel->data(msgListModel->index(i, 0), Imap::Mailbox::RoleMessageUid) == uid){
            QModelIndex qmi = msgListModel->index(i, 0);
            qDebug() << "find message. the subject is" << msgListModel->data(qmi, Imap::Mailbox::RoleMessageSubject);
            msgListClicked(qmi);
            break;
        }
    }
}

// Listen to the event: message is clicked using QModeIndex: index
void EmailApplication::msgListClicked(const QModelIndex &index)
{
    Q_ASSERT(index.isValid());

    if (qApp->keyboardModifiers() & Qt::ShiftModifier || qApp->keyboardModifiers() & Qt::ControlModifier)
        return;

    if (! index.data(Imap::Mailbox::RoleMessageUid).isValid())
        return;

    // Be sure to update the toolbar/actions with the state of the current message
    //    updateMessageFlags();

    // Mark mail as read
    QModelIndexList translatedIndexes;
    translatedIndexes << Imap::deproxifiedIndex(index);
    model->markMessagesRead(translatedIndexes, Imap::Mailbox::FLAG_ADD);

    // Because it's quite possible that we have switched into another mailbox, make sure that we're in the "current" one so that
    // user will be notified about new arrivals, etc.
    QModelIndex translated = Imap::deproxifiedIndex(index);
    model->switchToMailbox(translated.parent().parent());
    trojitaMessageDetails->setMessage(index);
}

void EmailApplication::addingAccount(QVariant account){
    MailBackend * newMailAccount = new MailBackend(account, m_settings);
    mailBackendList.append(newMailAccount);
    connect(newMailAccount, SIGNAL(updateUnreadCount(QString,int)), trojitaAlert, SLOT(slotSetAlert(QString, int)));
    connect(newMailAccount, SIGNAL(loginError(QString, QString)), trojitaAlert, SLOT(slotAuthenticateFailed(QString, QString)));
}

void EmailApplication::removingAccount(int idx){
    mailBackendList.removeAt(idx);
}

void EmailApplication::goOnline(){
    Q_FOREACH(MailBackend * m, mailBackendList){
        m->m_networkWatcher->setNetworkOnline();
    }
}

void EmailApplication::goOffline(){
    Q_FOREACH(MailBackend * m, mailBackendList){
        m->m_networkWatcher->setNetworkOffline();
    }
}
