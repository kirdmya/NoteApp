#pragma once
#include "storage/IFileRepository.h"

namespace storage {

class FsFileRepository final : public IFileRepository {
public:
    QStringList listNoteFiles(const QString& rootPath) override;
    bool canOpenInEditor(const QString& filePath) const override;
    bool readTextFile(const QString& filePath, QString& content) const override;
    bool writeTextFile(const QString& filePath, const QString& content) override;
    bool renameNote(const QString& filePath,
                    const QString& newFileName,
                    QString& renamedFilePath) override;
};

}
