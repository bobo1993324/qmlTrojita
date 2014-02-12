#include <QObject>
#include <QDebug>
#include <QSettings>
#include <QAbstractProxyModel>
#include "Common/SettingsNames.h"
#include "MSA/SMTP.h"
#include "Composer/Submission.h"
#include "Imap/Model/Model.h"
class TrojitaSendMail: public QObject{
    Q_OBJECT
public:
    Q_INVOKABLE int sendMail(QString to, QString content);
    TrojitaSendMail(QSettings * settings, Imap::Mailbox::Model * model);
public slots:
    void sent();
private:
    QSettings * m_settings;
    Imap::Mailbox::Model * m_model;
    Composer::Submission * m_submission;
    bool buildMessageData();
};
