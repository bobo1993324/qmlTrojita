#include <QObject>
#include <Imap/Model/Model.h>
class TrojitaNetwork: public QObject{
    Q_OBJECT
    Q_PROPERTY(bool isOnline READ isOnline NOTIFY isOnlineChanged)
public:
    bool isOnline();
    TrojitaNetwork();
    void setModel(Imap::Mailbox::Model * model);
public slots:

    void networkOffline();
    void networkOnline();

signals:
    void isOnlineChanged();
private:
    bool m_isOnline;
    Imap::Mailbox::Model * m_model;
};
