#include "messages.h"
QString TrojitaMessage::subject() const{
    return m_subject;
}
uint TrojitaMessage::uid() const{
    return m_uid;
}
QString TrojitaMessage::sender() const{
    return m_sender;
}
bool TrojitaMessage::unread() const{
    return m_unread;
}
QString TrojitaMessage::time() const{
    if(QDate::currentDate() == m_qdt.date()){
        return m_qdt.time().toString();
    }else{
        return m_qdt.date().toString(Qt::ISODate);
    }
}
bool TrojitaMessage::isStared() const{
    return m_isStared;
}

TrojitaMessagesModel::TrojitaMessagesModel(Imap::Mailbox::MsgListModel *msgList)
{
    setMsgListModel(msgList);
}

void TrojitaMessagesModel::setMsgListModel(Imap::Mailbox::MsgListModel *msgListModel){
    m_msgListModel = msgListModel;
    disconnect(this, SLOT(msgModelDataChanged(QModelIndex,QModelIndex)));
    disconnect(this, SLOT(msgModelMailBoxChanged(QModelIndex)));
    connect(m_msgListModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(msgModelDataChanged(QModelIndex,QModelIndex)));
    connect(m_msgListModel, SIGNAL(mailboxChanged(QModelIndex)), this, SLOT(msgModelMailBoxChanged(QModelIndex)));
    connect(m_msgListModel, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(reloadMessages()));
    reloadMessages();
}
QHash<int, QByteArray> TrojitaMessagesModel::roleNames() const{
    QHash<int, QByteArray> roles;
    roles[SubjectRole] = "subject";
    roles[UidRole] = "uid";
    roles[SenderRole] = "sender";
    roles[UnreadRole] = "unread";
    roles[TimeRole] = "time";
    roles[StarRole] = "star";
    return roles;
}
int TrojitaMessagesModel::rowCount(const QModelIndex & parent) const{
    return m_msg_list.count();
}
QVariant TrojitaMessagesModel::data(const QModelIndex &index, int role) const {
//    qDebug() << "subject is " << m_msg_list[index.row()].subject();
    if (index.row() < 0 || index.row() > m_msg_list.count())
        return QVariant();
    const TrojitaMessage &tmb = m_msg_list[index.row()];
//    qDebug() << " data " << index.row() << (role==NameRole) << tmb.name();
    if (role == SubjectRole)
        return tmb.subject();
    if (role == UidRole)
        return tmb.uid();
    if (role == SenderRole)
        return tmb.sender();
    if (role == UnreadRole)
        return tmb.unread();
    if (role == TimeRole)
        return tmb.time();
    if (role == StarRole)
        return tmb.isStared();
    return QVariant();
}
void TrojitaMessagesModel::addTrojitaMessage(TrojitaMessage tm){
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_msg_list.append(tm);
    endInsertRows();
}

void TrojitaMessagesModel::msgModelDataChanged(QModelIndex a,QModelIndex b){
//    qDebug() << "TrojitaMessagesModel::msgModelDataChanged" ;
    reloadMessages();
}

void TrojitaMessagesModel::msgModelMailBoxChanged(QModelIndex q){
//    qDebug() << "TrojitaMessagesModel::msgModelMailBoxChanged" ;
    reloadMessages();
}

void TrojitaMessagesModel::msgModelRowInserted(QModelIndex, int, int){
    reloadMessages();
}

void TrojitaMessagesModel::reloadMessages(){
    beginRemoveRows(QModelIndex(), 0, rowCount()-1);
    m_msg_list.clear();
    endRemoveRows();
//    qDebug() << "rowCount is " << m_msgListModel->rowCount();
    for(int i=0;i<m_msgListModel->rowCount();i++){
        bool isDeleted=m_msgListModel->data(m_msgListModel->index(i,0), Imap::Mailbox::RoleMessageIsMarkedDeleted).toBool();
        if(isDeleted)
            continue;
        QString subject = m_msgListModel->data(m_msgListModel->index(i,0), Imap::Mailbox::RoleMessageSubject).toString();
        uint uid = m_msgListModel->data(m_msgListModel->index(i,0), Imap::Mailbox::RoleMessageUid).toUInt();
        QVariantList senderInfoList = m_msgListModel->data(m_msgListModel->index(i,0), Imap::Mailbox::RoleMessageSender).toList();
        QString sender = "";
        if(!senderInfoList.empty()){
            QVariantList oneSender = senderInfoList[0].toList();
            if(oneSender[0].toString()!="")
                sender=oneSender[0].toString();
            else
                sender = oneSender[2].toString() + "@" + oneSender[3].toString();
        }
        bool unread = !m_msgListModel->data(m_msgListModel->index(i,0), Imap::Mailbox::RoleMessageIsMarkedRead).toBool();
        QDateTime qdt = m_msgListModel->data(m_msgListModel->index(i,0), Imap::Mailbox::RoleMessageDate).toDateTime();
        bool isStared=false;
        QStringList flags=  m_msgListModel->data(m_msgListModel->index(i,0), Imap::Mailbox::RoleMessageFlags).toStringList();
        for(int i=0;i<flags.size();i++){
            if(flags[i]=="\\Flagged") // gmail
                isStared=true;
        }
        TrojitaMessage tmb(subject, uid, sender, unread, qdt, isStared);
        addTrojitaMessage(tmb);
//        qDebug() << "add subject " << subject;
    }
    emit dataChanged(this->index(0), this->index(m_msg_list.count()-1));
}

QString TrojitaMessagesModel::getMailBoxName(){
    return m_mailBoxName;
}

void TrojitaMessagesModel::setMailBoxName(QString mbn){
    m_mailBoxName = mbn;
    mailBoxNameChanged();
}
