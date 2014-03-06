#include <QObject>
#include <QAbstractListModel>
#include <QDebug>
#include "Imap/Model/MailboxModel.h"
#include "Imap/Model/ItemRoles.h"
class TrojitaMailBox
{
public:
    TrojitaMailBox(QString name, bool hasChildren, int unreadCount, int level = 0)
        : m_name(name), m_hasChildren(hasChildren), m_unreadCount(unreadCount), m_level(level){}
    QString name() const;
    bool hasChildren() const;
    int unreadCount() const;
private:
    QString m_name;
    bool m_hasChildren;
    int m_unreadCount;

    int m_level;
};

class TrojitaMailBoxModel : public QAbstractListModel{
    Q_OBJECT
public:
    enum TrojitaMailBoxModelRoles {
        NameRole = Qt::UserRole + 1,
        HasChildrenRole,
        UnreadCountRole,
        LevelRole
    };
    TrojitaMailBoxModel(){}
    TrojitaMailBoxModel(Imap::Mailbox::MailboxModel * mbox);
    void setMailBoxModel(Imap::Mailbox::MailboxModel * mbox);
    QHash<int, QByteArray> roleNames() const;

    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    void addTrojitaMailBox(TrojitaMailBox tmb);
public slots:
    void mailBoxDataChanged();

private:
    void addChildren(QModelIndex parent);
    QList<TrojitaMailBox> m_mbox_list;
    Imap::Mailbox::MailboxModel * m_mbox;
};

