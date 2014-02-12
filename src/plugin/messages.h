#include <QObject>
#include <QAbstractListModel>
#include <QDebug>

#include "Imap/Model/MsgListModel.h"
class TrojitaMessage
{
public:
    TrojitaMessage(QString title, uint uid) : m_title(title), m_uid(uid){}
    QString title() const;
    uint uid() const;
private:
    QString m_title;
    uint m_uid;
};

class TrojitaMessagesModel : public QAbstractListModel{
    Q_OBJECT
public:
    enum TrojitaMessagesModelRoles {
        TitleRole = Qt::UserRole + 1,
        UidRole
    };

    TrojitaMessagesModel(Imap::Mailbox::MsgListModel * m_msgList);
    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    void addTrojitaMessage(TrojitaMessage tm);
    void setMsgListModel(Imap::Mailbox::MsgListModel * m_msgListModel);

public slots:
    void msgModelDataChanged(QModelIndex,QModelIndex);

private:
    void addChildren(QModelIndex parent);
    QList<TrojitaMessage> m_msg_list;
    Imap::Mailbox::MsgListModel * m_msgListModel;

};

