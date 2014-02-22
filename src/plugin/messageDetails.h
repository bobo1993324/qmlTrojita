#include <QObject>
#include <QStringList>
#include <QPalette>
#include <QFont>
#include <QFontInfo>
#include <QFile>
#include <QFileInfo>
#include "Imap/Model/Model.h"
#include "Imap/Model/MailboxTree.h"
#include "Imap/Model/ItemRoles.h"
#include "Imap/Model/Utils.h"
#include "Imap/Encoders.h"
#include "Common/Paths.h"
#include "Composer/PlainTextFormatter.h"
#include "attachments.h"
class TrojitaMessageDetails : public QObject{
    Q_OBJECT

    enum { ExpensiveFetchThreshold = 50*1024 };

    Q_PROPERTY(QString content READ content WRITE setContent NOTIFY contentChanged)
    Q_PROPERTY(QString subject READ subject NOTIFY subjectChanged)
    Q_PROPERTY(QString from READ from WRITE setFrom NOTIFY fromChanged)
    Q_PROPERTY(QString date READ date WRITE setDate NOTIFY dateChanged)
public:
    TrojitaMessageDetails(QString content = "", TrojitaAttachmentsModel * tam = 0);

    Q_INVOKABLE void deleteMessage();
    Q_INVOKABLE void markUnreadMessage();

    QString content();
    void setContent(QString content);
    QString subject();
    void setSubject(QString subject);
    QString from();
    void setFrom(QString from);
    QString date();
    void setDate(QString date);
    void setMessage(const QModelIndex &index);
signals:
    void contentChanged();
    void subjectChanged();
    void fromChanged();
    void dateChanged();
public slots:
    void simplePartFetched();
private:
    void partFactoryCreate(const QModelIndex &partIndex, int recursionDepth);
    void markupPlainText();
    void fetchSimpleContent(QModelIndex anotherPart, const Imap::Mailbox::Model * constModel);
    QColor tintColor(const QColor &color, const QColor &tintColor);
    QModelIndex m_partIndex;
    QModelIndex m_messageIndex;
    QString m_content;
    QString m_subject;
    QString m_from;
    QString m_date;
    Imap::Mailbox::Model *model;
    TrojitaAttachmentsModel * m_tam;
};
