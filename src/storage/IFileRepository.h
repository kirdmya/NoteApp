#pragma once
#include <QStringList>
#include <QString>

namespace storage {

class IFileRepository {
public:
    virtual ~IFileRepository() = default;

    virtual QStringList listNoteFiles(const QString& rootPath) = 0;
};

}
