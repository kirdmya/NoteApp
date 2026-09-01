#include <QtTest>

#include <QAction>
#include <QStatusBar>
#include <QTabWidget>
#include <QTemporaryDir>
#include <QTreeView>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>

#include "app/App.h"
#include "app/AppVersion.h"
#include "core/usecases/WorkspaceService.h"
#include "core/usecases/DocumentManager.h"
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
    void initTestCase();
    void init();
    void listNoteFilesReturnsOnlySupportedExtensions();
    void listNoteFilesReturnsEmptyForMissingDirectory();
    void fileRepositoryReadsAndWritesSupportedFiles();
    void fileRepositorySaveAsCreatesFile();
    void fileRepositoryDoesNotRecreateMissingFile();
    void fileRepositoryRejectsUnsupportedFiles();
    void fileRepositoryCreatesFileAndFolder();
    void fileRepositoryRejectsDuplicateEntries();
    void fileRepositoryMovesNoteToTrash();
    void fileRepositoryRejectsInvalidDelete();
    void fileRepositoryRenamesNote();
    void fileRepositoryRenamesFolder();
    void fileRepositoryRejectsInvalidRename();
    void workspaceServiceStartsWithDefaultWorkspace();
    void workspaceServicePersistsLastWorkspacePath();
    void documentManagerTracksSessions();
    void appCreatesCoreServices();
    void mainWindowBuildsExpectedUi();
};

void NoteAppTests::initTestCase()
{
    QStandardPaths::setTestModeEnabled(true);
    QCoreApplication::setOrganizationName("NoteAppTests");
    QCoreApplication::setApplicationName("NoteAppTests");
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, QDir::tempPath());
}

void NoteAppTests::init()
{
    QSettings settings;
    settings.clear();
    settings.sync();
}

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

void NoteAppTests::fileRepositoryReadsAndWritesSupportedFiles()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    const QString filePath = tempDir.filePath("note.txt");
    createFile(filePath);

    storage::FsFileRepository repo;
    QString content;

    QVERIFY(repo.canOpenInEditor(filePath));
    QVERIFY(repo.readTextFile(filePath, content));
    QCOMPARE(content, QString("test"));

    QVERIFY(repo.writeTextFile(filePath, "updated"));
    QVERIFY(repo.readTextFile(filePath, content));
    QCOMPARE(content, QString("updated"));
}

void NoteAppTests::fileRepositorySaveAsCreatesFile()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    const QString filePath = tempDir.filePath("saved-as.md");
    storage::FsFileRepository repo;
    QString content;

    QVERIFY(repo.writeTextFileAs(filePath, "new content"));
    QVERIFY(repo.readTextFile(filePath, content));
    QCOMPARE(content, QString("new content"));
}

void NoteAppTests::fileRepositoryDoesNotRecreateMissingFile()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    const QString filePath = tempDir.filePath("missing.txt");
    storage::FsFileRepository repo;

    QVERIFY(!repo.writeTextFile(filePath, "content"));
    QVERIFY(!QFileInfo::exists(filePath));
}

void NoteAppTests::fileRepositoryRejectsUnsupportedFiles()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    const QString filePath = tempDir.filePath("image.png");
    createFile(filePath);

    storage::FsFileRepository repo;
    QString content;

    QVERIFY(!repo.canOpenInEditor(filePath));
    QVERIFY(!repo.readTextFile(filePath, content));
    QVERIFY(!repo.writeTextFile(filePath, "updated"));
}

void NoteAppTests::fileRepositoryCreatesFileAndFolder()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    storage::FsFileRepository repo;
    QString filePath;
    QString folderPath;

    QVERIFY(repo.createFile(tempDir.path(), "note.md", filePath));
    QVERIFY(QFileInfo(filePath).isFile());
    QVERIFY(repo.createFolder(tempDir.path(), "Nested", folderPath));
    QVERIFY(QFileInfo(folderPath).isDir());
}

void NoteAppTests::fileRepositoryRejectsDuplicateEntries()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    storage::FsFileRepository repo;
    QString createdPath;

    QVERIFY(repo.createFile(tempDir.path(), "note.txt", createdPath));
    QVERIFY(!repo.createFile(tempDir.path(), "note.txt", createdPath));
    QVERIFY(!repo.createFile(tempDir.path(), "image.png", createdPath));
    QVERIFY(repo.createFolder(tempDir.path(), "Folder", createdPath));
    QVERIFY(!repo.createFolder(tempDir.path(), "Folder", createdPath));
}

void NoteAppTests::fileRepositoryMovesNoteToTrash()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    const QString filePath = tempDir.filePath("note.md");
    createFile(filePath);

    storage::FsFileRepository repo;
    if (!repo.moveToTrash(filePath)) {
        QSKIP("The OS Recycle Bin is unavailable for the temporary directory");
    }
    QVERIFY(!QFileInfo::exists(filePath));
}

void NoteAppTests::fileRepositoryRejectsInvalidDelete()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    const QString unsupportedPath = tempDir.filePath("image.png");
    createFile(unsupportedPath);

    storage::FsFileRepository repo;
    QVERIFY(!repo.moveToTrash(tempDir.filePath("missing.md")));
    QVERIFY(QFileInfo::exists(unsupportedPath));
}

void NoteAppTests::fileRepositoryRenamesNote()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    const QString sourcePath = tempDir.filePath("before.md");
    createFile(sourcePath);

    storage::FsFileRepository repo;
    QString renamedPath;

    QVERIFY(repo.renamePath(sourcePath, "after.md", renamedPath));
    QCOMPARE(renamedPath, tempDir.filePath("after.md"));
    QVERIFY(!QFileInfo::exists(sourcePath));
    QVERIFY(QFileInfo::exists(renamedPath));
}

void NoteAppTests::fileRepositoryRenamesFolder()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    const QString sourcePath = tempDir.filePath("Before");
    QVERIFY(QDir().mkdir(sourcePath));

    storage::FsFileRepository repo;
    QString renamedPath;

    QVERIFY(repo.renamePath(sourcePath, "After", renamedPath));
    QCOMPARE(renamedPath, tempDir.filePath("After"));
    QVERIFY(!QFileInfo::exists(sourcePath));
    QVERIFY(QFileInfo(renamedPath).isDir());
}

void NoteAppTests::fileRepositoryRejectsInvalidRename()
{
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());

    const QString sourcePath = tempDir.filePath("note.md");
    createFile(sourcePath);

    storage::FsFileRepository repo;
    QString renamedPath;

    QVERIFY(!repo.renamePath(sourcePath, "../outside.md", renamedPath));
    QVERIFY(!repo.renamePath(sourcePath, "image.png", renamedPath));
    QVERIFY(QFileInfo::exists(sourcePath));
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

void NoteAppTests::documentManagerTracksSessions()
{
    core::DocumentManager manager;

    core::DocumentSession& first = manager.open("C:/notes/first.md", "alpha");
    QCOMPARE(first.text, QString("alpha"));
    QCOMPARE(manager.sessions().size(), 1);

    manager.open("C:/notes/first.md", "ignored duplicate");
    manager.open("C:/notes/nested/second.txt", "beta");
    QCOMPARE(manager.sessions().size(), 2);

    manager.update("C:/notes/first.md", "changed", true);
    const core::DocumentSession* updated = manager.find("C:/notes/first.md");
    QVERIFY(updated != nullptr);
    QCOMPARE(updated->text, QString("changed"));
    QVERIFY(updated->modified);

    QCOMPARE(manager.remapPath("C:/notes", "D:/workspace"), 2);
    QVERIFY(manager.find("D:/workspace/first.md") != nullptr);
    QCOMPARE(manager.closeUnder("D:/workspace/nested"), 1);
    QCOMPARE(manager.sessions().size(), 1);
    QVERIFY(manager.close("D:/workspace/first.md"));
    QVERIFY(manager.sessions().isEmpty());
}

void NoteAppTests::appCreatesCoreServices()
{
    app::App app;

    QCOMPARE(app.settings().lastWorkspacePath(), QString());
    QCOMPARE(app.workspaceService().current().displayName, QString("No workspace"));
    QVERIFY(app.documentManager().sessions().isEmpty());
    QVERIFY(app.canOpenFileInEditor("note.txt"));
    QVERIFY(!app.canOpenFileInEditor("note.bin"));
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
    auto* saveAction = window.findChild<QAction*>(ui::actions::kSave);
    auto* saveAsAction = window.findChild<QAction*>(ui::actions::kSaveAs);
    QVERIFY(saveAction != nullptr);
    QVERIFY(saveAsAction != nullptr);
    QCOMPARE(saveAction->shortcut(), QKeySequence::Save);
    QCOMPARE(saveAsAction->shortcut(), QKeySequence::SaveAs);
    QVERIFY(window.findChild<QAction*>(ui::actions::kExit) != nullptr);
    QVERIFY(window.findChild<QAction*>(ui::actions::kAbout) != nullptr);
    QVERIFY(window.findChild<QAction*>(ui::actions::kRenameNote) != nullptr);
    QVERIFY(window.findChild<QAction*>(ui::actions::kDeleteNote) != nullptr);
    QVERIFY(window.findChild<QAction*>(ui::actions::kNewFile) != nullptr);
    QVERIFY(window.findChild<QAction*>(ui::actions::kNewFolder) != nullptr);
    QVERIFY(window.findChild<QAction*>(ui::actions::kOpenInExplorer) != nullptr);
    QVERIFY(window.findChild<QAction*>(ui::actions::kProperties) != nullptr);
}

QTEST_MAIN(NoteAppTests)
#include "test_main.moc"
