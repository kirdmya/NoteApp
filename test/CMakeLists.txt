find_package(Qt6 REQUIRED COMPONENTS Widgets Test)

add_executable(ORangeTests
    test_main.cpp

    ../src/app/App.h
    ../src/app/App.cpp
    ../src/app/AppVersion.h

    ../src/core/domain/Workspace.h
    ../src/core/domain/DocumentSession.h
    ../src/core/usecases/DocumentManager.h
    ../src/core/usecases/DocumentManager.cpp
    ../src/core/usecases/IWorkspaceService.h
    ../src/core/usecases/WorkspaceService.h
    ../src/core/usecases/WorkspaceService.cpp

    ../src/storage/IFileRepository.h
    ../src/storage/FsFileRepository.h
    ../src/storage/FsFileRepository.cpp

    ../src/network/INetworkClient.h
    ../src/network/StubNetworkClient.h
    ../src/network/StubNetworkClient.cpp

    ../src/infra/settings/Settings.h
    ../src/infra/settings/Settings.cpp

    ../src/ui/mainwindow.h
    ../src/ui/mainwindow.cpp
    ../src/ui/actions/ActionIds.h
    ../src/ui/widgets/PlaceholderWidget.h
    ../src/ui/widgets/PlaceholderWidget.cpp
)

target_include_directories(ORangeTests PRIVATE ../src)
target_link_libraries(ORangeTests PRIVATE Qt6::Widgets Qt6::Test)

add_test(NAME ORangeTests COMMAND ORangeTests)
