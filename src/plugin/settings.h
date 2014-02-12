#include <QSettings>
#include <QObject>
#include "../trojita/src/Common/SettingsNames.h"
class TrojitaSetting: public QObject{
    Q_OBJECT
    Q_PROPERTY(QString userName READ getUserName WRITE setUserName)
public:
    TrojitaSetting(){}
    TrojitaSetting(QSettings * setting);
    Q_INVOKABLE void setUserName(QString name);
    Q_INVOKABLE QString getUserName();
    Q_INVOKABLE void setPassword(QString password);
private:
    QSettings * m_setting;
};
