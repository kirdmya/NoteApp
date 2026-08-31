#include "MainWindow.h"

#include <QAction>
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QInputDialog>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QSplitter>
#include <QStatusBar>
#include <QTabWidget>
#include <QTextDocument>
#include <QTreeView>
#include <QUrl>

#include "core/domain/Workspace.h"
#include "core/usecases/IWorkspaceService.h"
#include "infra/settings/Settings.h"
#include "ui/widgets/PlaceholderWidget.h"
#include "ui/actions/ActionIds.h"
#include "app/App.h"
#include "app/AppVersion.h"

namespace {

QString workspaceDisplayName(const QString& rootPath)
{
    const QFileInfo info(rootPath);
    const QString displayName = info.fileName();
    return displayName.isEmpty() ? QDir::toNativeSeparators(rootPath) : displayName;
}

QString formatFileSize(const qint64 bytes)
{
    constexpr qint64 kibibyte = 1024;
    constexpr qint64 mebibyte = kibibyte * 1024;

    if (bytes >= mebibyte) {
        return QString("%1 MB").arg(bytes / static_cast<double>(mebibyte), 0, 'f', 1);
    }
    if (bytes >= kibibyte) {
        return QString("%1 KB").arg(bytes / static_cast<double>(kibibyte), 0, 'f', 1);
    }
    return QString("%1 bytes").arg(bytes);
}

bool isSameOrChildPath(const QString& path, const QString& parentPath)
{
    const QString cleanPath = QDir::cleanPath(path);
    QString cleanParent = QDir::cleanPath(parentPath);
    if (!cleanParent.endsWith('/')) {
        cleanParent += '/';
    }

#ifdef Q_OS_WIN
    constexpr Qt::CaseSensitivity pathCase = Qt::CaseInsensitive;
#else
    constexpr Qt::CaseSensitivity pathCase = Qt::CaseSensitive;
#endif

    return cleanPath.compare(QDir::cleanPath(parentPath), pathCase) == 0
        || cleanPath.startsWith(cleanParent, pathCase);
}

bool pathsEqual(const QString& left, const QString& right)
{
#ifdef Q_OS_WIN
    constexpr Qt::CaseSensitivity pathCase = Qt::CaseInsensitive;
#else
    constexpr Qt::CaseSensitivity pathCase = Qt::CaseSensitive;
#endif
    return QDir::cleanPath(left).compare(QDir::cleanPath(right), pathCase) == 0;
}

}

namespace ui {

MainWindow::MainWindow(app::App& app, QWidget* parent)
    : QMainWindow(parent)
    , app_(app)
{
    setWindowTitle(QString("%1 v%2").arg(app::kAppName).arg(app::kAppVersion));
    resize(1100, 700);

    setupUiRuntime();
    setupActions();
    connectSignals();

    statusBar()->showMessage("Ready");
    restoreLastWorkspace();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUiRuntime()
{
    auto* splitter = new QSplitter(this);

    tree_ = new QTreeView(splitter);
    tree_->setHeaderHidden(true);
    tree_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tree_->setSortingEnabled(true);
    tree_->setMinimumWidth(57);
    tree_->setContextMenuPolicy(Qt::CustomContextMenu);
    tree_->setVisible(false);

    const QStringList filters = { "*.txt", "*.md" };
    fileModel_ = new QFileSystemModel(this);
    fileModel_->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    fileModel_->setNameFilters(filters);
    fileModel_->setNameFilterDisables(false);
    tree_->setModel(fileModel_);

    tabs_ = new QTabWidget(splitter);
    tabs_->setTabsClosable(true);
    openWelcomeTab();

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 3);

    textInfo_ = new QLabel("column: 1, row: 1", statusBar()->window());
    statusBar()->addPermanentWidget(textInfo_);
    statusBar()->setSizeGripEnabled(false);
    textInfo_->setMouseTracking(true);
    textInfo_->setFrameStyle(QFrame::NoFrame);
    textInfo_->setStyleSheet("border: 3px solid #FF8F2E; border-radius: 6px; "
                             "background-color: #FFB573; color: #472100; "
                             "margin: 0 2px 0 0; "); // 2px from right side
    textInfo_->setVisible(false);

    setCentralWidget(splitter);
}

void MainWindow::setupActions()
{
    auto* fileMenu = menuBar()->addMenu("&File");
    auto* helpMenu = menuBar()->addMenu("&Help");

    auto* openWorkspace = new QAction("Open workspace...", this);
    openWorkspace->setObjectName(ui::actions::kOpenWorkspace);

    auto* save = new QAction("Save", this);
    save->setObjectName(ui::actions::kSave);
    save->setShortcut(QKeySequence::Save);

    auto* newFile = new QAction("New File...", this);
    newFile->setObjectName(ui::actions::kNewFile);

    auto* newFolder = new QAction("New Folder", this);
    newFolder->setObjectName(ui::actions::kNewFolder);

    auto* renameNote = new QAction("Rename", this);
    renameNote->setObjectName(ui::actions::kRenameNote);

    auto* exit = new QAction("Exit", this);
    exit->setObjectName(ui::actions::kExit);

    fileMenu->addAction(openWorkspace);
    fileMenu->addSeparator();
    fileMenu->addAction(save);
    fileMenu->addSeparator();
    fileMenu->addAction(exit);

    auto* about = new QAction("About", this);
    about->setObjectName(ui::actions::kAbout);
    helpMenu->addAction(about);

    auto* deleteNote = new QAction("Delete", this);
    deleteNote->setObjectName(ui::actions::kDeleteNote);

    auto* openInExplorer = new QAction("Open folder in Explorer", this);
    openInExplorer->setObjectName(ui::actions::kOpenInExplorer);

    auto* properties = new QAction("Properties", this);
    properties->setObjectName(ui::actions::kProperties);
}

void MainWindow::connectSignals()
{
    auto* exit = findChild<QAction*>(ui::actions::kExit);
    connect(exit, &QAction::triggered, this, &QWidget::close);

    auto* about = findChild<QAction*>(ui::actions::kAbout);
    connect(about, &QAction::triggered, this, [this](){
        QMessageBox::about(this, "About",
            QString("%1\nVersion: %2").arg(app::kAppName).arg(app::kAppVersion));
    });

    auto* save = findChild<QAction*>(ui::actions::kSave);
    connect(save, &QAction::triggered, this, &MainWindow::saveCurrentNote);

    auto* openWorkspace = findChild<QAction*>(ui::actions::kOpenWorkspace);
    connect(openWorkspace, &QAction::triggered, this, &MainWindow::openWorkspace);

    connect(tree_, &QTreeView::activated, this, &MainWindow::openItem);

    connect(tabs_, &QTabWidget::tabCloseRequested, this, [this](const int index) {
        (void)closeTabAt(index);
    });

    connect(tabs_, &QTabWidget::currentChanged, this, [this](int index) {
        updateTextCursor(qobject_cast<QPlainTextEdit*>(tabs_->widget(index)));
    });

    auto* newFile = findChild<QAction*>(ui::actions::kNewFile);
    auto* newFolder = findChild<QAction*>(ui::actions::kNewFolder);
    auto* renameAction = findChild<QAction*>(ui::actions::kRenameNote);
    auto* deleteAction = findChild<QAction*>(ui::actions::kDeleteNote);
    auto* openInExplorer = findChild<QAction*>(ui::actions::kOpenInExplorer);
    auto* properties = findChild<QAction*>(ui::actions::kProperties);

    connect(newFile, &QAction::triggered, this, &MainWindow::createNoteFile);
    connect(newFolder, &QAction::triggered, this, &MainWindow::createFolder);
    connect(renameAction, &QAction::triggered, this, &MainWindow::renamePath);
    connect(deleteAction, &QAction::triggered, this, &MainWindow::deletePath);
    connect(openInExplorer, &QAction::triggered, this, &MainWindow::openFolderInExplorer);
    connect(properties, &QAction::triggered, this, &MainWindow::showProperties);
    connect(tree_, &QWidget::customContextMenuRequested, this, &MainWindow::showTreeContextMenu);
}

void MainWindow::showTreeContextMenu(const QPoint& position)
{
    QModelIndex index = tree_->indexAt(position);
    if (!index.isValid()) {
        index = fileModel_->index(app_.workspaceService().current().rootPath);
    }
    if (!index.isValid()) {
        return;
    }

    tree_->setCurrentIndex(index);

    auto* newFile = findChild<QAction*>(ui::actions::kNewFile);
    auto* newFolder = findChild<QAction*>(ui::actions::kNewFolder);
    auto* renameAction = findChild<QAction*>(ui::actions::kRenameNote);
    auto* deleteAction = findChild<QAction*>(ui::actions::kDeleteNote);
    auto* openInExplorer = findChild<QAction*>(ui::actions::kOpenInExplorer);
    auto* properties = findChild<QAction*>(ui::actions::kProperties);

    const bool isWorkspaceRoot = pathsEqual(
        fileModel_->filePath(index),
        app_.workspaceService().current().rootPath);
    renameAction->setEnabled(!isWorkspaceRoot);
    deleteAction->setEnabled(!isWorkspaceRoot);

    QMenu menu(this);
    menu.addAction(newFile);
    menu.addAction(newFolder);
    menu.addSeparator();
    menu.addAction(renameAction);
    menu.addAction(deleteAction);
    menu.addSeparator();
    menu.addAction(openInExplorer);
    menu.addAction(properties);
    menu.exec(tree_->viewport()->mapToGlobal(position));
}

QString MainWindow::selectedDirectoryPath() const
{
    const QModelIndex index = tree_->currentIndex();
    if (!index.isValid()) {
        return {};
    }

    const QFileInfo info(fileModel_->filePath(index));
    return info.isDir() ? info.absoluteFilePath() : info.absolutePath();
}

void MainWindow::showFileSystemError(const QString& title, const QString& message)
{
    statusBar()->showMessage(message, 5000);
    QMessageBox::warning(this, title, message);
}

void MainWindow::createNoteFile()
{
    const QString directoryPath = selectedDirectoryPath();
    if (directoryPath.isEmpty()) {
        return;
    }

    bool accepted = false;
    const QString format = QInputDialog::getItem(
        this,
        "New file",
        "File type:",
        {"Markdown (.md)", "Text (.txt)"},
        0,
        false,
        &accepted);
    if (!accepted) {
        return;
    }

    QString fileName = QInputDialog::getText(
        this,
        "New file",
        "File name:",
        QLineEdit::Normal,
        "New note",
        &accepted).trimmed();
    if (!accepted || fileName.isEmpty()) {
        return;
    }

    const QString extension = format.startsWith("Markdown") ? ".md" : ".txt";
    const QString currentSuffix = QFileInfo(fileName).suffix();
    if (currentSuffix.compare("md", Qt::CaseInsensitive) == 0
        || currentSuffix.compare("txt", Qt::CaseInsensitive) == 0) {
        fileName.chop(currentSuffix.size() + 1);
    }
    fileName += extension;

    QString createdPath;
    if (!app_.createNoteFile(directoryPath, fileName, createdPath)) {
        showFileSystemError(
            "Unable to create file",
            "The file could not be created. Check its name, permissions, and whether it already exists.");
        return;
    }

    openNoteFile(createdPath);
    statusBar()->showMessage(QString("Created: %1").arg(fileName), 3000);
}

void MainWindow::createFolder()
{
    const QString directoryPath = selectedDirectoryPath();
    if (directoryPath.isEmpty()) {
        return;
    }

    bool accepted = false;
    const QString folderName = QInputDialog::getText(
        this,
        "New folder",
        "Folder name:",
        QLineEdit::Normal,
        "New folder",
        &accepted).trimmed();
    if (!accepted || folderName.isEmpty()) {
        return;
    }

    QString createdPath;
    if (!app_.createFolder(directoryPath, folderName, createdPath)) {
        showFileSystemError(
            "Unable to create folder",
            "The folder could not be created. Check its name, permissions, and whether it already exists.");
        return;
    }

    statusBar()->showMessage(QString("Created folder: %1").arg(folderName), 3000);
}

void MainWindow::restoreLastWorkspace()
{
    const QString lastPath = app_.settings().lastWorkspacePath();
    if (app_.settings().lastWorkspacePath().isEmpty()) {
        return;
    }

    loadWorkspace(lastPath);
}

void MainWindow::openWorkspace()
{
    const QString startPath = app_.settings().lastWorkspacePath().isEmpty()
        ? QDir::homePath()
        : app_.settings().lastWorkspacePath();

    const QString rootPath = QFileDialog::getExistingDirectory(
        this,
        "Open workspace",
        startPath,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (rootPath.isEmpty()) {
        return;
    }

    loadWorkspace(rootPath);
}

void MainWindow::loadWorkspace(const QString& rootPath)
{
    const QFileInfo info(rootPath);
    if (!info.exists() || !info.isDir()) {
        statusBar()->showMessage("Selected path is not a valid folder", 4000);
        return;
    }

    const QModelIndex rootIndex = fileModel_->setRootPath(rootPath);
    tree_->setModel(fileModel_);
    tree_->setRootIndex(rootIndex);
    tree_->hideColumn(1);
    tree_->hideColumn(2);
    tree_->hideColumn(3);
    tree_->sortByColumn(0, Qt::AscendingOrder);
    tree_->setVisible(true);

    app_.workspaceService().setCurrent(core::Workspace{rootPath, workspaceDisplayName(rootPath)});
    statusBar()->showMessage(QString("Workspace: %1").arg(QDir::toNativeSeparators(rootPath)));
}

void MainWindow::openItem(const QModelIndex& index)
{
    if (!index.isValid()) {
        return;
    }

    const QFileInfo info = fileModel_->fileInfo(index);
    if (info.isDir()) {
        return;
    }

    if (!app_.canOpenFileInEditor(info.filePath())) {
        statusBar()->showMessage("This file is visible in the workspace but only .txt/.md open in the editor", 5000);
        return;
    }

    openNoteFile(info.filePath());
}

bool MainWindow::maybeDeletePath(const QString& displayName, const bool isDirectory)
{
    QMessageBox box(this);
    box.setIcon(QMessageBox::Warning);
    box.setWindowTitle(isDirectory ? "Delete folder" : "Delete file");
    box.setText(QString("Move \"%1\" to the Recycle Bin?").arg(displayName));
    box.setInformativeText(isDirectory
        ? "The folder and all of its contents will be moved to the Recycle Bin."
        : "The file will be moved to the Recycle Bin.");
    box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    box.setDefaultButton(QMessageBox::No);

    const int answer = box.exec();
    return answer == QMessageBox::Yes;
}

void MainWindow::deletePath()
{
    const QModelIndex index = tree_->currentIndex();
    if (!index.isValid()) {
        return;
    }

    const QString path = fileModel_->filePath(index);
    if (pathsEqual(path, app_.workspaceService().current().rootPath)) {
        return;
    }
    const QString displayName = fileModel_->fileName(index);
    const bool isDirectory = fileModel_->isDir(index);
    if (!maybeDeletePath(displayName, isDirectory)) {
        return;
    }

    for (int i = 0; i < tabs_->count(); ++i) {
        auto* editor = qobject_cast<QPlainTextEdit*>(tabs_->widget(i));
        if (editor != nullptr
            && isSameOrChildPath(editor->property("filePath").toString(), path)
            && !maybeSaveEditor(editor)) {
            return;
        }
    }

    if (!app_.moveToTrash(path)) {
        showFileSystemError(
            isDirectory ? "Unable to delete folder" : "Unable to delete file",
            "The selected item could not be moved to the Recycle Bin. Check permissions and try again.");
        return;
    }

    for (int i = tabs_->count() - 1; i >= 0; --i) {
        QWidget* page = tabs_->widget(i);
        if (isSameOrChildPath(page->property("filePath").toString(), path)) {
            tabs_->removeTab(i);
            page->deleteLater();
        }
    }

    openWelcomeTab();
    updateTextCursor(qobject_cast<QPlainTextEdit*>(tabs_->currentWidget()));
    statusBar()->showMessage(QString("Moved to Recycle Bin: %1").arg(displayName), 3000);
}

void MainWindow::openFolderInExplorer()
{
    const QModelIndex index = tree_->currentIndex();
    if (!index.isValid()) {
        return;
    }

    const QFileInfo info(fileModel_->filePath(index));
    const QString folderPath = info.isDir() ? info.absoluteFilePath() : info.absolutePath();
    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(folderPath))) {
        statusBar()->showMessage("Unable to open the folder in Explorer", 4000);
    }
}

void MainWindow::showProperties()
{
    const QModelIndex index = tree_->currentIndex();
    if (!index.isValid()) {
        return;
    }

    const QFileInfo info(fileModel_->filePath(index));
    const QString type = info.isDir()
        ? "Folder"
        : (info.suffix().isEmpty() ? "File" : QString("%1 file").arg(info.suffix().toUpper()));

    QString details = QString(
        "Name: %1\n"
        "Type: %2\n"
        "Location: %3\n")
        .arg(info.fileName(), type, QDir::toNativeSeparators(info.absolutePath()));

    if (info.isFile()) {
        details += QString("Size: %1\n").arg(formatFileSize(info.size()));
    }

    details += QString(
        "Modified: %1\n"
        "Readable: %2\n"
        "Writable: %3")
        .arg(QLocale().toString(info.lastModified(), QLocale::ShortFormat),
             info.isReadable() ? "Yes" : "No",
             info.isWritable() ? "Yes" : "No");

    QMessageBox::information(this, "Properties", details);
}

void MainWindow::openNoteFile(const QString& filePath)
{
    for (int i = 0; i < tabs_->count(); ++i) {
        if (tabs_->widget(i)->property("filePath").toString() == filePath) {
            tabs_->setCurrentIndex(i);
            return;
        }
    }

    QString content;
    if (!app_.readTextFile(filePath, content)) {
        statusBar()->showMessage("Unable to open the selected file", 4000);
        return;
    }

    if (tabs_->count() == 1 && qobject_cast<PlaceholderWidget*>(tabs_->widget(0)) != nullptr) {
        QWidget* welcomeTab = tabs_->widget(0);
        tabs_->removeTab(0);
        welcomeTab->deleteLater();
    }

    auto* editor = new QPlainTextEdit(tabs_);
    editor->setPlainText(content);
    editor->document()->setModified(false);
    editor->setProperty("filePath", filePath);


    connect(editor->document(), &QTextDocument::modificationChanged, this, [this, editor](bool) {
        updateEditorTabTitle(editor);
    });


    const QFileInfo info(filePath);
    const int tabIndex = tabs_->addTab(editor, info.fileName());
    tabs_->setTabToolTip(tabIndex, QDir::toNativeSeparators(filePath));
    tabs_->setCurrentIndex(tabIndex);

    textInfo_->setText("column: 1, row: 1");
    textInfo_->setVisible(true);

    connect(editor, &QPlainTextEdit::cursorPositionChanged, this, [this, editor]() {
        updateTextCursor(editor);
    });

    statusBar()->showMessage(QString("Opened: %1").arg(QDir::toNativeSeparators(filePath)), 3000);
}

void MainWindow::renamePath()
{
    const QModelIndex index = tree_->currentIndex();
    if (!index.isValid()) {
        return;
    }

    const QString oldPath = fileModel_->filePath(index);
    if (pathsEqual(oldPath, app_.workspaceService().current().rootPath)) {
        return;
    }
    const QString currentName = fileModel_->fileName(index);
    const bool isDirectory = fileModel_->isDir(index);

    bool accepted = false;
    const QString newName = QInputDialog::getText(
        this,
        isDirectory ? "Rename folder" : "Rename file",
        isDirectory ? "New folder name:" : "New file name:",
        QLineEdit::Normal,
        currentName,
        &accepted);

    if (!accepted || newName.trimmed() == currentName) {
        return;
    }

    QString renamedPath;
    if (!app_.renamePath(oldPath, newName, renamedPath)) {
        showFileSystemError(
            isDirectory ? "Unable to rename folder" : "Unable to rename file",
            "The selected item could not be renamed. Check its name, permissions, and whether the target already exists.");
        return;
    }

    for (int i = 0; i < tabs_->count(); ++i) {
        QWidget* page = tabs_->widget(i);
        const QString openPath = page->property("filePath").toString();
        if (!isSameOrChildPath(openPath, oldPath)) {
            continue;
        }

        const QString updatedPath = renamedPath + openPath.mid(oldPath.size());
        page->setProperty("filePath", updatedPath);
        if (auto* editor = qobject_cast<QPlainTextEdit*>(page)) {
            updateEditorTabTitle(editor);
        }
        tabs_->setTabToolTip(i, QDir::toNativeSeparators(updatedPath));
    }

    statusBar()->showMessage(
        QString("Renamed: %1").arg(QDir::toNativeSeparators(renamedPath)),
        3000);
}

bool MainWindow::saveEditor(QPlainTextEdit* editor)
{
    if (editor == nullptr) return false;

    const QString filePath = editor->property("filePath").toString();
    if (!app_.canOpenFileInEditor(filePath)) return false;
    if (!app_.writeTextFile(filePath, editor->toPlainText())) return false;

    editor->document()->setModified(false);
    updateEditorTabTitle(editor);
    return true;
}

bool MainWindow::maybeSaveEditor(QPlainTextEdit* editor)
{
    if (editor == nullptr || !editor->document()->isModified()) return true;

    const QString filePath = editor->property("filePath").toString();
    const QString fileName = QFileInfo(filePath).fileName();

    QMessageBox box(this);
    box.setIcon(QMessageBox::Warning);
    box.setWindowTitle("Unsaved changes");
    box.setText(QString("File \"%1\" has unsaved changes.").arg(fileName));
    box.setInformativeText("Save before closing?");
    box.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    box.setDefaultButton(QMessageBox::Save);

    const int answer = box.exec();
    if (answer == QMessageBox::Save) return saveEditor(editor);
    if (answer == QMessageBox::Discard) return true;
    return false;
}

void MainWindow::updateTextCursor(QPlainTextEdit* editor) {
    if (editor == nullptr) {
        textInfo_->setText("column: 1, row: 1");
        textInfo_->setVisible(false);
        return;
    }

    textInfo_->setVisible(true);
    textInfo_->setText(QString("column: %1, row: %2")
        .arg(editor->textCursor().columnNumber() + 1)
        .arg(editor->textCursor().blockNumber() + 1));
}

bool MainWindow::closeTabAt(int index)
{
    if (index < 0 || index >= tabs_->count()) return false;

    QWidget* page = tabs_->widget(index);
    auto* editor = qobject_cast<QPlainTextEdit*>(page);

    if (editor != nullptr && !maybeSaveEditor(editor)) return false;

    tabs_->removeTab(index);
    page->deleteLater();
    openWelcomeTab();
    updateTextCursor(qobject_cast<QPlainTextEdit*>(tabs_->currentWidget()));
    return true;
}

void MainWindow::updateEditorTabTitle(QPlainTextEdit* editor)
{
    const int index = tabs_->indexOf(editor);
    if (index < 0) return;

    const QString filePath = editor->property("filePath").toString();
    const QString fileName = QFileInfo(filePath).fileName();
    const QString prefix = editor->document()->isModified() ? "*" : "";
    tabs_->setTabText(index, prefix + fileName);
}

void MainWindow::saveCurrentNote()
{
    auto* editor = qobject_cast<QPlainTextEdit*>(tabs_->currentWidget());
    if (editor == nullptr) {
        statusBar()->showMessage("No editable note is open", 3000);
        return;
    }

    const QString filePath = editor->property("filePath").toString();
    if (!app_.canOpenFileInEditor(filePath)) {
        statusBar()->showMessage("The current tab is not a supported note file", 3000);
        return;
    }
    if (!app_.writeTextFile(filePath, editor->toPlainText())) {
        statusBar()->showMessage("Unable to save the full file content", 4000);
        return;
    }

    editor->document()->setModified(false);
    statusBar()->showMessage(QString("Saved: %1").arg(QDir::toNativeSeparators(filePath)), 3000);
}

void MainWindow::openWelcomeTab()
{
    if (tabs_->count() == 0) {
        tabs_->addTab(new PlaceholderWidget("Open a workspace to browse notes", tabs_), "Welcome");
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    for (int i = tabs_->count() - 1; i >= 0; --i) {
        if (!closeTabAt(i)) {
            event->ignore();
            return;
        }
    }
    event->accept();
}

}
