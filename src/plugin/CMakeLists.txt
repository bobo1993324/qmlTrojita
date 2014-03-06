add_library(
    email-plugin
    settings.cpp
    mailBox.cpp
    messages.cpp
    messageDetails.cpp
    alert.cpp
    sendmail.cpp
    attachments.cpp
    mailAddress.cpp
)
qt5_use_modules(email-plugin Core Quick)
