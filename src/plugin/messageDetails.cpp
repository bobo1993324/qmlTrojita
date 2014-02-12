#include "messageDetails.h"
TrojitaMessageDetails::TrojitaMessageDetails(QString content, TrojitaAttachmentsModel * tam)
    : m_content(content), m_tam(tam)
{
}

QString TrojitaMessageDetails::content(){
    return m_content;
}

void TrojitaMessageDetails::setContent(QString content){
    m_content=content;
    contentChanged();
}

void TrojitaMessageDetails::setMessage(const QModelIndex &index){
    //clear attachments
    m_tam->clear();
    // first, let's get a real model
    QModelIndex messageIndex;
    const Imap::Mailbox::Model *constModel = 0;
    Imap::Mailbox::TreeItem *item = Imap::Mailbox::Model::realTreeItem(index, &constModel, &messageIndex);
    Q_ASSERT(item); // Make sure it's a message
    Q_ASSERT(messageIndex.isValid());
    Imap::Mailbox::Model *realModel = const_cast<Imap::Mailbox::Model *>(constModel);
    Q_ASSERT(realModel);

    // The data might be available from the local cache, so let's try to save a possible roundtrip here
    item->fetch(realModel);

    if (!messageIndex.data(Imap::Mailbox::RoleIsFetched).toBool()) {
        qDebug() << "TODO message is not fetched";
        return;
    }

    QModelIndex rootPartIndex = messageIndex.child(0, 0);
    partFactoryCreate(rootPartIndex, 0);
}

void TrojitaMessageDetails::partFactoryCreate(const QModelIndex &partIndex, int recursionDepth){
    using namespace Imap::Mailbox;
    Q_ASSERT(partIndex.isValid());

    if (recursionDepth > 1000) {
        qDebug()<< "recursionDepth > 1000";
        return;
    }

    QString mimeType = partIndex.data(Imap::Mailbox::RolePartMimeType).toString().toLower();
    qDebug()<< "mimetype is" << mimeType;
    bool isMessageRfc822 = mimeType == QLatin1String("message/rfc822");
    bool isCompoundMimeType = mimeType.startsWith(QLatin1String("multipart/")) || isMessageRfc822;

    // Check whether we can render this MIME type at all
    QStringList allowedMimeTypes;
    allowedMimeTypes << "text/html" << "text/plain" << "image/jpeg" <<
                        "image/jpg" << "image/pjpeg" << "image/png" << "image/gif";
    bool recognizedMimeType = isCompoundMimeType || allowedMimeTypes.contains(mimeType);
    bool isDerivedMimeType = false;
    if (!recognizedMimeType) {
        // QMimeType's docs say that one shall use inherit() to check for "is this a recognized MIME type".
        // E.g. text/x-csrc inherits text/plain.
        qDebug() << "minetype not recognized";
        //        QMimeType partType = QMimeDatabase().mimeTypeForName(mimeType);
        //        Q_FOREACH(const QString &candidate, allowedMimeTypes) {
        //            if (partType.isValid() && !partType.isDefault() && partType.inherits(candidate)) {
        //                // Looks like we shall be able to show this
        //                recognizedMimeType = true;
        //                // If it's a derived MIME type, it makes sense to not block inline display, yet still make it possible to hide it
        //                // using the regular attachment controls
        //                isDerivedMimeType = true;
        //                manager->registerMimeTypeTranslation(mimeType, candidate);
        //                break;
        //            }
        //        }
    }

    const Imap::Mailbox::Model *constModel = 0;
    Imap::Mailbox::TreeItemPart *part = dynamic_cast<Imap::Mailbox::TreeItemPart *>(Imap::Mailbox::Model::realTreeItem(partIndex, &constModel));
    model = const_cast<Imap::Mailbox::Model *>(constModel);
    Q_ASSERT(model);
    Q_ASSERT(part);

    // Check whether it shall be wrapped inside an AttachmentView
    // From section 2.8 of RFC 2183: "Unrecognized disposition types should be treated as `attachment'."
    const QByteArray contentDisposition = partIndex.data(Imap::Mailbox::RolePartBodyDisposition).toByteArray().toLower();
    const bool isInline = contentDisposition.isEmpty() || contentDisposition == "inline";
    const bool looksLikeAttachment = !partIndex.data(Imap::Mailbox::RolePartFileName).toString().isEmpty();
    const bool wrapInAttachmentView = (looksLikeAttachment || !isInline || !recognizedMimeType || isDerivedMimeType || isMessageRfc822);
    if (wrapInAttachmentView) {
        // The problem is that some nasty MUAs (hint hint Thunderbird) would
        // happily attach a .tar.gz and call it "inline"
        qDebug() << "wrapInAttachmentView";
        //        QWidget *contentWidget = 0;
        //        if (recognizedMimeType) {
        //            PartLoadingOptions options = loadingMode | PART_IGNORE_DISPOSITION_ATTACHMENT;
        //            if (!isInline) {
        //                // The widget will be hidden by default, i.e. the "inline preview" will be deactivated.
        //                // If the user clicks that action in the AttachmentView, it makes sense to load the plugin without any further ado,
        //                // without requiring an extra clickthrough
        //                options |= PART_IS_HIDDEN;
        //            } else if (!isCompoundMimeType) {
        //                // This is to prevent a clickthrough when the data can be already shown
        //                part->fetchFromCache(model);

        //                // This makes sure that clickthrough only affects big parts during "expensive network" mode
        //                if (model->isNetworkOnline() || part->octets() <= ExpensiveFetchThreshold) {
        //                    options |= PART_IGNORE_CLICKTHROUGH;
        //                }
        //            } else {
        //                // A compound type -> make sure we disable clickthrough
        //                options |= PART_IGNORE_CLICKTHROUGH;
        //            }

        //            if (!model->isNetworkAvailable()) {
        //                // This is to prevent a clickthrough when offline
        //                options |= PART_IGNORE_CLICKTHROUGH;
        //            }
        //            contentWidget = new LoadablePartWidget(0, manager, partIndex, m_messageView, this, recursionDepth + 1, options);
        //            if (!isInline) {
        //                contentWidget->hide();
        //            }
        //        }
        //        // Previously, we would also hide an attachment if the current policy was "expensive network", the part was too big
        //        // and not fetched yet. Arguably, that's a bug -- an item which is marked online shall not be hidden.
        //        // Wrapping via a clickthrough is fine, though; the user is clearly informed that this item *should* be visible,
        //        // yet the bandwidth is not excessively trashed.
        //        return new AttachmentView(0, manager, partIndex, m_messageView, contentWidget);
    }

    // Now we know for sure that it's not supposed to be wrapped in an AttachmentView, cool.
    if (mimeType.startsWith(QLatin1String("multipart/"))) {
        // it's a compound part
        qDebug() << "it's a compound part";
        // First loop iteration is used to find out what MIME type to show
        if (mimeType == QLatin1String("multipart/alternative")) {
            //user plain text
            for (int i = 0; i < partIndex.model()->rowCount(partIndex); ++i) {
                QModelIndex anotherPart = partIndex.child(i, 0);
                Q_ASSERT(anotherPart.isValid());
                QString mimeType2 = anotherPart.data(Imap::Mailbox::RolePartMimeType).toString();
                qDebug() << "subtype " << mimeType2;
                if(mimeType2 == "text/plain"){
                    fetchSimpleContent(anotherPart, constModel);
                }
            }
        } else if (mimeType == QLatin1String("multipart/signed")) {
            //            return new MultipartSignedWidget(0, this, partIndex, recursionDepth, loadingMode);
        } else if (mimeType == QLatin1String("multipart/related")) {
            // The purpose of this section is to find a text/html e-mail, along with its associated body parts, and hide
            // everything else than the HTML widget.

            // At this point, it might be interesting to somehow respect the user's preference about using text/plain
            // instead of text/html. However, things are a bit complicated; the widget used at this point really wants
            // to either show just a single part or alternatively all of them in a sequence.
            // Furthermore, if someone sends a text/plain and a text/html together inside a multipart/related, they're
            // just wrong.

            // Let's see if we know what the root part is
            //            QModelIndex mainPartIndex;
            //            QVariant mainPartCID = partIndex.data(RolePartMultipartRelatedMainCid);
            //            if (mainPartCID.isValid()) {
            //                const Imap::Mailbox::Model *constModel = 0;
            //                Imap::Mailbox::TreeItemPart *part = dynamic_cast<Imap::Mailbox::TreeItemPart *>(Imap::Mailbox::Model::realTreeItem(partIndex, &constModel));
            //                Imap::Mailbox::Model *model = const_cast<Imap::Mailbox::Model *>(constModel);
            //                Imap::Mailbox::TreeItemPart *mainPartPtr = Imap::Network::MsgPartNetAccessManager::cidToPart(mainPartCID.toByteArray(), model, part);
            //                if (mainPartPtr) {
            //                    mainPartIndex = mainPartPtr->toIndex(model);
            //                }
            //            }

            //            if (!mainPartIndex.isValid()) {
            //                // The Content-Type-based start parameter was not terribly useful. Let's find the HTML part manually.
            //                QModelIndex candidate = partIndex.child(0, 0);
            //                while (candidate.isValid()) {
            //                    if (candidate.data(RolePartMimeType).toString() == QLatin1String("text/html")) {
            //                        mainPartIndex = candidate;
            //                        break;
            //                    }
            //                    candidate = candidate.sibling(candidate.row() + 1, 0);
            //                }
            //            }

            //            if (mainPartIndex.isValid()) {
            //                if (mainPartIndex.data(RolePartMimeType).toString() == QLatin1String("text/html")) {
            //                    return PartWidgetFactory::create(mainPartIndex, recursionDepth+1, loadingMode);
            //                } else {
            //                    // Sorry, but anything else than text/html is by definition suspicious here. Better than picking some random
            //                    // choice, let's just show everything.
            //                    return new GenericMultipartWidget(0, this, partIndex, recursionDepth, loadingMode);
            //                }
            //            } else {
            //                // The RFC2387's wording is clear that in absence of an explicit START argument, the first part is the starting one.
            //                // On the other hand, I've seen real-world messages whose first part is some utter garbage (an image sent as
            //                // application/octet-stream, for example) and some *other* part is an HTML text. In that case (and if we somehow
            //                // failed to pick the HTML part by a heuristic), it's better to show everything.
            //                return new GenericMultipartWidget(0, this, partIndex, recursionDepth, loadingMode);
            //            }
        } else {
            //            return new GenericMultipartWidget(0, this, partIndex, recursionDepth, loadingMode);
            bool findPlain = false;
            for (int i = 0; i < partIndex.model()->rowCount(partIndex); ++i) {
                QModelIndex anotherPart = partIndex.child(i, 0);
                Q_ASSERT(anotherPart.isValid()); // guaranteed by the MVC
                QString mimeType2 = anotherPart.data(Imap::Mailbox::RolePartMimeType).toString();
                qDebug() << "subtype " << mimeType2;
                if(mimeType2 == "text/plain"){
                    findPlain = true;
                    fetchSimpleContent(anotherPart, constModel);
                }else{
                    m_tam->add(TrojitaAttachment(anotherPart.data(Imap::Mailbox::RolePartFileName).toString()));
                }
            }
        }
    } else if (mimeType == QLatin1String("message/rfc822")) {
        //        return new Message822Widget(0, this, partIndex, recursionDepth, loadingMode);
    } else {
        part->fetchFromCache(model);

        if (part->fetched() || model->isNetworkOnline() || part->octets() < ExpensiveFetchThreshold) {
            // Show it directly without any fancy wrapping
            //            return new SimplePartWidget(0, manager, partIndex, m_messageView);
            qDebug() << "SimpleWidget";
            fetchSimpleContent(partIndex, constModel);
        } else {
            //            return new LoadablePartWidget(0, manager, partIndex, m_messageView, this, recursionDepth + 1,
            //                                          model->isNetworkAvailable() ? loadingMode : loadingMode | PART_IGNORE_CLICKTHROUGH);
        }
    }
    //    QLabel *lbl = new QLabel(mimeType, 0);
    //    return lbl;
}
void TrojitaMessageDetails::simplePartFetched(){
    qDebug() << "TrojitaMessageDetails::simplePartFetched " ;
    if(m_partIndex.data(Imap::Mailbox::RoleIsFetched).toBool()){
        disconnect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(simplePartFetched(QModelIndex,QModelIndex)));
        qDebug() << "part fetched";
        if (m_partIndex.data(Imap::Mailbox::RolePartMimeType).toString() == QLatin1String("text/plain")
                && m_partIndex.data(Imap::Mailbox::RolePartOctets).toUInt() < 100 * 1024) {
            markupPlainText();
        }else{
            setContent(m_partIndex.data(Imap::Mailbox::RolePartData).toString());
        }
    }
}

void TrojitaMessageDetails::markupPlainText()
{
    // If there's no data, don't try to "fix it up"
    if (!m_partIndex.isValid() || !m_partIndex.data(Imap::Mailbox::RoleIsFetched).toBool())
        return;

    static const QString defaultStyle = QString::fromUtf8(
                "pre{word-wrap: break-word; white-space: pre-wrap;}"
                // The following line, sadly, produces a warning "QFont::setPixelSize: Pixel size <= 0 (0)".
                // However, if it is not in place or if the font size is set higher, even to 0.1px, WebKit reserves space for the
                // quotation characters and therefore a weird white area appears. Even width: 0px doesn't help, so it looks like
                // we will have to live with this warning for the time being.
                ".quotemarks{color:transparent;font-size:0px;}"

                // Cannot really use the :dir(rtl) selector for putting the quote indicator to the "correct" side.
                // It's CSS4 and it isn't supported yet.
                "blockquote{font-size:90%; margin: 4pt 0 4pt 0; padding: 0 0 0 1em; border-left: 2px solid %1; unicode-bidi: -webkit-plaintext}"

                // Stop the font size from getting smaller after reaching two levels of quotes
                // (ie. starting on the third level, don't make the size any smaller than what it already is)
                "blockquote blockquote blockquote {font-size: 100%}"
                ".signature{opacity: 0.6;}"

                // Dynamic quote collapsing via pure CSS, yay
                "input {display: none}"
                "input ~ span.full {display: block}"
                "input ~ span.short {display: none}"
                "input:checked ~ span.full {display: none}"
                "input:checked ~ span.short {display: block}"
                "label {border: 1px solid %2; border-radius: 5px; padding: 0px 4px 0px 4px; white-space: nowrap}"
                // BLACK UP-POINTING SMALL TRIANGLE (U+25B4)
                // BLACK DOWN-POINTING SMALL TRIANGLE (U+25BE)
                "span.full > blockquote > label:before {content: \"\u25b4\"}"
                "span.short > blockquote > label:after {content: \" \u25be\"}"
                "span.shortquote > blockquote > label {display: none}"
                );
    QFont q;
    QFontInfo monospaceInfo(q);
    QString fontSpecification(QLatin1String("pre{"));
    if (monospaceInfo.italic())
        fontSpecification += QLatin1String("font-style: italic; ");
    if (monospaceInfo.bold())
        fontSpecification += QLatin1String("font-weight: bold; ");
    fontSpecification += QString::fromUtf8("font-size: %1px; font-family: \"%2\", monospace }").arg(
                QString::number(monospaceInfo.pixelSize()), monospaceInfo.family());
    QPalette palette;
    QString textColors = QString::fromUtf8("body { background-color: %1; color: %2 }"
                                           "a:link { color: %3 } a:visited { color: %4 } a:hover { color: %3 }").arg(
                palette.base().color().name(), palette.text().color().name(),
                palette.link().color().name(), palette.linkVisited().color().name());

    // build stylesheet and html header
    QColor tintForQuoteIndicator = palette.base().color();
    tintForQuoteIndicator.setAlpha(0x66);
    static QString stylesheet = defaultStyle.arg(palette.link().color().name(),
                                                 tintColor(palette.text().color(), tintForQuoteIndicator).name());
    static QFile file(Common::writablePath(Common::LOCATION_DATA) + QLatin1String("message.css"));
    static QDateTime lastVersion;
    QDateTime lastTouched(file.exists() ? QFileInfo(file).lastModified() : QDateTime());
    if (lastVersion < lastTouched) {
        stylesheet = defaultStyle;
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            const QString userSheet = QString::fromLocal8Bit(file.readAll().data());
            lastVersion = lastTouched;
            stylesheet += "\n" + userSheet;
            file.close();
        }
    }

    //set flow format
    Composer::Util::FlowedFormat flowedFormat=Composer::Util::FORMAT_PLAIN;

    if (m_partIndex.data(Imap::Mailbox::RolePartContentFormat).toString().toLower() == QLatin1String("flowed")) {
        flowedFormat = Composer::Util::FORMAT_FLOWED;

        if (m_partIndex.data(Imap::Mailbox::RolePartContentDelSp).toString().toLower() == QLatin1String("yes"))
            flowedFormat = Composer::Util::FORMAT_FLOWED_DELSP;
    }

    // The dir="auto" is required for WebKit to treat all paragraphs as entities with possibly different text direction.
    // The individual paragraphs unfortunately share the same text alignment, though, as per
    // https://bugs.webkit.org/show_bug.cgi?id=71194 (fixed in Blink already).
    QString htmlHeader("<html><meta name='viewport' content='width=device-width'><head><style type=\"text/css\"><!--" + textColors + fontSpecification + stylesheet +
                       "--></style></head><body><pre dir=\"auto\">");
    static QString htmlFooter("\n</pre></body></html>");

    // We cannot rely on the QWebFrame's toPlainText because of https://bugs.kde.org/show_bug.cgi?id=321160
    QString markup = Composer::Util::plainTextToHtml(
                Imap::decodeByteArray(m_partIndex.data(Imap::Mailbox::RolePartData).toByteArray(),
                                      m_partIndex.data(Imap::Mailbox::RolePartCharset).toString()),
                flowedFormat);

    // and finally set the marked up page.
    setContent(htmlHeader + markup + htmlFooter);
}

QColor TrojitaMessageDetails::tintColor(const QColor &color, const QColor &tintColor){
    QColor finalColor;
    int a = tintColor.alpha();
    if (a == 0xff) {
        finalColor = tintColor;
    } else if (a == 0x00) {
        finalColor = color;
    } else {
        qreal a = tintColor.alphaF();
        qreal inv_a = 1.0 - a;

        finalColor.setRgbF(tintColor.redF() * a + color.redF() * inv_a,
                           tintColor.greenF() * a + color.greenF() * inv_a,
                           tintColor.blueF() * a + color.blueF() * inv_a,
                           a + inv_a * color.alphaF());
    }
    return finalColor;
}
void TrojitaMessageDetails::fetchSimpleContent(QModelIndex anotherPart, const Imap::Mailbox::Model * constModel){
    m_partIndex=anotherPart;
    if(m_partIndex.data(Imap::Mailbox::RoleIsFetched).toBool()){
        simplePartFetched();
    }else{
        Imap::Mailbox::TreeItemPart *part = dynamic_cast<Imap::Mailbox::TreeItemPart *>(Imap::Mailbox::Model::realTreeItem(m_partIndex, &constModel));
        connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                this, SLOT(simplePartFetched()));
        part->fetch(model);
    }
}
