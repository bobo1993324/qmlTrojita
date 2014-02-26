#include "alert.h"
bool TrojitaAlert::hasAlert(){
    Q_FOREACH(QString key, m_alertAccountHash.keys()){
        if(m_alertAccountHash[key] == true)
            return true;
    }
    return false;
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
//    qDebug() << "slotSetAlert" << account << " " << unreadCount;
    m_alertAccountHash[account] = (unreadCount != 0);
    hasAlertChanged();
}
