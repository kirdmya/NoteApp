#include "FsFileRepository.h"
#include <QDir>

namespace storage {

QStringList FsFileRepository::listNoteFiles(const QString& rootPath)
{
    QDir dir(rootPath);
    if (!dir.exists()) return {};

    const QStringList filters = {"*.txt", "*.md"};
    return dir.entryList(filters, QDir::Files, QDir::Name);
}

}
