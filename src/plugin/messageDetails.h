#include <QObject>
#include <QStringList>
#include <QPalette>
#include <QFont>
#include <QFontInfo>
#include <QFile>
#include <QFileInfo>
#include <QTextDocument>
#include <QQmlListProperty>
#include "Imap/Model/Model.h"
#include "Imap/Model/MailboxTree.h"
#include "Imap/Model/ItemRoles.h"
#include "Imap/Model/Utils.h"
#include "Imap/Encoders.h"
#include "Imap/Network/MsgPartNetAccessManager.h"
#include "Common/Paths.h"
#include "Composer/PlainTextFormatter.h"
#include "attachments.h"
#include "mailAddress.h"
class TrojitaMessageDetails : public QObject{
    Q_OBJECT

    enum { ExpensiveFetchThreshold = 50*1024 };

    Q_PROPERTY(QString content READ content WRITE setContent NOTIFY contentChanged)
    Q_PROPERTY(QString subject READ subject NOTIFY subjectChanged)
    Q_PROPERTY(MailAddress* from READ from NOTIFY fromChanged)
    Q_PROPERTY(QString date READ date WRITE setDate NOTIFY dateChanged)
    Q_PROPERTY(QQmlListProperty<MailAddress> to READ to NOTIFY toChanged)
    Q_PROPERTY(QQmlListProperty<MailAddress> cc READ cc NOTIFY ccChanged)
    Q_PROPERTY(QQmlListProperty<MailAddress> bcc READ bcc NOTIFY bccChanged)
public:
    TrojitaMessageDetails(QString content = "", TrojitaAttachmentsModel * tam = 0);

    Q_INVOKABLE void deleteMessage();
    Q_INVOKABLE void markUnreadMessage();
    Q_INVOKABLE QString generateReplyMessage();

    QString content();
    void setContent(QString content);
    QString subject();
    void setSubject(QString subject);
    MailAddress* from();
    void setFrom(QVariantList from);
    QString date();
    void setDate(QString date);
    QQmlListProperty<MailAddress> to();
    void setTo(QVariantList);
    QQmlListProperty<MailAddress> cc();
    void setCc(QVariantList cc);
     QQmlListProperty<MailAddress> bcc();
    void setBcc(QVariantList bcc);

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

    QColor tintColor(const QColor &color, const QColor &tintColor);
    QModelIndex m_partIndex;
    QModelIndex m_messageIndex;
    QString m_content;
    QString m_subject;
    MailAddress* m_from;
    QString m_date;
    QList<MailAddress *> m_to;
    QList<MailAddress*> m_cc;
    QList<MailAddress*> m_bcc;
    Imap::Mailbox::Model *model;
    TrojitaAttachmentsModel * m_tam;
};
