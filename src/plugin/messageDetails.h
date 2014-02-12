#include <QObject>
#include <QStringList>
#include <QPalette>
#include <QFont>
#include <QFontInfo>
#include <QFile>
#include <QFileInfo>
#include "Imap/Model/Model.h"
#include "Imap/Model/MailboxTree.h"
#include "Imap/Encoders.h"
#include "Common/Paths.h"
#include "Composer/PlainTextFormatter.h"
#include "attachments.h"
class TrojitaMessageDetails : public QObject{
    Q_OBJECT

    enum { ExpensiveFetchThreshold = 50*1024 };

    Q_PROPERTY(QString content READ content WRITE setContent NOTIFY contentChanged)
public:
    TrojitaMessageDetails(QString content = "", TrojitaAttachmentsModel * tam = 0);

    QString content();
    void setContent(QString content);
    void setMessage(const QModelIndex &index);
signals:
    void contentChanged();
public slots:
    void simplePartFetched();
private:
    void partFactoryCreate(const QModelIndex &partIndex, int recursionDepth);
    void markupPlainText();
    void fetchSimpleContent(QModelIndex anotherPart, const Imap::Mailbox::Model * constModel);
    QColor tintColor(const QColor &color, const QColor &tintColor);
    QModelIndex m_partIndex;
    QString m_content;
    Imap::Mailbox::Model *model;
    TrojitaAttachmentsModel * m_tam;
};
