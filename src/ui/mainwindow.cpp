#include "MainWindow.h"

#include <QAction>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QSplitter>
#include <QStatusBar>
#include <QTabWidget>
#include <QTextDocument>
#include <QTreeView>

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
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUiRuntime()
{
    auto* splitter = new QSplitter(this);

    tree_ = new QTreeView(splitter);
    tree_->setHeaderHidden(true);
    tree_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tree_->setSortingEnabled(true);

    fileModel_ = new QFileSystemModel(this);
    fileModel_->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);

    tabs_ = new QTabWidget(splitter);
    tabs_->addTab(new PlaceholderWidget("Open a workspace to browse notes", tabs_), "Welcome");

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 3);

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

    const QFileInfo info(filePath);
    const int tabIndex = tabs_->addTab(editor, info.fileName());
    tabs_->setTabToolTip(tabIndex, QDir::toNativeSeparators(filePath));
    tabs_->setCurrentIndex(tabIndex);

    statusBar()->showMessage(QString("Opened: %1").arg(QDir::toNativeSeparators(filePath)), 3000);
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

}
