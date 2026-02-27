#include "MainWindow.h"

#include <QMessageBox>
#include <QTreeView>
#include <QTabWidget>
#include <QSplitter>
#include <QStatusBar>
#include <QMenuBar>
#include <QAction>

#include "ui/widgets/PlaceholderWidget.h"
#include "ui/actions/ActionIds.h"
#include "app/AppVersion.h"
#include "app/App.h"

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

    tabs_ = new QTabWidget(splitter);
    tabs_->addTab(new PlaceholderWidget("Editor placeholder", tabs_), "Welcome");

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
    connect(save, &QAction::triggered, this, [this](){
        statusBar()->showMessage("Save: TODO (hotkeys later)", 2000);
    });

    auto* openWorkspace = findChild<QAction*>(ui::actions::kOpenWorkspace);
    connect(openWorkspace, &QAction::triggered, this, [this](){
        statusBar()->showMessage("Open workspace: TODO", 2000);
    });
}

}
