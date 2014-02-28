#include "alert.h"
bool TrojitaAlert::hasAlert(){
    Q_FOREACH(QString key, m_alertAccountHash.keys()){
        if(m_alertAccountHash[key] == true)
            return true;
    }
    return false;
}

QString TrojitaAlert::errorMsg(){
    return m_errorMsg;
}

void TrojitaAlert::setErrorMsg(QString m){
    m_errorMsg = m;
    emit errorMsgChanged();
}

QString TrojitaAlert::errorAccount(){
    return m_errorAccount;
}

void TrojitaAlert::setErrorAccount(QString s){
    m_errorAccount = s;
    emit errorAccountChanged();
}

bool TrojitaAlert::getAlertAccount(QString accountName){
    return m_alertAccountHash[accountName];
}
void TrojitaAlert::setAlertAccount(QString accountName, bool b){
    m_alertAccountHash[accountName] = b;
}

void TrojitaAlert::setHasAlert(bool b){
    m_hasAlert = b;
    hasAlertChanged();
}

void TrojitaAlert::slotSetAlert(QString account, int unreadCount){
    m_alertAccountHash[account] = (unreadCount != 0);
    hasAlertChanged();
}

void TrojitaAlert::slotAuthenticateFailed(QString message, QString account){
    setErrorMsg(message);
    setErrorAccount(account);
    emit authenticateFailed();
}
