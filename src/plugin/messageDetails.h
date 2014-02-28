#include <QObject>
#include <QStringList>
#include <QPalette>
#include <QFont>
#include <QFontInfo>
#include <QFile>
#include <QFileInfo>
#include <QTextDocument>
#include "Imap/Model/Model.h"
#include "Imap/Model/MailboxTree.h"
#include "Imap/Model/ItemRoles.h"
#include "Imap/Model/Utils.h"
#include "Imap/Encoders.h"
#include "Imap/Network/MsgPartNetAccessManager.h"
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
    Q_PROPERTY(QStringList to READ to NOTIFY toChanged)
    Q_PROPERTY(int toCount READ toCount NOTIFY toChanged)
    Q_PROPERTY(QStringList cc READ cc NOTIFY ccChanged)
    Q_PROPERTY(int ccCount READ ccCount NOTIFY ccChanged)
    Q_PROPERTY(QStringList bcc READ bcc  NOTIFY bccChanged)
    Q_PROPERTY(int bccCount READ bccCount NOTIFY bccChanged)
public:
    TrojitaMessageDetails(QString content = "", TrojitaAttachmentsModel * tam = 0);

    Q_INVOKABLE void deleteMessage();
    Q_INVOKABLE void markUnreadMessage();
    Q_INVOKABLE QString generateReplyMessage();

    QString content();
    void setContent(QString content);
    QString subject();
    void setSubject(QString subject);
    QString from();
    void setFrom(QString from);
    QString date();
    void setDate(QString date);
    QStringList to();
    void setTo(QStringList to);
    int toCount();
    QStringList cc();
    void setCc(QStringList cc);
    int ccCount();
    QStringList bcc();
    void setBcc(QStringList cc);
    int bccCount();

    void setMessage(const QModelIndex &index);
signals:
    void contentChanged();
    void subjectChanged();
    void fromChanged();
    void dateChanged();
    void toChanged();
    void ccChanged();
    void bccChanged();
public slots:
    void simplePartFetched();
private:
    void partFactoryCreate(const QModelIndex &partIndex, int recursionDepth);
    void markupPlainText();
    void fetchSimpleContent(QModelIndex anotherPart, const Imap::Mailbox::Model * constModel);
    void fetchGenericMultipart(QModelIndex anotherPart, const Imap::Mailbox::Model * constModel);

    QString getFirstMailFromList(QVariantList qvl);
    QStringList getMailsFromList(QVariantList qvl);
    QColor tintColor(const QColor &color, const QColor &tintColor);
    QModelIndex m_partIndex;
    QModelIndex m_messageIndex;
    QString m_content;
    QString m_subject;
    QString m_from;
    QString m_date;
    QStringList m_to;
    QStringList m_cc;
    QStringList m_bcc;
    Imap::Mailbox::Model *model;
    TrojitaAttachmentsModel * m_tam;
};
