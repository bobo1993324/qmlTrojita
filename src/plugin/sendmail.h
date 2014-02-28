#include <QObject>
#include <QDebug>
#include <QSettings>
#include <QAbstractProxyModel>
#include "Common/SettingsNames.h"
#include "MSA/SMTP.h"
#include "Composer/Submission.h"
#include "Composer/MessageComposer.h"
#include "Composer/Recipients.h"
#include "Imap/Model/Model.h"
class TrojitaSendMail: public QObject{
    Q_OBJECT
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString errorMsg READ errorMsg NOTIFY errorMsgChanged)
public:
    Q_INVOKABLE int sendMail(QString to, QString subject, QString content, QString cc, QString bcc);
    Q_INVOKABLE bool isAddrCorrect(QString addrs);

    TrojitaSendMail(){}
    void setModel(Imap::Mailbox::Model * model);
    void setSettings(QHash<QString, QVariant> * settings);

    QString status();
    void setStatus(QString);
    QString errorMsg();
    void setErrorMsg(QString);

public slots:
    void sent();
    void gotError(QString);
    void passwordRequested(QString,QString);
signals:
    void statusChanged();
    void errorMsgChanged();
private:
    QHash<QString, QVariant> * m_settings;
    Imap::Mailbox::Model * m_model;
    Composer::Submission * m_submission;
    bool buildMessageData(QString to, QString subject, QString content, QString cc, QString bcc);

    QString m_status;
    QString m_errorMsg;
};
