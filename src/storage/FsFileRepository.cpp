#include "FsFileRepository.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

namespace {

QString normalizedEntryName(const QString& name)
{
    const QString normalized = name.trimmed();
    if (normalized.isEmpty()
        || normalized == "."
        || normalized == ".."
        || QFileInfo(normalized).fileName() != normalized) {
        return {};
    }
    return normalized;
}

}

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

bool FsFileRepository::createFile(const QString& directoryPath,
                                  const QString& fileName,
                                  QString& createdFilePath)
{
    createdFilePath.clear();

    const QDir directory(directoryPath);
    const QString normalizedName = normalizedEntryName(fileName);
    if (!directory.exists() || normalizedName.isEmpty()) {
        return false;
    }

    const QString targetPath = directory.filePath(normalizedName);
    if (!canOpenInEditor(targetPath) || QFileInfo::exists(targetPath)) {
        return false;
    }

    QFile file(targetPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::NewOnly | QIODevice::Text)) {
        return false;
    }

    file.close();
    createdFilePath = targetPath;
    return true;
}

bool FsFileRepository::createFolder(const QString& directoryPath,
                                    const QString& folderName,
                                    QString& createdFolderPath)
{
    createdFolderPath.clear();

    QDir directory(directoryPath);
    const QString normalizedName = normalizedEntryName(folderName);
    if (!directory.exists() || normalizedName.isEmpty()) {
        return false;
    }

    const QString targetPath = directory.filePath(normalizedName);
    if (QFileInfo::exists(targetPath) || !directory.mkdir(normalizedName)) {
        return false;
    }

    createdFolderPath = targetPath;
    return true;
}

bool FsFileRepository::moveToTrash(const QString& path)
{
    const QFileInfo info(path);
    return info.exists() && QFile::moveToTrash(path);
}

bool FsFileRepository::renamePath(const QString& path,
                                  const QString& newName,
                                  QString& renamedPath)
{
    renamedPath.clear();

    const QFileInfo sourceInfo(path);
    const QString normalizedName = normalizedEntryName(newName);
    if (!sourceInfo.exists() || normalizedName.isEmpty()) {
        return false;
    }

    const QString destinationPath = sourceInfo.dir().filePath(normalizedName);
    if (sourceInfo.isFile() && !canOpenInEditor(destinationPath)) {
        return false;
    }

    if (QFileInfo(destinationPath).absoluteFilePath() == sourceInfo.absoluteFilePath()) {
        renamedPath = path;
        return true;
    }

    if (QFileInfo::exists(destinationPath)) {
        return false;
    }

    const bool renamed = sourceInfo.isDir()
        ? QDir().rename(path, destinationPath)
        : QFile::rename(path, destinationPath);
    if (!renamed) {
        return false;
    }

    renamedPath = destinationPath;
    return true;
}

}
