#include <QObject>
#ifndef MAIL_ADDRESS
#define MAIL_ADDRESS
class MailAddress : public QObject{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setname NOTIFY nameChanged)
    Q_PROPERTY(QString address READ address WRITE setaddress NOTIFY addressChanged)
public:
    MailAddress(){};
    MailAddress(QString name, QString address);

    QString name();
    void setname(QString);
    QString address();
    void setaddress(QString);
signals:
    void nameChanged();
    void addressChanged();
private:
    QString m_name;
    QString m_address;
};
#endif
