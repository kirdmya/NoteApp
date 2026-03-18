#pragma once

#include <QMainWindow>

namespace app { class App; }

class QFileSystemModel;
class QModelIndex;
class QTreeView;
class QTabWidget;

namespace ui {

class MainWindow final : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(app::App& app, QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    void setupUiRuntime();
    void setupActions();
    void connectSignals();
    void restoreLastWorkspace();
    void openWorkspace();
    void loadWorkspace(const QString& rootPath);
    void openItem(const QModelIndex& index);
    void openNoteFile(const QString& filePath);
    void saveCurrentNote();

    app::App& app_;

    QFileSystemModel* fileModel_ = nullptr;
    QTreeView*  tree_ = nullptr;
    QTabWidget* tabs_ = nullptr;
};

}
