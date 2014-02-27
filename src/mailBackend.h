#include <QSettings>
#include <QObject>
#include <QHash>
#include <QDir>
#include <QFile>

#include "Imap/Model/Model.h"
#include "Imap/Model/MemoryCache.h"
#include "Imap/Model/CombinedCache.h"
#include "Imap/Model/Model.h"
#include "Imap/Model/NetworkWatcher.h"
#include "Imap/Model/PrettyMailboxModel.h"
#include "Imap/Model/ThreadingMsgListModel.h"
#include "Imap/Model/PrettyMsgListModel.h"
#include "Imap/Model/MsgListModel.h"
#include "Imap/Model/Utils.h"
#include "Common/PortNumbers.h"
#include "Common/SettingsNames.h"
#include "Common/Paths.h"
#include "Common/MetaTypes.h"
#include "Streams/SocketFactory.h"

//this class represents all activity of one mail account
#ifndef MAILBACKEND
#define MAILBACKEND
class MailBackend : public QObject{
    Q_OBJECT
public:
    MailBackend(QVariant account, QSettings * default_settings);
    ~MailBackend();
    QHash<QString, QVariant> * m_settings;
    Imap::Mailbox::Model *model;
    Imap::Mailbox::MailboxModel *mboxModel;
    //    Imap::Mailbox::PrettyMailboxModel *prettyMboxModel;
    Imap::Mailbox::MsgListModel *msgListModel;
    Imap::Mailbox::NetworkWatcher *m_networkWatcher;
signals:
    void emitDataChanged();
    void updateUnreadCount(QString accountName, int unReadCount);

public slots:
    void authenticationRequested();
    void authenticationFailed(const QString &message);
    void sslErrors(const QList<QSslCertificate> &certificateChain, const QList<QSslError> &errors);
    void slotImapLogged(uint parserId, const Common::LogMessage &message);
    void slotNewMessageRecieved();
    void cacheError(QString);
private:
    void setupModels();
    void nukeModels();
};
#endif
