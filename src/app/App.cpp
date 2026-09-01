#include "App.h"

#include "core/usecases/WorkspaceService.h"
#include "core/usecases/DocumentManager.h"
#include "infra/settings/Settings.h"
#include "network/StubNetworkClient.h"
#include "storage/FsFileRepository.h"

namespace app {

App::App() {
    fileRepo_ = std::make_unique<storage::FsFileRepository>();
    netClient_ = std::make_unique<network::StubNetworkClient>();
    settings_  = std::make_unique<infra::Settings>();
    workspaceService_ = std::make_unique<core::WorkspaceService>(*fileRepo_, *netClient_, *settings_);
    documentManager_ = std::make_unique<core::DocumentManager>();
}

App::~App() = default;

core::IWorkspaceService& App::workspaceService() { return *workspaceService_; }
core::DocumentManager& App::documentManager() { return *documentManager_; }
infra::Settings& App::settings() { return *settings_; }
bool App::canOpenFileInEditor(const QString& filePath) const { return fileRepo_->canOpenInEditor(filePath); }
bool App::readTextFile(const QString& filePath, QString& content) const { return fileRepo_->readTextFile(filePath, content); }
bool App::writeTextFile(const QString& filePath, const QString& content) { return fileRepo_->writeTextFile(filePath, content); }
bool App::writeTextFileAs(const QString& filePath, const QString& content) { return fileRepo_->writeTextFileAs(filePath, content); }
bool App::createNoteFile(const QString& directoryPath,
                         const QString& fileName,
                         QString& createdFilePath)
{
    return fileRepo_->createFile(directoryPath, fileName, createdFilePath);
}
bool App::createFolder(const QString& directoryPath,
                       const QString& folderName,
                       QString& createdFolderPath)
{
    return fileRepo_->createFolder(directoryPath, folderName, createdFolderPath);
}
bool App::moveToTrash(const QString& path) { return fileRepo_->moveToTrash(path); }
bool App::renamePath(const QString& path,
                     const QString& newName,
                     QString& renamedPath)
{
    return fileRepo_->renamePath(path, newName, renamedPath);
}

}
