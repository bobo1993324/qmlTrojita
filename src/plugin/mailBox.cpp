#include "mailBox.h"
TrojitaMailBoxModel::TrojitaMailBoxModel(Imap::Mailbox::MailboxModel * mbox)
{
    setMailBoxModel(mbox);
}
QString TrojitaMailBox::name() const{
    return m_name;
}

bool TrojitaMailBox::hasChildren() const{
    return m_hasChildren;
}

int TrojitaMailBox::unreadCount() const{
    return m_unreadCount;
}

void TrojitaMailBoxModel::mailBoxDataChanged(){
    beginRemoveRows(QModelIndex(), 0, rowCount()-1);
    m_mbox_list.clear();
    endRemoveRows();
    addChildren(QModelIndex());
    emit dataChanged(this->index(0), this->index(m_mbox_list.size()-1));
}

void TrojitaMailBoxModel::mailBoxInserted(QModelIndex parent, int first, int last){
    if(first == 0 && !parent.isValid()){
        //click first mailbox
        qDebug() << m_mbox->index(0,0).data(Imap::Mailbox::RoleMailboxName);
        emit clickMailBox(m_mbox->index(0,0).data(Imap::Mailbox::RoleMailboxName).toString());
    }
}

QHash<int, QByteArray> TrojitaMailBoxModel::roleNames() const{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[HasChildrenRole] = "hasChildren";
    roles[UnreadCountRole] = "unreadCount";
//    roles[LevelRole] = "level";
    return roles;
}
int TrojitaMailBoxModel::rowCount(const QModelIndex &) const {
    return m_mbox_list.count();
}

QVariant TrojitaMailBoxModel::data(const QModelIndex & index, int role) const {
    if (index.row() < 0 || index.row() > m_mbox_list.count())
        return QVariant();

    const TrojitaMailBox &tmb = m_mbox_list[index.row()];
    if (role == NameRole)
        return tmb.name();
    if (role == HasChildrenRole)
        return tmb.hasChildren();
    if (role == UnreadCountRole)
        return tmb.unreadCount();
    return QVariant();
}

void TrojitaMailBoxModel::addTrojitaMailBox(TrojitaMailBox tmb){
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_mbox_list << tmb;
    endInsertRows();
}


void TrojitaMailBoxModel::addChildren(QModelIndex parent){
    for(int i=0;i<m_mbox->rowCount(parent);i++){
        QString name = m_mbox->data(m_mbox->index(i,0, parent), Imap::Mailbox::RoleMailboxName).toString();
        bool hasChildren = m_mbox->data(m_mbox->index(i,0, parent), Imap::Mailbox::RoleMailboxHasChildMailboxes).toBool();
        int unreadCount = m_mbox->data(m_mbox->index(i, 0), Imap::Mailbox::RoleUnreadMessageCount).toInt();
        TrojitaMailBox tmb(name, hasChildren, unreadCount);
        addTrojitaMailBox(tmb);
        if(hasChildren){
            addChildren(m_mbox->index(i,0, parent));
        }
    }
}

void TrojitaMailBoxModel::setMailBoxModel(Imap::Mailbox::MailboxModel *mbox){
    m_mbox=mbox;
    disconnect(this, SLOT(mailBoxDataChanged()));
    connect(mbox, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(mailBoxInserted(QModelIndex,int,int)));
    connect(mbox, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(mailBoxDataChanged()));
    mailBoxDataChanged();
}
