#include "messages.h"
QString TrojitaMessage::title() const{
    return m_title;
}
uint TrojitaMessage::uid() const{
    return m_uid;
}
TrojitaMessagesModel::TrojitaMessagesModel(Imap::Mailbox::MsgListModel *msgList)
{
    setMsgListModel(msgList);
}

void TrojitaMessagesModel::setMsgListModel(Imap::Mailbox::MsgListModel *msgListModel){
    m_msgListModel = msgListModel;
    connect(m_msgListModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(msgModelDataChanged(QModelIndex,QModelIndex)));
}
QHash<int, QByteArray> TrojitaMessagesModel::roleNames() const{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[UidRole] = "uid";
    return roles;
}
int TrojitaMessagesModel::rowCount(const QModelIndex & parent) const{
    return m_msg_list.count();
}
QVariant TrojitaMessagesModel::data(const QModelIndex &index, int role) const {
    if (index.row() < 0 || index.row() > m_msg_list.count())
        return QVariant();

    const TrojitaMessage &tmb = m_msg_list[index.row()];
//    qDebug() << " data " << index.row() << (role==NameRole) << tmb.name();
    if (role == TitleRole)
        return tmb.title();
    if (role == UidRole)
        return tmb.uid();
    return QVariant();
}
void TrojitaMessagesModel::addTrojitaMessage(TrojitaMessage tm){
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_msg_list.append(tm);
    endInsertRows();
}

void TrojitaMessagesModel::msgModelDataChanged(QModelIndex a,QModelIndex b){
//    qDebug() << "msgModelDataChanged" ;
    beginRemoveRows(QModelIndex(), 0, rowCount()-1);
    m_msg_list.clear();
    endRemoveRows();
//    qDebug() << "rowCount is " << m_msgListModel->rowCount();
    for(int i=0;i<m_msgListModel->rowCount();i++){
        QString subject = m_msgListModel->data(m_msgListModel->index(i,0), Imap::Mailbox::RoleMessageSubject).toString();
        uint uid = m_msgListModel->data(m_msgListModel->index(i,0), Imap::Mailbox::RoleMessageUid).toUInt();
//        qDebug() << "subject is " << title;
        TrojitaMessage tmb(subject, uid);
        addTrojitaMessage(tmb);
    }
    emit dataChanged(this->index(0), this->index(m_msg_list.count()-1));
}
