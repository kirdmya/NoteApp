cmake_minimum_required(VERSION 3.21)
project(NoteApp VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options / warnings (заготовки на будущее)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(options OPTIONAL)
include(warnings OPTIONAL)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(NoteApp
    src/app/main.cpp
    src/app/App.h
    src/app/App.cpp
    src/app/AppVersion.h

    src/ui/MainWindow.h
    src/ui/MainWindow.cpp
    src/ui/MainWindow.ui

    src/ui/actions/ActionIds.h
    src/ui/widgets/PlaceholderWidget.h
    src/ui/widgets/PlaceholderWidget.cpp

    src/core/domain/Workspace.h
    src/core/domain/NoteId.h
    src/core/usecases/IWorkspaceService.h
    src/core/usecases/WorkspaceService.h
    src/core/usecases/WorkspaceService.cpp

    src/storage/IFileRepository.h
    src/storage/FsFileRepository.h
    src/storage/FsFileRepository.cpp

    src/network/INetworkClient.h
    src/network/StubNetworkClient.h
    src/network/StubNetworkClient.cpp

    src/infra/logging/Logger.h
    src/infra/logging/Logger.cpp
    src/infra/settings/Settings.h
    src/infra/settings/Settings.cpp
)

target_link_libraries(NoteApp PRIVATE Qt6::Widgets)

# Чтобы пути include были удобнее
target_include_directories(NoteApp PRIVATE src)
option(NOTEAPP_BUILD_TESTS "Build NoteApp tests" ON)

if (NOTEAPP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
