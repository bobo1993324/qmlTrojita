#include <QApplication>
#include <QQuickView>
#include <QUrl>
#include <QSettings>
#include <QQmlContext>
#include <QDebug>
#include <QQuickItem>
#include <QDir>

#include "plugin/settings.h"
#include "plugin/mailBox.h"
#include "plugin/messages.h"
#include "plugin/messageDetails.h"
#include "plugin/attachments.h"
#include "plugin/sendmail.h"

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
class EmailApplication : public QApplication
{
    Q_OBJECT
public:
    EmailApplication(int& argc, char** argv, QSettings * setting);
    int exec();
public slots:
    //qml
    void settingUpdated();
    void mailBoxClicked(QString name);
    void messageClicked(int uid);
    //trojita
    void cacheError(const QString &message);
    void authenticationRequested();
    void authenticationFailed(const QString &message);
    void slotImapLogged(uint parserId, const Common::LogMessage &message);
    void sslErrors(const QList<QSslCertificate> &certificateChain, const QList<QSslError> &errors);

private:
    void createView();
    void registerQml();

    void defaultSetting();
    void reconnectContextModels();

    void msgListClicked(const QModelIndex &index);

    //trojita
    void nukeModels();
    void setupModels();

    QSettings * m_settings;
    QQuickView *m_view;

    //trojita
    Imap::Mailbox::Model *model;
    Imap::Mailbox::MailboxModel *mboxModel;
    Imap::Mailbox::PrettyMailboxModel *prettyMboxModel;
    Imap::Mailbox::MsgListModel *msgListModel;
    Imap::Mailbox::NetworkWatcher *m_networkWatcher;
    Imap::Mailbox::ThreadingMsgListModel *threadingMsgListModel;
    Imap::Mailbox::PrettyMsgListModel *prettyMsgListModel;

    //qml
    TrojitaSetting * trojitaSetting ;
    TrojitaMailBoxModel * trojitaMailBoxModel ;
    TrojitaMessagesModel * trojitaMessagesModel ;
    TrojitaMessageDetails * trojitaMessageDetails ;
    TrojitaAttachmentsModel * trojitaAttachmentsModel ;
    TrojitaSendMail * trojitaSendMail ;
};
