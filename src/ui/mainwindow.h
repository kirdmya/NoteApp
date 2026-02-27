#pragma once

#include <QMainWindow>

namespace app { class App; }

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

    app::App& app_;

    QTreeView*  tree_ = nullptr;
    QTabWidget* tabs_ = nullptr;
};

}
