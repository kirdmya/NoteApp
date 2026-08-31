#pragma once
#include <QWidget>

class QLabel; // почему просто не сделать #include <QLabel> ?
class QPushButton;

namespace ui {

class PlaceholderWidget final : public QWidget {
    Q_OBJECT
public:
    explicit PlaceholderWidget(const QString& title, QWidget* parent = nullptr);

private:
    QLabel* label_ = nullptr;
};

}
