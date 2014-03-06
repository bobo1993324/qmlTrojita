#include "network.h"
bool TrojitaNetwork::isOnline(){
    return m_isOnline;
}
TrojitaNetwork::TrojitaNetwork()
    : m_model(0){

}
void TrojitaNetwork::setModel(Imap::Mailbox::Model *model){
    if(m_model){
        disconnect(this, SLOT(networkOffline()));
        disconnect(this, SLOT(networkOnline()));
    }
    m_model = model;
    connect(model, SIGNAL(networkPolicyOnline()), this, SLOT(networkOnline()));
    connect(model, SIGNAL(networkPolicyOffline()), this, SLOT(networkOffline()));
}
void TrojitaNetwork::networkOffline(){
    m_isOnline = false;
    emit isOnlineChanged();
}
void TrojitaNetwork::networkOnline(){
    m_isOnline = true;
    emit isOnlineChanged();
}
