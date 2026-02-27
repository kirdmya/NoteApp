#pragma once
#include "storage/IFileRepository.h"

namespace storage {

class FsFileRepository final : public IFileRepository {
public:
    QStringList listNoteFiles(const QString& rootPath) override;
};

}
