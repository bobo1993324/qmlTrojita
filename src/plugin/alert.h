#include <QObject>
#include <QHash>
#include <QDebug>
class TrojitaAlert : public QObject{
    Q_OBJECT
    Q_PROPERTY(bool hasAlert READ hasAlert NOTIFY hasAlertChanged)
    Q_PROPERTY(QString errorMsg READ errorMsg NOTIFY errorMsgChanged)
    Q_PROPERTY(QString errorAccount READ errorAccount NOTIFY errorAccountChanged)
public:

    //TODO move the following three to Q_PROPERTY
    Q_INVOKABLE bool getAlertAccount(QString accountName);
    Q_INVOKABLE void setAlertAccount(QString accountName, bool b);
    Q_INVOKABLE void setHasAlert(bool b);

    bool hasAlert();
    QString errorMsg();
    void setErrorMsg(QString);
    QString errorAccount();
    void setErrorAccount(QString);
signals:
    void hasAlertChanged();
    void authenticateFailed();
    void errorMsgChanged();
    void errorAccountChanged();
public slots:
    void slotSetAlert(QString account, int unreadCount);
    void slotAuthenticateFailed(QString message, QString account);
private:
    QHash<QString, bool> m_alertAccountHash;
    bool m_hasAlert;
    QString m_errorMsg;
    QString m_errorAccount;
};
