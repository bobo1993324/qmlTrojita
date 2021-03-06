#include "sendmail.h"
void TrojitaSendMail::prepare(){
    using namespace Common;
    QString method = m_settings->value(SettingsNames::msaMethodKey).toString();
    MSA::MSAFactory *msaFactory = 0;
    if (method == SettingsNames::methodSMTP || method == SettingsNames::methodSSMTP) {
        msaFactory = new MSA::SMTPFactory(m_settings->value(SettingsNames::smtpHostKey).toString(),
                                          m_settings->value(SettingsNames::smtpPortKey).toInt(),
                                          (method == SettingsNames::methodSSMTP),
                                          (method == SettingsNames::methodSMTP)
                                          && m_settings->value(SettingsNames::smtpStartTlsKey).toBool(),
                                          m_settings->value(SettingsNames::smtpAuthKey).toBool(),
                                          m_settings->value(SettingsNames::smtpUserKey).toString(),
                                          m_settings->value(SettingsNames::smtpPassKey).toString());
        m_submission = new Composer::Submission(this, m_model, msaFactory);
        connect(m_submission, SIGNAL(succeeded()), this, SLOT(sent()));
        connect(m_submission, SIGNAL(failed(QString)), this, SLOT(gotError(QString)));
        connect(m_submission, SIGNAL(passwordRequested(QString,QString)), this, SLOT(passwordRequested(QString,QString)), Qt::QueuedConnection);
    } else if (method == SettingsNames::methodSENDMAIL) {
        //        QStringList args = m_settings->value(SettingsNames::sendmailKey, SettingsNames::sendmailDefaultCmd).toString().split(QLatin1Char(' '));
        //        if (args.isEmpty()) {
        //            QMessageBox::critical(this, tr("Error"), tr("Please configure the SMTP or sendmail settings in application settings."));
        //            return 0;
        //        }
        //        QString appName = args.takeFirst();
        //        msaFactory = new MSA::SendmailFactory(appName, args);
    } else if (method == SettingsNames::methodImapSendmail) {
        //        if (!imapModel()->capabilities().contains(QLatin1String(""))) {
        //            QMessageBox::critical(this, tr("Error"), tr("The IMAP server does not support mail submission. Please reconfigure the application."));
        //            return 0;
        //        }
        // no particular preparation needed here
    } else {
        //        QMessageBox::critical(this, tr("Error"), tr("Please configure e-mail delivery method in application settings."));
        //        return 0;
    }
}

bool TrojitaSendMail::addAttach(QString file){
    if(file.indexOf("file://") != -1){
        file.remove(0, 7);
    }
    return m_submission->composer()->addFileAttachment(file);
}

void TrojitaSendMail::removeAttachAtIdx(int idx){
    m_submission->composer()->removeAttachment(m_submission->composer()->index(idx));
}

int TrojitaSendMail::sendMail(QString to, QString subject, QString content, QString cc, QString bcc){
    qDebug() << "sendMail to " << to <<" " << content;
    setStatus("sending");

    if (!buildMessageData(to, subject, content, cc, bcc))
        return 0;
    m_submission->send();


}

bool TrojitaSendMail::isAddrCorrect(QString addrs){
    QStringList tos = addrs.split(",");
    //parse to
    Q_FOREACH(QString to2, tos){
        Imap::Message::MailAddress addr;
        bool ok = Imap::Message::MailAddress::fromPrettyString(addr, to2);
        if(!ok){
            setErrorMsg(to2 + "is not a valid email address");
            return false;
        }
    }
    return true;
}

void TrojitaSendMail::sent(){
    setStatus("sent");
}

void TrojitaSendMail::gotError(QString message){
    setErrorMsg(message);
}

bool TrojitaSendMail::buildMessageData(QString to, QString subject, QString content, QString cc, QString bcc)
{
    QList<QPair<Composer::RecipientKind,Imap::Message::MailAddress> > recipients;

    QStringList tos = to.split(",");
    //parse to
    Q_FOREACH(QString to2, tos){
        Imap::Message::MailAddress addr;
        bool ok = Imap::Message::MailAddress::fromPrettyString(addr, to2);
        Q_ASSERT(ok);
        qDebug() << "to" << to2;
        recipients << qMakePair(Composer::ADDRESS_TO, addr);
    }
    //parse cc
    if(cc != ""){
        QStringList ccs = cc.split(",");
        Q_FOREACH(QString cc2, ccs){
            Imap::Message::MailAddress addr;
            bool ok = Imap::Message::MailAddress::fromPrettyString(addr, cc2);
            qDebug() << "cc" << cc2;
            Q_ASSERT(ok);
            recipients << qMakePair(Composer::ADDRESS_CC, addr);
        }
    }
    //parse bcc
    if(bcc != ""){
        QStringList bccs = bcc.split(",");
        Q_FOREACH(QString bcc2, bccs){
            Imap::Message::MailAddress addr;
            bool ok = Imap::Message::MailAddress::fromPrettyString(addr, bcc2);
            qDebug() << "bcc" << bcc2;
            Q_ASSERT(ok);
            recipients << qMakePair(Composer::ADDRESS_BCC, addr);
        }
    }
    //    if (!parseRecipients(recipients, errorMessage)) {
    //        gotError(tr("Cannot parse recipients:\n%1").arg(errorMessage));
    //        return false;
    //    }
    //    if (recipients.isEmpty()) {
    //        gotError(tr("You haven't entered any recipients"));
    //        return false;
    //    }
    m_submission->composer()->setRecipients(recipients);
    //

    Imap::Message::MailAddress fromAddress;
    if (!Imap::Message::MailAddress::fromPrettyString(fromAddress, m_settings->value(Common::SettingsNames::smtpUserKey).toString())) {
        //        gotError(tr("The From: address does not look like a valid one"));
        return false;
    }
    //    if (ui->subject->text().isEmpty()) {
    //        gotError(tr("You haven't entered any subject. Cannot send such a mail, sorry."));
    //        ui->subject->setFocus();
    //        return false;
    //    }
    m_submission->composer()->setFrom(fromAddress);
    m_submission->composer()->setTimestamp(QDateTime::currentDateTime());
    m_submission->composer()->setSubject(subject);

    //    QAbstractProxyModel *proxy = qobject_cast<QAbstractProxyModel*>(ui->sender->model());
    //    Q_ASSERT(proxy);

    //    if (ui->sender->findText(ui->sender->currentText()) != -1) {
    //        QModelIndex proxyIndex = ui->sender->model()->index(ui->sender->currentIndex(), 0, ui->sender->rootModelIndex());
    //        Q_ASSERT(proxyIndex.isValid());
    //        m_submission->composer()->setOrganization(
    //                    proxy->mapToSource(proxyIndex).sibling(proxyIndex.row(), Composer::SenderIdentitiesModel::COLUMN_ORGANIZATION)
    //                    .data().toString());
    //    }
    m_submission->composer()->setText(content);

    //    if (m_actionInReplyTo->isChecked()) {
    //        m_submission->composer()->setInReplyTo(m_inReplyTo);
    //        m_submission->composer()->setReferences(m_references);
    //        m_submission->composer()->setReplyingToMessage(m_replyingToMessage);
    //    } else {
    m_submission->composer()->setInReplyTo(QList<QByteArray>());
    m_submission->composer()->setReferences(QList<QByteArray>());
    m_submission->composer()->setReplyingToMessage(QModelIndex());
    //    }

    return m_submission->composer()->isReadyForSerialization();
}

void TrojitaSendMail::setModel(Imap::Mailbox::Model *model){
    m_model=model;
}

void TrojitaSendMail::setSettings(QHash<QString, QVariant> * settings){
    m_settings = settings;
}

QString TrojitaSendMail::status(){
    return m_status;
}
void TrojitaSendMail::setStatus(QString s){
    m_status = s;
    emit statusChanged();
}

QString TrojitaSendMail::errorMsg(){
    return m_errorMsg;
}
void TrojitaSendMail::setErrorMsg(QString s){
    setStatus("error");
    m_errorMsg = s;
    emit errorMsgChanged();
}

void TrojitaSendMail::passwordRequested(QString user, QString host){
    //todo implement password resquest
    qDebug() << "TrojitaSendMail::passwordRequested";
}
