#pragma once
#include <QWidget>

class QLabel;

namespace ui {

class PlaceholderWidget final : public QWidget {
    Q_OBJECT
public:
    explicit PlaceholderWidget(const QString& title, QWidget* parent = nullptr);

private:
    QLabel* label_ = nullptr;
};

}
