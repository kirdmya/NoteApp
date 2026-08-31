#pragma once

#include <QMainWindow>

namespace app { class App; }

class QFileSystemModel;
class QLabel;
class QModelIndex;
class QTreeView;
class QTabWidget;
class QPlainTextEdit;

namespace ui {

class MainWindow final : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(app::App& app, QWidget* parent = nullptr);
    ~MainWindow() override;
protected:
    void closeEvent(QCloseEvent* event) override;
private:
    void setupUiRuntime();
    void setupActions();
    void connectSignals();
    void restoreLastWorkspace();
    void openWorkspace();
    void openWelcomeTab();
    void saveCurrentNote();

    void loadWorkspace(const QString& rootPath);
    void openItem(const QModelIndex& index);
    bool maybeDeleteFile(const QString& fileName);
    void deleteNote();
    void openNoteFile(const QString& filePath);
    void renameNote();
    void updateEditorTabTitle(QPlainTextEdit* editor);
    void updateTextCursor(QPlainTextEdit* editor);

    bool closeTabAt(int index);
    bool maybeSaveEditor(QPlainTextEdit* editor);
    bool saveEditor(QPlainTextEdit* editor);

    app::App& app_;
    QFileSystemModel* fileModel_ = nullptr;
    QTreeView*  tree_ = nullptr;
    QTabWidget* tabs_ = nullptr;
    QLabel* textInfo_ = nullptr;
};

}
