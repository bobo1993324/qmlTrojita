#include "mailAddress.h"
MailAddress::MailAddress(QString name, QString address)
    : m_name(name), m_address(address){

}

QString MailAddress::name(){
    return m_name;
}

void MailAddress::setname(QString name){
    m_name = name;
    emit nameChanged();
}

QString MailAddress::address(){
    return m_address;
}

void MailAddress::setaddress(QString addr){
    m_address = addr;
    emit addressChanged();
}
