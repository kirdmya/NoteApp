#pragma once
#include "storage/IFileRepository.h"

namespace storage {

class FsFileRepository final : public IFileRepository {
public:
    QStringList listNoteFiles(const QString& rootPath) override;
    bool canOpenInEditor(const QString& filePath) const override;
    bool readTextFile(const QString& filePath, QString& content) const override;
    bool writeTextFile(const QString& filePath, const QString& content) override;
    bool writeTextFileAs(const QString& filePath, const QString& content) override;
    bool createFile(const QString& directoryPath,
                    const QString& fileName,
                    QString& createdFilePath) override;
    bool createFolder(const QString& directoryPath,
                      const QString& folderName,
                      QString& createdFolderPath) override;
    bool moveToTrash(const QString& path) override;
    bool renamePath(const QString& path,
                    const QString& newName,
                    QString& renamedPath) override;
};

}
