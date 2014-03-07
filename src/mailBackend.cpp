#include "mailBackend.h"
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

MailBackend::MailBackend(QVariant account, QSettings * default_settings){
    m_settings = new QHash<QString, QVariant>();
    QStringList keys = default_settings->allKeys();
    Q_FOREACH(QString key, keys) {
        (*m_settings)[key] = default_settings->value(key);
    }
    QMap<QString, QVariant> accountMap = account.toMap();
    keys = accountMap.keys();
    Q_FOREACH(QString key, keys) {
        (*m_settings)[key] = accountMap.value(key);
    }
    setupModels();
    model->reloadMailboxList();
}

MailBackend::~MailBackend(){
    nukeModels();
    delete m_settings;
}

void MailBackend::nukeModels()
{
    m_networkWatcher->setNetworkOffline();
    delete msgListModel;
    msgListModel = 0;
    delete mboxModel;
    mboxModel = 0;
    delete model;
    model = 0;
}

void MailBackend::setupModels(){
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

	//Get home directory of the current user
	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;

//    QString cacheDir = Common::writablePath(Common::LOCATION_CACHE);
    //save to local directory because of confinement
    QString cacheDir = QString(homedir) + QString("/.local/share/com.bobo-324.qmltrojita/.cache") + m_settings->value(SettingsNames::imapUserKey).toString();
    Imap::Mailbox::AbstractCache *cache = 0;

    //TODO enable cache in production
    (*m_settings)[SettingsNames::cacheOfflineKey] = SettingsNames::cacheOfflineAll;
    bool shouldUsePersistentCache = m_settings->value(SettingsNames::cacheOfflineKey).toString() != SettingsNames::cacheOfflineNone;
//    bool shouldUsePersistentCache = false;
    if (shouldUsePersistentCache) {
//        qDebug() << "cache dir is " <<  cacheDir;
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
        cache = new Imap::Mailbox::CombinedCache(this,QLatin1String("trojita-imap-cache"), cacheDir);
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
    msgListModel = new Imap::Mailbox::MsgListModel(this, model);
    msgListModel->setObjectName(QLatin1String("msgListModel"));

//    connect (model, SIGNAL(mailBoxModelChanged(QModelIndex, QModelIndex)), mboxModel, SLOT(emitDataChanged()));
    connect(model, SIGNAL(authRequested()), this, SLOT(authenticationRequested()), Qt::QueuedConnection);
    connect(model, SIGNAL(connectionError(const QString &)), this, SLOT(connectionError(const QString &)));
    connect(model, SIGNAL(authAttemptFailed(QString)), this, SLOT(authenticationFailed(QString)));
    connect(model, SIGNAL(needsSslDecision(QList<QSslCertificate>,QList<QSslError>)),
            this, SLOT(sslErrors(QList<QSslCertificate>,QList<QSslError>)), Qt::QueuedConnection);
    connect(model, SIGNAL(logged(uint,Common::LogMessage)), this, SLOT(slotImapLogged(uint,Common::LogMessage)));
    connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(slotNewMessageRecieved()));
}

void MailBackend::authenticationRequested()
{
    QString user = m_settings->value(Common::SettingsNames::imapUserKey).toString();
    QString pass = m_settings->value(Common::SettingsNames::imapPassKey).toString();
    model->setImapUser(user);
    model->setImapPassword(pass);
}
void MailBackend::authenticationFailed(const QString &message)
{
//    qDebug() << "Login Failed: " << message;
    emit loginError(message, m_settings->value(Common::SettingsNames::imapUserKey).toString());

}
void MailBackend::sslErrors(const QList<QSslCertificate> &certificateChain, const QList<QSslError> &errors)
{
    //TODO popup to accecpt unknown key.
    model->setSslPolicy(certificateChain, errors, true);
}
void MailBackend::slotImapLogged(uint parserId, const Common::LogMessage &message){
//    qDebug() << "From parser " << parserId << ": " << message.message;
}
void MailBackend::slotNewMessageRecieved(){
    QModelIndex mailbox = model->index(1, 0, QModelIndex());
//    qDebug() << "slotNewMessageRecieved" << m_settings->value(Common::SettingsNames::imapUserKey).toString()
//             << " " << mailbox.data(Imap::Mailbox::RoleUnreadMessageCount).toInt();
    if (mailbox.isValid()) {
        //TODO hand count RoleUnreadMessageCount becuase it is not updated unless at start =.=
        Q_ASSERT(mailbox.data(Imap::Mailbox::RoleMailboxName).toString() == QLatin1String("INBOX"));
        emit updateUnreadCount(m_settings->value(Common::SettingsNames::imapUserKey).toString(),
                    mailbox.data(Imap::Mailbox::RoleUnreadMessageCount).toInt());

    }
}
void MailBackend::cacheError(QString message){
    qDebug() << tr("IMAP Cache Error") <<
                          tr("The caching subsystem managing a cache of the data already "
                             "downloaded from the IMAP server is having troubles. "
                             "All caching will be disabled.\n\n%1").arg(message);
    if (model)
        model->setCache(new Imap::Mailbox::MemoryCache(model));
}
void MailBackend::connectionError(QString message){
    if(message.indexOf("Too many commands before") < 0)
        emit loginError(message, m_settings->value(Common::SettingsNames::imapUserKey).toString());
}
