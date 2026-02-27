#include "PlaceholderWidget.h"
#include <QVBoxLayout>
#include <QLabel>

namespace ui {

PlaceholderWidget::PlaceholderWidget(const QString& title, QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    label_ = new QLabel(title, this);
    label_->setAlignment(Qt::AlignCenter);
    layout->addWidget(label_);
}

}
