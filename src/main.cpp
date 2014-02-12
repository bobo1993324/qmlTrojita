#include "ubuntu-emailclient-app.h"
int main(int argc, char *argv[])
{
    Common::registerMetaTypes();
    QSettings settings("trojita", "email-app");
    EmailApplication app(argc, argv, &settings);
    return app.exec();
}

