#include "App.h"

#include "core/usecases/WorkspaceService.h"
#include "infra/settings/Settings.h"
#include "network/StubNetworkClient.h"
#include "storage/FsFileRepository.h"

namespace app {

App::App() {
    fileRepo_ = std::make_unique<storage::FsFileRepository>();
    netClient_ = std::make_unique<network::StubNetworkClient>();
    settings_  = std::make_unique<infra::Settings>();
    workspaceService_ = std::make_unique<core::WorkspaceService>(*fileRepo_, *netClient_, *settings_);
}

App::~App() = default;

core::IWorkspaceService& App::workspaceService() { return *workspaceService_; }
infra::Settings& App::settings() { return *settings_; }
storage::IFileRepository& App::fileRepository() { return *fileRepo_; }
bool App::deleteFile(const QString& filePath) { return fileRepo_->deleteFile(filePath); }
bool App::canOpenFileInEditor(const QString& filePath) const { return fileRepo_->canOpenInEditor(filePath); }
bool App::readTextFile(const QString& filePath, QString& content) const { return fileRepo_->readTextFile(filePath, content); }
bool App::writeTextFile(const QString& filePath, const QString& content) { return fileRepo_->writeTextFile(filePath, content); }

}
