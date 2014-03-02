#include <QObject>
#include <QAbstractListModel>
#include <QDebug>
#include <QDate>
#include "Imap/Model/MailboxTree.h"
#include "Imap/Model/MsgListModel.h"
#include "Imap/Model/Utils.h"
class TrojitaMessage
{
public:
    TrojitaMessage(QString subject, uint uid, QString sender, bool unread, QDateTime qdt,
                    bool isStared)
        : m_subject(subject), m_uid(uid), m_sender(sender), m_unread(unread), m_qdt(qdt),
            m_isStared(isStared){}
    QString subject() const;
    uint uid() const;
    QString sender() const;
    bool unread() const;
    QString time() const;
    bool isStared() const;

    
private:
    QString m_subject;
    uint m_uid;
    QString m_sender;
    bool m_unread;
    QDateTime m_qdt;
    bool m_isStared;
};

class TrojitaMessagesModel : public QAbstractListModel{
    Q_OBJECT
    Q_PROPERTY(QString mailBoxName READ getMailBoxName WRITE setMailBoxName NOTIFY mailBoxNameChanged)
    Q_PROPERTY(int displayCount READ displayCount WRITE setDisplayCount NOTIFY displayCountChanged)
    Q_PROPERTY(int messageCount READ messageCount)
public:
    enum TrojitaMessagesModelRoles {
        SubjectRole = Qt::UserRole + 1,
        UidRole,
        SenderRole,
        UnreadRole,
        TimeRole,
        StarRole
    };
    TrojitaMessagesModel(){};
    TrojitaMessagesModel(Imap::Mailbox::MsgListModel * m_msgList);
    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    void addTrojitaMessage(TrojitaMessage tm);
    void setMsgListModel(Imap::Mailbox::MsgListModel * m_msgListModel);

    QString getMailBoxName();
    void setMailBoxName(QString);
    int displayCount();
    void setDisplayCount(int count);
    int messageCount();
    Q_INVOKABLE void setStar(int uid, bool b);
signals:
    void mailBoxNameChanged();
    void displayCountChanged();
public slots:
    void msgModelDataChanged(QModelIndex,QModelIndex);
    void msgModelMailBoxChanged(QModelIndex);
    void msgModelRowInserted(QModelIndex, int, int);

    void reloadMessages();
private:
    void addChildren(QModelIndex parent);

    QList<TrojitaMessage> m_msg_list;
    Imap::Mailbox::MsgListModel * m_msgListModel;
    QString m_mailBoxName;
    int m_displayCount = 0;
};

