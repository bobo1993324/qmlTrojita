#ifndef TROJITA_ATTACHMENTS
#define TROJITA_ATTACHMENTS

#include <QObject>
#include <QAbstractListModel>
#include <QDebug>
class TrojitaAttachment
{
public:
    TrojitaAttachment(QString fileName);
    QString fileName() const;
private:
    QString m_fileName;
};
class TrojitaAttachmentsModel : public QAbstractListModel{
    Q_OBJECT
public:
    enum TrojitaAttachmentsModelRoles{
        FileNameRole = Qt::UserRole+1
    };
    TrojitaAttachmentsModel();
    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    void add(TrojitaAttachment ta);
    void clear();
private:
    QList<TrojitaAttachment> m_attach_list;
};
#endif
