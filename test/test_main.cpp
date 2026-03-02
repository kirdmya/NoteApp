#include <QtTest>

#include <QAction>
#include <QStatusBar>
#include <QTabWidget>
#include <QTemporaryDir>
#include <QTreeView>
#include <QFile>

#include "app/App.h"
#include "app/AppVersion.h"
#include "core/usecases/WorkspaceService.h"
#include "infra/settings/Settings.h"
#include "network/StubNetworkClient.h"
#include "storage/FsFileRepository.h"
#include "ui/mainwindow.h"
#include "ui/actions/ActionIds.h"

namespace {

void createFile(const QString& path)
{
    QFile file(path);
    const bool opened = file.open(QIODevice::WriteOnly | QIODevice::Text);
    Q_ASSERT(opened);
    file.write("test");
    file.close();
}

} // namespace

class NoteAppTests final : public QObject {
    Q_OBJECT

private slots:
    void listNoteFilesReturnsOnlySupportedExtensions();
    void listNoteFilesReturnsEmptyForMissingDirectory();
    void workspaceServiceStartsWithDefaultWorkspace();
    void workspaceServicePersistsLastWorkspacePath();
    void appCreatesCoreServices();
    void mainWindowBuildsExpectedUi();
};

void NoteAppTests::listNoteFilesReturnsOnlySupportedExtensions()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    createFile(tempDir.filePath("alpha.md"));
    createFile(tempDir.filePath("beta.txt"));
    createFile(tempDir.filePath("ignore.png"));

    storage::FsFileRepository repo;
    QStringList files = repo.listNoteFiles(tempDir.path());
    files.sort();

    QCOMPARE(files, QStringList({"alpha.md", "beta.txt"}));
}

void NoteAppTests::listNoteFilesReturnsEmptyForMissingDirectory()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    storage::FsFileRepository repo;
    const QStringList files = repo.listNoteFiles(tempDir.filePath("missing"));

    QVERIFY(files.isEmpty());
}

void NoteAppTests::workspaceServiceStartsWithDefaultWorkspace()
{
    storage::FsFileRepository repo;
    network::StubNetworkClient network;
    infra::Settings settings;
    core::WorkspaceService service(repo, network, settings);

    const core::Workspace workspace = service.current();

    QCOMPARE(workspace.rootPath, QString());
    QCOMPARE(workspace.displayName, QString("No workspace"));
    QCOMPARE(settings.lastWorkspacePath(), QString());
}

void NoteAppTests::workspaceServicePersistsLastWorkspacePath()
{
    storage::FsFileRepository repo;
    network::StubNetworkClient network;
    infra::Settings settings;
    core::WorkspaceService service(repo, network, settings);

    const core::Workspace expected{"C:/notes", "My Notes"};
    service.setCurrent(expected);

    const core::Workspace actual = service.current();
    QCOMPARE(actual.rootPath, expected.rootPath);
    QCOMPARE(actual.displayName, expected.displayName);
    QCOMPARE(settings.lastWorkspacePath(), expected.rootPath);
}

void NoteAppTests::appCreatesCoreServices()
{
    app::App app;

    QCOMPARE(app.settings().lastWorkspacePath(), QString());
    QCOMPARE(app.workspaceService().current().displayName, QString("No workspace"));
}

void NoteAppTests::mainWindowBuildsExpectedUi()
{
    app::App app;
    ui::MainWindow window(app);

    QCOMPARE(window.windowTitle(), QString("%1 v%2").arg(app::kAppName).arg(app::kAppVersion));
    QCOMPARE(window.statusBar()->currentMessage(), QString("Ready"));

    QVERIFY(window.findChild<QTreeView*>() != nullptr);

    auto* tabs = window.findChild<QTabWidget*>();
    QVERIFY(tabs != nullptr);
    QCOMPARE(tabs->count(), 1);
    QCOMPARE(tabs->tabText(0), QString("Welcome"));

    QVERIFY(window.findChild<QAction*>(ui::actions::kOpenWorkspace) != nullptr);
    QVERIFY(window.findChild<QAction*>(ui::actions::kSave) != nullptr);
    QVERIFY(window.findChild<QAction*>(ui::actions::kExit) != nullptr);
    QVERIFY(window.findChild<QAction*>(ui::actions::kAbout) != nullptr);
}

QTEST_MAIN(NoteAppTests)
#include "test_main.moc"
