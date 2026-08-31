#include "FsFileRepository.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

namespace storage {

QStringList FsFileRepository::listNoteFiles(const QString& rootPath)
{
    QDir dir(rootPath);
    if (!dir.exists()) return {};

    const QStringList filters = {"*.txt", "*.md"};
    return dir.entryList(filters, QDir::Files, QDir::Name);
}

bool FsFileRepository::canOpenInEditor(const QString& filePath) const
{
    const QString suffix = QFileInfo(filePath).suffix();
    return suffix.compare("txt", Qt::CaseInsensitive) == 0
        || suffix.compare("md", Qt::CaseInsensitive) == 0;
}

bool FsFileRepository::readTextFile(const QString& filePath, QString& content) const
{
    if (!canOpenInEditor(filePath)) {
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    content = QString::fromUtf8(file.readAll());
    return true;
}

bool FsFileRepository::writeTextFile(const QString& filePath, const QString& content)
{
    if (!canOpenInEditor(filePath)) {
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        return false;
    }

    const QByteArray bytes = content.toUtf8();
    return file.write(bytes) == bytes.size();
}

bool FsFileRepository::renameNote(const QString& filePath,
                                  const QString& newFileName,
                                  QString& renamedFilePath)
{
    renamedFilePath.clear();

    const QFileInfo sourceInfo(filePath);
    if (!sourceInfo.exists() || !sourceInfo.isFile() || !canOpenInEditor(filePath)) {
        return false;
    }

    const QString normalizedName = newFileName.trimmed();
    if (normalizedName.isEmpty()
        || normalizedName == "."
        || normalizedName == ".."
        || QFileInfo(normalizedName).fileName() != normalizedName) {
        return false;
    }

    const QString destinationPath = sourceInfo.dir().filePath(normalizedName);
    if (!canOpenInEditor(destinationPath)) {
        return false;
    }

    if (QFileInfo(destinationPath).absoluteFilePath() == sourceInfo.absoluteFilePath()) {
        renamedFilePath = filePath;
        return true;
    }

    if (QFileInfo::exists(destinationPath) || !QFile::rename(filePath, destinationPath)) {
        return false;
    }

    renamedFilePath = destinationPath;
    return true;
}

}
