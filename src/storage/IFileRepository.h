#pragma once
#include <QStringList>
#include <QString>

namespace storage {

class IFileRepository {
public:
    virtual ~IFileRepository() = default;

    virtual QStringList listNoteFiles(const QString& rootPath) = 0;
    virtual bool canOpenInEditor(const QString& filePath) const = 0;
    virtual bool readTextFile(const QString& filePath, QString& content) const = 0;
    virtual bool writeTextFile(const QString& filePath, const QString& content) = 0;
    virtual bool createFile(const QString& directoryPath,
                            const QString& fileName,
                            QString& createdFilePath) = 0;
    virtual bool createFolder(const QString& directoryPath,
                              const QString& folderName,
                              QString& createdFolderPath) = 0;
    virtual bool moveToTrash(const QString& path) = 0;
    virtual bool renamePath(const QString& path,
                            const QString& newName,
                            QString& renamedPath) = 0;
};

}
