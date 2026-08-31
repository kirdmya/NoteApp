#pragma once

#include <memory>
#include <QString>

namespace core { class IWorkspaceService; }
namespace storage { class IFileRepository; }
namespace network { class INetworkClient; }
namespace infra { class Settings; }

namespace app {

class App final {
public:
    App();
    ~App();

    core::IWorkspaceService& workspaceService();
    infra::Settings& settings();
    bool canOpenFileInEditor(const QString& filePath) const;
    bool readTextFile(const QString& filePath, QString& content) const;
    bool writeTextFile(const QString& filePath, const QString& content);
    bool renameNote(const QString& filePath,
                    const QString& newFileName,
                    QString& renamedFilePath);

private:
    std::unique_ptr<storage::IFileRepository> fileRepo_;
    std::unique_ptr<network::INetworkClient> netClient_;
    std::unique_ptr<infra::Settings> settings_;
    std::unique_ptr<core::IWorkspaceService> workspaceService_;
};

}
