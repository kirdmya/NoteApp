#pragma once

#include <QVector>

#include "core/domain/DocumentSession.h"

namespace core {

class DocumentManager final {
public:
    const QVector<DocumentSession>& sessions() const;

    DocumentSession* find(const QString& filePath);
    const DocumentSession* find(const QString& filePath) const;
    DocumentSession& open(const QString& filePath, const QString& text);
    void update(const QString& filePath, const QString& text, bool modified);
    bool close(const QString& filePath);
    int closeUnder(const QString& rootPath);
    int remapPath(const QString& oldPath, const QString& newPath);

private:
    QVector<DocumentSession> sessions_;
};

}
