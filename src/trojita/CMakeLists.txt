project(trojita)

set(WITH_QT5 1)
set(WITH_DESKTOP 1)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX11_COMPILER_FLAGS}")
if(WIN32)
    trojita_option(WITH_NSIS "Build Windows NSIS installer" AUTO)
endif()

message(STATUS "Building the Qt5 version")
find_package(Qt5Gui REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5WebKitWidgets REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5LinguistTools)

if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR "lib${LIB_SUFFIX}")
endif()
mark_as_advanced(CMAKE_INSTALL_LIBDIR)

if(NOT CMAKE_INSTALL_PLUGIN_DIR)
    set(CMAKE_INSTALL_PLUGIN_DIR "${CMAKE_INSTALL_LIBDIR}/trojita")
endif()
mark_as_advanced(CMAKE_INSTALL_PLUGIN_DIR)

if(NOT PLUGIN_DIR)
    set(PLUGIN_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_PLUGIN_DIR}")
endif()
mark_as_advanced(PLUGIN_DIR)

if(NOT WIN32 AND NOT MSVC)
    # Check if we are NOT on Windows and don't use MSVC since -g is not supported by VC
    # and Wall makes the build very noisy.
    # At first, enable optimizations. In my testing, these do *not* make debugging any harder than no optimization and
    # they (of course) optimize away stuff like QByteArray::operator[] etc.
    # We put the user's CXXFLAGS *after* that so that they take priority.
    set(CMAKE_CXX_FLAGS "-O2 -g ${CMAKE_CXX_FLAGS}")
    # Now enable build warnings; these are useful tools (and Trojita should be warning-free anyway). We do not set
    # -Werror for sanity reasons, though (one cannot know what warnings a future compiler might bring along).
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wsign-compare")
endif(NOT WIN32 AND NOT MSVC)
if(DEV_FATAL_WARNINGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
# The following is required so that the moc_*.cpp and ui_*.h are found
include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_definitions(-DQT_STRICT_ITERATORS)

# Make sure that plugins not export all symbols, only that which are explicitly marked
include(GenerateExportHeader)
add_compiler_export_flags()

if(NOT WITH_QT5 AND CMAKE_VERSION VERSION_LESS 2.8.10)
    set(SUPPORTS_TARGET_INCLUDES 0)
    include_directories(
        ${QT_MKSPECS_DIR}/default
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Gui/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mimetypes-qt4/io/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Harmattan/qmlapplicationviewer
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_LibMailboxSync)
else()
    set(SUPPORTS_TARGET_INCLUDES 1)
endif()

set(CMAKE_AUTOMOC True)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/configure.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/configure.cmake.h)

set(path_Common ${CMAKE_CURRENT_SOURCE_DIR}/src/Common)
set(libCommon_SOURCES
    ${path_Common}/Application.cpp
    ${path_Common}/ConnectionId.cpp
    ${path_Common}/DeleteAfter.cpp
    ${path_Common}/FileLogger.cpp
    ${path_Common}/MetaTypes.cpp
    ${path_Common}/Paths.cpp
    ${path_Common}/SettingsNames.cpp
)

set(path_Plugins ${CMAKE_CURRENT_SOURCE_DIR}/src/Plugins)
set(libPlugins_SOURCES
    ${path_Plugins}/AddressbookPlugin.cpp
    ${path_Plugins}/PasswordPlugin.cpp
    ${path_Plugins}/PluginJob.cpp
    ${path_Plugins}/PluginManager.cpp
)

set(path_Composer ${CMAKE_CURRENT_SOURCE_DIR}/src/Composer)
set(libComposer_SOURCES
    ${path_Composer}/ComposerAttachments.cpp
    ${path_Composer}/MessageComposer.cpp
    ${path_Composer}/PlainTextFormatter.cpp
    ${path_Composer}/Recipients.cpp
    ${path_Composer}/ReplaceSignature.cpp
    ${path_Composer}/SenderIdentitiesModel.cpp
    ${path_Composer}/SubjectMangling.cpp
    ${path_Composer}/Submission.cpp
)

set(path_MSA ${CMAKE_CURRENT_SOURCE_DIR}/src/MSA)
set(libMSA_SOURCES
    ${path_MSA}/AbstractMSA.cpp
    ${path_MSA}/FakeMSA.cpp
    ${path_MSA}/ImapSubmit.cpp
    ${path_MSA}/SMTP.cpp
    ${path_MSA}/Sendmail.cpp
)

set(path_Streams ${CMAKE_CURRENT_SOURCE_DIR}/src/Streams)
set(libStreams_SOURCES
    ${path_Streams}/DeletionWatcher.cpp
    ${path_Streams}/FakeSocket.cpp
    ${path_Streams}/IODeviceSocket.cpp
    ${path_Streams}/Socket.cpp
    ${path_Streams}/SocketFactory.cpp
)

if(WITH_ZLIB)
    set(libStreams_SOURCES ${libStreams_SOURCES}
        ${path_Streams}/3rdparty/rfc1951.cpp)
    include_directories(${ZLIB_INCLUDE_DIR})
endif()

if(NOT WITH_QT5)
    set(path_mimetypesqt4 ${CMAKE_CURRENT_SOURCE_DIR}/src/mimetypes-qt4)
    set(libMimetypesQt4_SOURCES
        ${path_mimetypesqt4}/io/qstandardpaths.cpp
        ${path_mimetypesqt4}/mimetypes/qmimedatabase.cpp
        ${path_mimetypesqt4}/mimetypes/qmimeglobpattern.cpp
        ${path_mimetypesqt4}/mimetypes/qmimemagicrule.cpp
        ${path_mimetypesqt4}/mimetypes/qmimemagicrulematcher.cpp
        ${path_mimetypesqt4}/mimetypes/qmimetype.cpp
        ${path_mimetypesqt4}/mimetypes/qmimetypeparser.cpp
        ${path_mimetypesqt4}/mimetypes/qmimeprovider.cpp
    )
    if(WIN32)
        set(libMimetypesQt4_SOURCES ${libMimetypesQt4_SOURCES}
            ${path_mimetypesqt4}/io/qstandardpaths_win.cpp)
    elseif(APPLE)
        set(libMimetypesQt4_SOURCES ${libMimetypesQt4_SOURCES}
            ${path_mimetypesqt4}/io/qstandardpaths_mac.cpp)
    elseif (OS2)
        set(libMimetypesQt4_SOURCES ${libMimetypesQt4_SOURCES}
            ${path_mimetypesqt4}/io/qstandardpaths_os2.cpp)
    elseif (UNIX)
        set(libMimetypesQt4_SOURCES ${libMimetypesQt4_SOURCES}
            ${path_mimetypesqt4}/io/qstandardpaths_unix.cpp)
    else()
        message(FATAL_ERROR "Unsupported platform -- mimetypes-Qt4 support only Unix, MacOSX, Windows and OS/2")
    endif()
endif()

set(path_DesktopGui ${CMAKE_CURRENT_SOURCE_DIR}/src/Gui)
set(libDesktopGui_SOURCES
    ${path_DesktopGui}/AttachmentView.cpp
    ${path_DesktopGui}/AutoCompletion.cpp
    ${path_DesktopGui}/CompleteMessageWidget.cpp
    ${path_DesktopGui}/ComposeWidget.cpp
    ${path_DesktopGui}/ComposerAttachmentsList.cpp
    ${path_DesktopGui}/ComposerTextEdit.cpp
    ${path_DesktopGui}/EmbeddedWebView.cpp
    ${path_DesktopGui}/EnvelopeView.cpp
    ${path_DesktopGui}/ExternalElementsWidget.cpp
    ${path_DesktopGui}/FindBar.cpp
    ${path_DesktopGui}/FlowLayout.cpp
    ${path_DesktopGui}/FromAddressProxyModel.cpp
    ${path_DesktopGui}/LineEdit.cpp
    ${path_DesktopGui}/LoadablePartWidget.cpp
    ${path_DesktopGui}/MailBoxTreeView.cpp
    ${path_DesktopGui}/MessageListWidget.cpp
    ${path_DesktopGui}/MessageSourceWidget.cpp
    ${path_DesktopGui}/MessageView.cpp
    ${path_DesktopGui}/MsgListView.cpp
    ${path_DesktopGui}/OnePanelAtTimeWidget.cpp
    ${path_DesktopGui}/OverlayWidget.cpp
    ${path_DesktopGui}/PartWidget.cpp
    ${path_DesktopGui}/PartWidgetFactory.cpp
    ${path_DesktopGui}/PasswordDialog.cpp
    ${path_DesktopGui}/ProgressPopUp.cpp
    ${path_DesktopGui}/ProtocolLoggerWidget.cpp
    ${path_DesktopGui}/ReplaceCharValidator.cpp
    ${path_DesktopGui}/SettingsDialog.cpp
    ${path_DesktopGui}/SimplePartWidget.cpp
    ${path_DesktopGui}/Spinner.cpp
    ${path_DesktopGui}/TagListWidget.cpp
    ${path_DesktopGui}/TagWidget.cpp
    ${path_DesktopGui}/TaskProgressIndicator.cpp
    ${path_DesktopGui}/UserAgentWebPage.cpp
    ${path_DesktopGui}/Util.cpp
    ${path_DesktopGui}/Window.cpp
    ${path_DesktopGui}/ShortcutHandler/ShortcutConfigDialog.cpp
    ${path_DesktopGui}/ShortcutHandler/ShortcutConfigWidget.cpp
    ${path_DesktopGui}/ShortcutHandler/ShortcutHandler.cpp
)
set(libDesktopGui_UI
    ${path_DesktopGui}/AboutDialog.ui
    ${path_DesktopGui}/ComposeWidget.ui
    ${path_DesktopGui}/CreateMailboxDialog.ui
    ${path_DesktopGui}/EditIdentity.ui
    ${path_DesktopGui}/PasswordDialog.ui
    ${path_DesktopGui}/ProgressPopUp.ui
    ${path_DesktopGui}/SettingsCachePage.ui
    ${path_DesktopGui}/SettingsGeneralPage.ui
    ${path_DesktopGui}/SettingsImapPage.ui
    ${path_DesktopGui}/SettingsOutgoingPage.ui
    ${path_DesktopGui}/ShortcutHandler/ShortcutConfigWidget.ui
)
set(libDesktopGui_RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/icons.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/license.qrc
)

set(libqwwsmtpclient_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/qwwsmtpclient/qwwsmtpclient.cpp)

set(libAppVersion_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/AppVersion/SetCoreApplication.cpp)

set(path_Imap ${CMAKE_CURRENT_SOURCE_DIR}/src/Imap)
set(libImap_SOURCES
    ${path_Imap}/ConnectionState.cpp
    ${path_Imap}/Encoders.cpp
    ${path_Imap}/Exceptions.cpp
    ${path_Imap}/Parser/3rdparty/kcodecs.cpp
    ${path_Imap}/Parser/3rdparty/rfccodecs.cpp

    ${path_Imap}/Parser/Command.cpp
    ${path_Imap}/Parser/Data.cpp
    ${path_Imap}/Parser/LowLevelParser.cpp
    ${path_Imap}/Parser/MailAddress.cpp
    ${path_Imap}/Parser/Message.cpp
    ${path_Imap}/Parser/Parser.cpp
    ${path_Imap}/Parser/Response.cpp
    ${path_Imap}/Parser/Sequence.cpp
    ${path_Imap}/Parser/ThreadingNode.cpp

    ${path_Imap}/Network/FileDownloadManager.cpp
    ${path_Imap}/Network/ForbiddenReply.cpp
    ${path_Imap}/Network/MsgPartNetAccessManager.cpp
    ${path_Imap}/Network/MsgPartNetworkReply.cpp

    ${path_Imap}/Model/Cache.cpp
    ${path_Imap}/Model/CombinedCache.cpp
    ${path_Imap}/Model/DragAndDrop.cpp
    ${path_Imap}/Model/DiskPartCache.cpp
    ${path_Imap}/Model/FindInterestingPart.cpp
    ${path_Imap}/Model/FlagsOperation.cpp
    ${path_Imap}/Model/FullMessageCombiner.cpp
    ${path_Imap}/Model/MailboxMetadata.cpp
    ${path_Imap}/Model/MailboxModel.cpp
    ${path_Imap}/Model/MailboxTree.cpp
    ${path_Imap}/Model/MemoryCache.cpp
    ${path_Imap}/Model/Model.cpp
    ${path_Imap}/Model/MsgListModel.cpp
    ${path_Imap}/Model/NetworkWatcher.cpp
    ${path_Imap}/Model/OneMessageModel.cpp
    ${path_Imap}/Model/ParserState.cpp
    ${path_Imap}/Model/PrettyMailboxModel.cpp
    ${path_Imap}/Model/PrettyMsgListModel.cpp
    ${path_Imap}/Model/SpecialFlagNames.cpp
    ${path_Imap}/Model/SQLCache.cpp
    ${path_Imap}/Model/SubtreeModel.cpp
    ${path_Imap}/Model/TaskFactory.cpp
    ${path_Imap}/Model/TaskPresentationModel.cpp
    ${path_Imap}/Model/ThreadingMsgListModel.cpp
    ${path_Imap}/Model/Utils.cpp
    ${path_Imap}/Model/VisibleTasksModel.cpp

    # The ModelTest is only needed when debugging manually
    #${path_Imap}/Model/ModelTest/modeltest.cpp
    # The ModelWatcher is another debugging aid
    ${path_Imap}/Model/ModelWatcher.cpp

    ${path_Imap}/Model/kdeui-itemviews/kdescendantsproxymodel.cpp

    ${path_Imap}/Tasks/AppendTask.cpp
    ${path_Imap}/Tasks/CopyMoveMessagesTask.cpp
    ${path_Imap}/Tasks/CreateMailboxTask.cpp
    ${path_Imap}/Tasks/DeleteMailboxTask.cpp
    ${path_Imap}/Tasks/EnableTask.cpp
    ${path_Imap}/Tasks/ExpungeMailboxTask.cpp
    ${path_Imap}/Tasks/ExpungeMessagesTask.cpp
    ${path_Imap}/Tasks/Fake_ListChildMailboxesTask.cpp
    ${path_Imap}/Tasks/Fake_OpenConnectionTask.cpp
    ${path_Imap}/Tasks/FetchMsgMetadataTask.cpp
    ${path_Imap}/Tasks/FetchMsgPartTask.cpp
    ${path_Imap}/Tasks/GenUrlAuthTask.cpp
    ${path_Imap}/Tasks/GetAnyConnectionTask.cpp
    ${path_Imap}/Tasks/IdTask.cpp
    ${path_Imap}/Tasks/IdleLauncher.cpp
    ${path_Imap}/Tasks/ImapTask.cpp
    ${path_Imap}/Tasks/KeepMailboxOpenTask.cpp
    ${path_Imap}/Tasks/ListChildMailboxesTask.cpp
    ${path_Imap}/Tasks/NoopTask.cpp
    ${path_Imap}/Tasks/NumberOfMessagesTask.cpp
    ${path_Imap}/Tasks/ObtainSynchronizedMailboxTask.cpp
    ${path_Imap}/Tasks/OfflineConnectionTask.cpp
    ${path_Imap}/Tasks/OpenConnectionTask.cpp
    ${path_Imap}/Tasks/SortTask.cpp
    ${path_Imap}/Tasks/SubscribeUnsubscribeTask.cpp
    ${path_Imap}/Tasks/ThreadTask.cpp
    ${path_Imap}/Tasks/UidSubmitTask.cpp
    ${path_Imap}/Tasks/UnSelectTask.cpp
    ${path_Imap}/Tasks/UpdateFlagsTask.cpp
    ${path_Imap}/Tasks/UpdateFlagsOfAllMessagesTask.cpp
)

if(WITH_RAGEL)
    message(STATUS "Using Ragel for the RFC 5322 parser")
    ragel_parser(${path_Imap}/Parser/Rfc5322HeaderParser.cpp)
    set(libImap_SOURCES ${libImap_SOURCES}
        ${CMAKE_CURRENT_BINARY_DIR}/Rfc5322HeaderParser.generated.cpp)
else()
    message(STATUS "Using pregenerated RFC 5322 parser")
    set(libImap_SOURCES ${libImap_SOURCES}
        ${path_Imap}/Parser/Rfc5322HeaderParser.generated.cpp)
endif()

set(path_AbookAddressbook ${CMAKE_CURRENT_SOURCE_DIR}/src/AbookAddressbook)
set(libAbookAddressbook_SOURCES
    ${path_AbookAddressbook}/AbookAddressbook.cpp
    ${path_AbookAddressbook}/be-contacts.cpp
)
set(libAbookAddressbook_UI
    ${path_AbookAddressbook}/be-contacts.ui
    ${path_AbookAddressbook}/onecontact.ui
)

set(trojita_desktop_SOURCES
    ${path_DesktopGui}/main.cpp
)

if(WIN32)
    list(APPEND trojita_desktop_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/trojita_win32.rc)
endif()

set(be_contacts_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/be.contacts/main.cpp
)

set(libQmlModelGlue_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/QmlSupport/ModelGlue/ImapAccess.cpp
)

set(libQNAMWebView_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/QmlSupport/QNAMWebView/plugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/QmlSupport/QNAMWebView/qdeclarativewebview.cpp
)

set(trojitaHarmattan_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Harmattan/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Harmattan/qmlapplicationviewer/qmlapplicationviewer.cpp
)

set(version_files ${CMAKE_CURRENT_BINARY_DIR}/trojita-version.h ${CMAKE_CURRENT_BINARY_DIR}/trojita-git-version.h)

add_custom_target(version DEPENDS version_fake_file)
add_custom_command(OUTPUT version_fake_file ${version_files}
    COMMAND ${CMAKE_COMMAND} -DGIT_EXECUTABLE=${GIT_EXECUTABLE} -DNSIS=${NSIS} -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/TrojitaVersion.cmake)
set_source_files_properties(${version_files}
    PROPERTIES GENERATED TRUE
    HEADER_FILE_ONLY TRUE)

add_library(Common ${libCommon_SOURCES})
add_dependencies(Common version)
target_link_libraries(Common ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY})

add_library(AppVersion ${libAppVersion_SOURCES})
add_dependencies(AppVersion version)
target_link_libraries(AppVersion Common ${QT_QTCORE_LIBRARY})

if(WITH_SHARED_PLUGINS)
    add_library(Plugins SHARED ${libPlugins_SOURCES})
else()
    add_library(Plugins STATIC ${libPlugins_SOURCES})
    set_property(TARGET Plugins APPEND PROPERTY COMPILE_DEFINITIONS QT_STATICPLUGIN)
endif()
set_target_properties(Plugins PROPERTIES OUTPUT_NAME trojita_plugins)
if(WITH_QT5)
    qt5_use_modules(Plugins Core)
else()
    target_link_libraries(Plugins ${QT_QTCORE_LIBRARY})
endif()

add_library(Streams ${libStreams_SOURCES})
target_link_libraries(Streams ${QT_QTNETWORK_LIBRARY} ${QT_QTCORE_LIBRARY})
if(WITH_ZLIB)
    target_link_libraries(Streams ${ZLIB_LIBRARIES})
endif()

add_library(qwwsmtpclient ${libqwwsmtpclient_SOURCES})
target_link_libraries(qwwsmtpclient ${QT_QTNETWORK_LIBRARY} ${QT_QTCORE_LIBRARY})

add_library(MSA ${libMSA_SOURCES})
target_link_libraries(MSA Imap Streams qwwsmtpclient ${QT_QTCORE_LIBRARY})

add_library(Composer ${libComposer_SOURCES})
target_link_libraries(Composer Common MSA Streams qwwsmtpclient ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY})
if(NOT WITH_QT5)
    target_link_libraries(Composer MimetypesQt4)
endif()

add_library(Imap ${libImap_SOURCES})
target_link_libraries(Imap Common Streams ${QT_QTNETWORK_LIBRARY} ${QT_QTSQL_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY})
if(WITH_ZLIB)
    target_link_libraries(Imap ${ZLIB_LIBRARIES})
endif()

# Generate file static_plugins.h.in
get_property(STATIC_PLUGINS GLOBAL PROPERTY TROJITA_STATIC_PLUGINS)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/static_plugins.h.in "#include <QtPlugin>\n")
foreach(PLUGIN ${STATIC_PLUGINS})
    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/static_plugins.h.in "Q_IMPORT_PLUGIN(${PLUGIN})\n")
endforeach()
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/static_plugins.h.in ${CMAKE_CURRENT_BINARY_DIR}/static_plugins.h)

if(WITH_DESKTOP)
    if(WITH_QT5)
        qt5_wrap_ui(libAbookAddressbook_UI_OUT ${libAbookAddressbook_UI})
        qt5_wrap_ui(libDesktopGui_UI_OUT ${libDesktopGui_UI})
        qt5_add_resources(libDesktopGui_RESOURCES_OUT ${libDesktopGui_RESOURCES})
    else()
        qt4_wrap_ui(libAbookAddressbook_UI_OUT ${libAbookAddressbook_UI})
        qt4_wrap_ui(libDesktopGui_UI_OUT ${libDesktopGui_UI})
        qt4_add_resources(libDesktopGui_RESOURCES_OUT ${libDesktopGui_RESOURCES})
    endif()

    add_library(AbookAddressbook ${libAbookAddressbook_SOURCES} ${libAbookAddressbook_UI_OUT})
    target_link_libraries(AbookAddressbook ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY})

    add_library(DesktopGui ${libDesktopGui_SOURCES} ${libDesktopGui_UI_OUT} ${libDesktopGui_RESOURCES_OUT})
    # The following is needed for the LineEdit widget within the .ui files.
    # The ${path_DesktopGui} is needed so that the generated ui_*.h file can find the headers of the custom widgets
    if(SUPPORTS_TARGET_INCLUDES)
        set_property(TARGET DesktopGui APPEND PROPERTY INCLUDE_DIRECTORIES ${path_DesktopGui})
    endif()
    target_link_libraries(DesktopGui Common Composer Imap MSA Plugins Streams qwwsmtpclient AbookAddressbook ${QT_QTWEBKIT_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY})

    # On Windows build a real Win32 GUI application without console window
    # On other platforms WIN32 flag is ignored
    add_library(trojita ${trojita_desktop_SOURCES} ${trojita_QM})
    target_link_libraries(trojita AppVersion Common DesktopGui ${STATIC_PLUGINS} ${QT_QTMAIN_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY})
    if(NOT WITH_QT5)
        target_link_libraries(trojita MimetypesQt4)
    endif()
    if(WITH_ZLIB)
        target_link_libraries(trojita ${ZLIB_LIBRARIES})
    endif()

    add_executable(be.contacts ${be_contacts_SOURCES})
    target_link_libraries(be.contacts AbookAddressbook ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY})
elseif(WITH_HARMATTAN)
    add_library(QmlModelGlue ${libQmlModelGlue_SOURCES})
    add_library(trojitaqnamwebviewplugin SHARED ${libQNAMWebView_SOURCES})
    add_executable(trojita-tp ${trojitaHarmattan_SOURCES})
    if(SUPPORTS_TARGET_INCLUDES)
        set_property(TARGET trojita-tp APPEND PROPERTY INCLUDE_DIRECTORIES
            ${QT_MKSPECS_DIR}/default ${CMAKE_CURRENT_SOURCE_DIR}/src/Harmattan/qmlapplicationviewer)
    endif()
    target_link_libraries(trojita-tp QmlModelGlue AppVersion Imap MSA Streams qwwsmtpclient Common Composer MimetypesQt4
        ${QT_QTSQL_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTWEBKIT_LIBRARY} ${QT_QTDECLARATIVE_LIBRARY})
endif()
message("use qt 5" ${WITH_QT5})
if(WITH_QT5)
    qt5_use_modules(AppVersion Core)
    qt5_use_modules(Common Core Network)
    qt5_use_modules(Streams Network)
    qt5_use_modules(qwwsmtpclient Network)
    qt5_use_modules(MSA Network)
    qt5_use_modules(Composer Gui Network)
    qt5_use_modules(Imap Gui Network Sql)
    qt5_use_modules(DesktopGui Network WebKitWidgets)
    qt5_use_modules(AbookAddressbook Widgets)
    qt5_use_modules(be.contacts Widgets)
    qt5_use_modules(trojita Widgets Network)
endif()

if(WITH_SHARED_PLUGINS)
    install(TARGETS Plugins DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

