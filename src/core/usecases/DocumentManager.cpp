#include "DocumentManager.h"

#include <QDir>

namespace {

Qt::CaseSensitivity pathCaseSensitivity()
{
#ifdef Q_OS_WIN
    return Qt::CaseInsensitive;
#else
    return Qt::CaseSensitive;
#endif
}

bool pathsEqual(const QString& left, const QString& right)
{
    return QDir::cleanPath(left).compare(
        QDir::cleanPath(right), pathCaseSensitivity()) == 0;
}

bool isSameOrChildPath(const QString& path, const QString& rootPath)
{
    const QString cleanPath = QDir::cleanPath(path);
    QString cleanRoot = QDir::cleanPath(rootPath);
    if (!cleanRoot.endsWith('/')) {
        cleanRoot += '/';
    }

    return pathsEqual(cleanPath, rootPath)
        || cleanPath.startsWith(cleanRoot, pathCaseSensitivity());
}

}

namespace core {

const QVector<DocumentSession>& DocumentManager::sessions() const
{
    return sessions_;
}

DocumentSession* DocumentManager::find(const QString& filePath)
{
    for (DocumentSession& session : sessions_) {
        if (pathsEqual(session.filePath, filePath)) {
            return &session;
        }
    }
    return nullptr;
}

const DocumentSession* DocumentManager::find(const QString& filePath) const
{
    for (const DocumentSession& session : sessions_) {
        if (pathsEqual(session.filePath, filePath)) {
            return &session;
        }
    }
    return nullptr;
}

DocumentSession& DocumentManager::open(const QString& filePath, const QString& text)
{
    if (DocumentSession* existing = find(filePath)) {
        return *existing;
    }

    sessions_.append(DocumentSession{filePath, text, false});
    return sessions_.last();
}

void DocumentManager::update(const QString& filePath,
                             const QString& text,
                             const bool modified)
{
    if (DocumentSession* session = find(filePath)) {
        session->text = text;
        session->modified = modified;
    }
}

bool DocumentManager::close(const QString& filePath)
{
    for (int i = 0; i < sessions_.size(); ++i) {
        if (pathsEqual(sessions_[i].filePath, filePath)) {
            sessions_.removeAt(i);
            return true;
        }
    }
    return false;
}

int DocumentManager::closeUnder(const QString& rootPath)
{
    int closed = 0;
    for (int i = sessions_.size() - 1; i >= 0; --i) {
        if (isSameOrChildPath(sessions_[i].filePath, rootPath)) {
            sessions_.removeAt(i);
            ++closed;
        }
    }
    return closed;
}

int DocumentManager::remapPath(const QString& oldPath, const QString& newPath)
{
    int remapped = 0;
    for (DocumentSession& session : sessions_) {
        if (isSameOrChildPath(session.filePath, oldPath)) {
            session.filePath = newPath + session.filePath.mid(oldPath.size());
            ++remapped;
        }
    }
    return remapped;
}

}
