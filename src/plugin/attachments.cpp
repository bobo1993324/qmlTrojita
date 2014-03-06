#include "attachments.h"
TrojitaAttachment::TrojitaAttachment(QString fileName, int size)
    : m_filename(fileName), m_size(size){
    }
QString TrojitaAttachment::fileName() const{
    return m_filename;
}
int TrojitaAttachment::sizeRead() const{
    return m_size;
}
//QHash<int, QByteArray> TrojitaAttachmentsModel::roleNames() const{
//    QHash<int, QByteArray> roles;
//    roles[FileNameRole] = "fileName";
//    return roles;
//}
//int TrojitaAttachmentsModel::rowCount(const QModelIndex & parent) const{
//    return m_attach_list.count();
//}
//QVariant TrojitaAttachmentsModel::data(const QModelIndex & index, int role) const{
//    if (index.row() < 0 || index.row() > m_attach_list.count())
//        return QVariant();
//    const TrojitaAttachment &ta = m_attach_list[index.row()];
//    if(role == FileNameRole){
//        return ta.fileName();
//    }
//    return QVariant();
//}
//void TrojitaAttachmentsModel::add(TrojitaAttachment ta){
//    beginInsertRows(QModelIndex(), rowCount(), rowCount());
//    m_attach_list.append(ta);
//    endInsertRows();
//}
//void TrojitaAttachmentsModel::clear(){
//    beginRemoveRows(QModelIndex(), 0, rowCount()-1);
//    m_attach_list.clear();
//    endRemoveRows();
//}
//TrojitaAttachmentsModel::TrojitaAttachmentsModel(){};
//
