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
    virtual bool deleteFile(const QString& filePath) = 0;
};

}
