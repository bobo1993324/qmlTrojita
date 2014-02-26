#include <QObject>
#include <QHash>
#include <QDebug>
class TrojitaAlert : public QObject{
    Q_OBJECT
    Q_PROPERTY(bool hasAlert READ hasAlert NOTIFY hasAlertChanged)
public:

    Q_INVOKABLE bool getAlertAccount(QString accountName);
    Q_INVOKABLE void setAlertAccount(QString accountName, bool b);
    Q_INVOKABLE void setHasAlert(bool b);

    bool hasAlert();
signals:
    void hasAlertChanged();

public slots:
    void slotSetAlert(QString account, int unreadCount);
private:
    QHash<QString, bool> m_alertAccountHash;
    bool m_hasAlert;
};
