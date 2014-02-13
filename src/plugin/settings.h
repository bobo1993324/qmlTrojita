#include <QSettings>
#include <QObject>
#include <QDebug>
#include "../trojita/src/Common/SettingsNames.h"
class TrojitaSetting: public QObject{
    Q_OBJECT
public:
    TrojitaSetting(){}
    TrojitaSetting(QSettings * setting);
    Q_INVOKABLE void updateSetting(QString key, QString value);
private:
    QSettings * m_setting;
};
