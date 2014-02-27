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
public:
    Q_INVOKABLE int sendMail(QString to, QString subject, QString content);

    TrojitaSendMail(){}
    void setModel(Imap::Mailbox::Model * model);
    void setSettings(QHash<QString, QVariant> * settings);
public slots:
    void sent();
private:
    QHash<QString, QVariant> * m_settings;
    Imap::Mailbox::Model * m_model;
    Composer::Submission * m_submission;
    bool buildMessageData(QString to, QString subject = "", QString content = "");
};
