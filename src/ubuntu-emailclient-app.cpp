#include "ubuntu-emailclient-app.h"
EmailApplication::EmailApplication(int& argc, char** argv, QSettings * settings)
    : QApplication(argc, argv),
      m_settings(settings),
      m_view(new QQuickView())
{
    defaultSetting();
    setupModels();
    registerQml();
}
int EmailApplication::exec(){
    createView();
    return QApplication::exec();
}
void EmailApplication::createView(){
    m_view->setSource(QUrl::fromLocalFile("qml/ubuntu-emailclient-app.qml"));
    connect(m_view->rootObject()->findChild<QQuickItem *>("settingPage"),
                     SIGNAL(settingChanged()), this, SLOT(settingUpdated()));
    connect(m_view->rootObject()->findChild<QQuickItem *>("mailBoxPage"),
                     SIGNAL(mailBoxClicked(QString)), this, SLOT(mailBoxClicked(QString)));
    connect(m_view->rootObject()->findChild<QQuickItem *>("messagesPage"),
                    SIGNAL(messageClicked(int)), this, SLOT(messageClicked(int)));
//    connect(msgListWidget->tree, SIGNAL(clicked(const QModelIndex &)), this, SLOT(msgListClicked(const QModelIndex &)));
    m_view->show();
}

void EmailApplication::registerQml(){
    //TODO user reconnect Context Models
    trojitaSetting=new TrojitaSetting(m_settings);
    trojitaMailBoxModel=new TrojitaMailBoxModel(mboxModel);
    trojitaMessagesModel = new TrojitaMessagesModel(msgListModel);
    trojitaAttachmentsModel = new TrojitaAttachmentsModel();
    trojitaMessageDetails = new TrojitaMessageDetails("", trojitaAttachmentsModel);
    trojitaSendMail = new TrojitaSendMail(m_settings, model);
    m_view->rootContext()->setContextProperty("TROJITA_MAILBOX", trojitaMailBoxModel);
    m_view->rootContext()->setContextProperty("TROJITA_SETTING", trojitaSetting);
    m_view->rootContext()->setContextProperty("TROJITA_MESSAGES", trojitaMessagesModel);
    m_view->rootContext()->setContextProperty("TROJITA_MESSAGE_DETAILS", trojitaMessageDetails);
    m_view->rootContext()->setContextProperty("TROJITA_ATTACHMENTS", trojitaAttachmentsModel);
    m_view->rootContext()->setContextProperty("TROJITA_SEND_MAIL", trojitaSendMail);
}
void EmailApplication::defaultSetting(){
    using Common::SettingsNames;
    m_settings->setValue(SettingsNames::imapMethodKey, SettingsNames::methodSSL);
    m_settings->setValue(SettingsNames::imapHostKey, "imap.gmail.com");
    m_settings->setValue(SettingsNames::imapPassKey, "z936563s");
    m_settings->setValue(SettingsNames::cacheOfflineKey, SettingsNames::cacheOfflineNone);
    m_settings->setValue(SettingsNames::msaMethodKey, SettingsNames::methodSMTP);
    m_settings->setValue(SettingsNames::smtpHostKey, "smtp.gmail.com");
    m_settings->setValue(SettingsNames::smtpPortKey, "587");
    m_settings->setValue(SettingsNames::smtpStartTlsKey, "true");
    m_settings->setValue(SettingsNames::smtpAuthKey, "true");
    m_settings->setValue(SettingsNames::smtpUserKey, "bobo1993324@gmail.com");
    m_settings->setValue(SettingsNames::smtpPassKey, "z936563s");
}

void EmailApplication::settingUpdated(){
    qDebug() << "setting Updated";
    nukeModels();
    setupModels();
    reconnectContextModels();
    model->reloadMailboxList();
}
void EmailApplication::reconnectContextModels(){
    trojitaMailBoxModel->setMailBoxModel(mboxModel);
    trojitaMessagesModel->setMsgListModel(msgListModel);
    trojitaSendMail->setModel(model);
}

void EmailApplication::nukeModels()
{
    m_networkWatcher->setNetworkOffline();
    delete prettyMsgListModel;
    prettyMsgListModel = 0;
    delete threadingMsgListModel;
    threadingMsgListModel = 0;
    delete msgListModel;
    msgListModel = 0;
    delete mboxModel;
    mboxModel = 0;
    delete prettyMboxModel;
    prettyMboxModel = 0;
    delete model;
    model = 0;
}
void EmailApplication::setupModels(){
    Imap::Mailbox::SocketFactoryPtr factory;
    Imap::Mailbox::TaskFactoryPtr taskFactory(new Imap::Mailbox::TaskFactory());

    using Common::SettingsNames;
    if (m_settings->value(SettingsNames::imapMethodKey).toString() == SettingsNames::methodTCP) {
        factory.reset(new Streams::TlsAbleSocketFactory(
                          m_settings->value(SettingsNames::imapHostKey).toString(),
                          m_settings->value(SettingsNames::imapPortKey, QString::number(Common::PORT_IMAP)).toUInt()));
        factory->setStartTlsRequired(m_settings->value(SettingsNames::imapStartTlsKey, true).toBool());
    } else if (m_settings->value(SettingsNames::imapMethodKey).toString() == SettingsNames::methodSSL) {
        factory.reset(new Streams::SslSocketFactory(
                          m_settings->value(SettingsNames::imapHostKey).toString(),
                          m_settings->value(SettingsNames::imapPortKey, QString::number(Common::PORT_IMAPS)).toUInt()));
    } else if (m_settings->value(SettingsNames::imapMethodKey).toString() == SettingsNames::methodProcess) {
        QStringList args = m_settings->value(SettingsNames::imapProcessKey).toString().split(QLatin1Char(' '));
        if (args.isEmpty()) {
            // it's going to fail anyway
            args << QLatin1String("");
        }
        QString appName = args.takeFirst();
        factory.reset(new Streams::ProcessSocketFactory(appName, args));
    } else {
        factory.reset(new Streams::FakeSocketFactory(Imap::CONN_STATE_LOGOUT));
    }

    QString cacheDir = Common::writablePath(Common::LOCATION_CACHE);
    Imap::Mailbox::AbstractCache *cache = 0;
//    qDebug()<<"cache directory is " << cacheDir;
    bool shouldUsePersistentCache = m_settings->value(SettingsNames::cacheOfflineKey).toString() != SettingsNames::cacheOfflineNone;
//    qDebug() << "shouldUsePersistentCache " << shouldUsePersistentCache;
    if (shouldUsePersistentCache) {
        if (! QDir().mkpath(cacheDir)) {
            //            QMessageBox::critical(this, tr("Cache Error"), tr("Failed to create directory %1").arg(cacheDir));
            qDebug() << "Cache Error: Failed to create directory " << cacheDir;
            shouldUsePersistentCache = false;
        }
        QFile::Permissions expectedPerms = QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner;
    }

    setProperty( "trojita-sqlcache-commit-period", QVariant(5000) );
    setProperty( "trojita-sqlcache-commit-delay", QVariant(1000) );

    if (! shouldUsePersistentCache) {
        cache = new Imap::Mailbox::MemoryCache(this);
    } else {
        cache = new Imap::Mailbox::CombinedCache(this, QLatin1String("trojita-imap-cache"), cacheDir);
        connect(cache, SIGNAL(error(QString)), this, SLOT(cacheError(QString)));
        if (! static_cast<Imap::Mailbox::CombinedCache *>(cache)->open()) {
            // Error message was already shown by the cacheError() slot
            cache->deleteLater();
            cache = new Imap::Mailbox::MemoryCache(this);
        } else {
            if (m_settings->value(SettingsNames::cacheOfflineKey).toString() == SettingsNames::cacheOfflineAll) {
                cache->setRenewalThreshold(0);
            } else {
                bool ok;
                int num = m_settings->value(SettingsNames::cacheOfflineNumberDaysKey, 30).toInt(&ok);
                if (!ok)
                    num = 30;
                cache->setRenewalThreshold(num);
            }
        }
    }
    model = new Imap::Mailbox::Model(this, cache, std::move(factory), std::move(taskFactory));
    m_networkWatcher = new Imap::Mailbox::NetworkWatcher(this, model);
    QMetaObject::invokeMethod(m_networkWatcher,
                              m_settings->value(SettingsNames::imapStartOffline).toBool() ?
                                  "setNetworkOffline" : "setNetworkOnline",
                              Qt::QueuedConnection);
    model->setObjectName(QLatin1String("model"));
    model->setCapabilitiesBlacklist(m_settings->value(SettingsNames::imapBlacklistedCapabilities).toStringList());
    if (m_settings->value(SettingsNames::imapEnableId, true).toBool()) {
        model->setProperty("trojita-imap-enable-id", true);
    }
    mboxModel = new Imap::Mailbox::MailboxModel(this, model);
    mboxModel->setObjectName(QLatin1String("mboxModel"));
    prettyMboxModel = new Imap::Mailbox::PrettyMailboxModel(this, mboxModel);
    prettyMboxModel->setObjectName(QLatin1String("prettyMboxModel"));
    msgListModel = new Imap::Mailbox::MsgListModel(this, model);
    msgListModel->setObjectName(QLatin1String("msgListModel"));
    threadingMsgListModel = new Imap::Mailbox::ThreadingMsgListModel(this);
    threadingMsgListModel->setObjectName(QLatin1String("threadingMsgListModel"));
    threadingMsgListModel->setSourceModel(msgListModel);
    prettyMsgListModel = new Imap::Mailbox::PrettyMsgListModel(this);
    prettyMsgListModel->setSourceModel(threadingMsgListModel);
    prettyMsgListModel->setObjectName(QLatin1String("prettyMsgListModel"));
    connect (model, SIGNAL(mailBoxModelChanged(QModelIndex, QModelIndex)), mboxModel, SLOT(emitDataChanged()));
    connect(model, SIGNAL(authRequested()), this, SLOT(authenticationRequested()), Qt::QueuedConnection);
    connect(model, SIGNAL(authAttemptFailed(QString)), this, SLOT(authenticationFailed(QString)));
    connect(model, SIGNAL(needsSslDecision(QList<QSslCertificate>,QList<QSslError>)),
            this, SLOT(sslErrors(QList<QSslCertificate>,QList<QSslError>)), Qt::QueuedConnection);
    connect(model, SIGNAL(logged(uint,Common::LogMessage)), this, SLOT(slotImapLogged(uint,Common::LogMessage)));

}

void EmailApplication::cacheError(const QString &message){
    qDebug()<< "IMAP Cache Error: " << message;
    if (model)
        model->setCache(new Imap::Mailbox::MemoryCache(model));
}

void EmailApplication::authenticationRequested()
{
    qDebug()<<"authentication requested";
    QString user = m_settings->value(Common::SettingsNames::imapUserKey).toString();
    QString pass = m_settings->value(Common::SettingsNames::imapPassKey).toString();
    model->setImapUser(user);
    model->setImapPassword(pass);
}

void EmailApplication::authenticationFailed(const QString &message)
{
    qDebug() << "Login Failed: " << message;
}

void EmailApplication::slotImapLogged(uint parserId, const Common::LogMessage &message){
    qDebug() << "From parser " << parserId << ": " << message.message;
}

void EmailApplication::sslErrors(const QList<QSslCertificate> &certificateChain, const QList<QSslError> &errors)
{
    //TODO popup to accecpt unknown key.
    model->setSslPolicy(certificateChain, errors, true);
}

void EmailApplication::mailBoxClicked(QString name){
    qDebug() << name << " clicked";
    //find the index of name in mboxModel
    msgListModel->setMailbox(name);

    trojitaMessagesModel->setMailBoxName(name);
}

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
void EmailApplication::msgListClicked(const QModelIndex &index)
{
    Q_ASSERT(index.isValid());

    if (qApp->keyboardModifiers() & Qt::ShiftModifier || qApp->keyboardModifiers() & Qt::ControlModifier)
        return;

    if (! index.data(Imap::Mailbox::RoleMessageUid).isValid())
        return;

    // Be sure to update the toolbar/actions with the state of the current message
//    updateMessageFlags();

    // Because it's quite possible that we have switched into another mailbox, make sure that we're in the "current" one so that
    // user will be notified about new arrivals, etc.
    QModelIndex translated = Imap::deproxifiedIndex(index);
    model->switchToMailbox(translated.parent().parent());
    trojitaMessageDetails->setMessage(index);
}

