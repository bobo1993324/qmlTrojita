#include "settings.h"
TrojitaSetting::TrojitaSetting(QSettings * setting)
    : m_setting(setting){
    //TODO Provide default value
}

void TrojitaSetting::setUserName(QString name){
    m_setting->setValue(Common::SettingsNames::imapUserKey, name);
}

QString TrojitaSetting::getUserName(){
    return m_setting->value(Common::SettingsNames::imapUserKey).toString();
}

void TrojitaSetting::setPassword(QString password){
    m_setting->setValue(Common::SettingsNames::imapPassKey, password);
}

