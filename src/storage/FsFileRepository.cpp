#include "FsFileRepository.h"
#include <QDir>

namespace storage {

QStringList FsFileRepository::listNoteFiles(const QString& rootPath)
{
    // v0.1: заглушка, позже добавим рекурсивный обход + фильтры .md/.txt
    QDir dir(rootPath);
    if (!dir.exists()) return {};

    const QStringList filters = {"*.txt", "*.md"};
    return dir.entryList(filters, QDir::Files, QDir::Name);
}

}
