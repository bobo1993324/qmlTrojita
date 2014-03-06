#include <QApplication>
#include <QQuickView>
#include <QUrl>
#include <QSettings>
#include <QQmlContext>
#include <QDebug>
#include <QQuickItem>
#include <QDir>
#include <QJsonDocument>

#include "plugin/settings.h"
#include "plugin/mailBox.h"
#include "plugin/messages.h"
#include "plugin/messageDetails.h"
#include "plugin/attachments.h"
#include "plugin/sendmail.h"
#include "plugin/alert.h"
#include "plugin/mailAddress.h"

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

#include "mailBackend.h"
class EmailApplication : public QApplication
{
    Q_OBJECT
public:
    EmailApplication(int& argc, char** argv, QSettings * setting);
    int exec();
public slots:
    //qml
    void mailBoxClicked(QString name);
    void messageClicked(int uid);

    void addingAccount(QVariant q);
    void removingAccount(int idx);
    void setCurrentAccount(int idx);

private:
    void createView();
    void registerQml();

    void msgListClicked(const QModelIndex &index);

    QSettings * m_settings;
    QQuickView *m_view;

    //trojita
    Imap::Mailbox::Model *model;
    Imap::Mailbox::MailboxModel *mboxModel;
//    Imap::Mailbox::PrettyMailboxModel *prettyMboxModel;
    Imap::Mailbox::MsgListModel *msgListModel;
    Imap::Mailbox::NetworkWatcher *m_networkWatcher;
//    Imap::Mailbox::ThreadingMsgListModel *threadingMsgListModel;
//    Imap::Mailbox::PrettyMsgListModel *prettyMsgListModel;

    //qml
    TrojitaMailBoxModel * trojitaMailBoxModel ;
    TrojitaMessagesModel * trojitaMessagesModel ;
    TrojitaMessageDetails * trojitaMessageDetails ;
    TrojitaAttachmentsModel * trojitaAttachmentsModel ;
    TrojitaSendMail * trojitaSendMail ;
    TrojitaAlert * trojitaAlert;
    //this array
    QList<MailBackend *> mailBackendList;
    int mailCurrentIndex;
};
