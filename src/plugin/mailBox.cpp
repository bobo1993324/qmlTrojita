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

void TrojitaMailBoxModel::mailBoxDataChanged(const QModelIndex &a, const QModelIndex &b){
    beginRemoveRows(QModelIndex(), 0, rowCount()-1);
    m_mbox_list.clear();
    endRemoveRows();
    for(int i=0;i<m_mbox->rowCount();i++){
        QString name = m_mbox->data(m_mbox->index(i,0), Imap::Mailbox::RoleMailboxName).toString();
        bool hasChildren = m_mbox->data(m_mbox->index(i,0), Imap::Mailbox::RoleMailboxHasChildMailboxes).toBool();

        TrojitaMailBox tmb(name, hasChildren);
        addTrojitaMailBox(tmb);
        if(hasChildren)
            addChildren(m_mbox->index(i,0));
    }
    emit dataChanged(this->index(0), this->index(m_mbox_list.size()-1));
}
QHash<int, QByteArray> TrojitaMailBoxModel::roleNames() const{
    qDebug() << "roleNames()" ;
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[HasChildrenRole] = "hasChildren";
    roles[LevelRole] = "level";
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

        TrojitaMailBox tmb(name, hasChildren);
        addTrojitaMailBox(tmb);
//        qDebug() << "hasChildren" << hasChildren;
        if(hasChildren){
            addChildren(m_mbox->index(i,0, parent));
        }
    }
}

void TrojitaMailBoxModel::setMailBoxModel(Imap::Mailbox::MailboxModel *mbox){
    m_mbox=mbox;
    connect(mbox, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(mailBoxDataChanged(const QModelIndex &, const QModelIndex &)));
}
