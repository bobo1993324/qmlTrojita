add_library(
    email-plugin
    settings.cpp
    mailBox.cpp
    messages.cpp
    messageDetails.cpp
    alert.cpp
    sendmail.cpp
)
qt5_use_modules(email-plugin Core Quick)
