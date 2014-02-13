#include "settings.h"
TrojitaSetting::TrojitaSetting(QSettings * setting)
    : m_setting(setting){
    //TODO Provide default value
}

void TrojitaSetting::updateSetting(QString key, QString value){
//    qDebug() << key <<" " << value;
//    return;
    m_setting->setValue(key, value);
}
