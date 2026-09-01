#pragma once

#include <QString>

namespace core {

struct DocumentSession final {
    QString filePath;
    QString text;
    bool modified = false;
};

}
