# Модули приложения

## Схема зависимостей

```mermaid
flowchart TB
    main["Точка входа<br/><code>src/app/main.cpp</code>"]

    subgraph presentation["Представление"]
        ui["UI<br/><code>src/ui</code><br/>MainWindow, виджеты и действия"]
    end

    subgraph composition["Композиция приложения"]
        app["App<br/><code>src/app</code><br/>создание и связывание зависимостей"]
    end

    subgraph business["Бизнес-логика"]
        core["Core<br/><code>src/core</code><br/>домен и сценарии использования"]
    end

    subgraph adapters["Инфраструктура и адаптеры"]
        storage["Storage<br/><code>src/storage</code><br/>работа с файлами"]
        network["Network<br/><code>src/network</code><br/>сетевой клиент"]
        infra["Infra<br/><code>src/infra</code><br/>настройки и журналирование"]
    end

    qt["Qt 6 Widgets"]
    fs[("Файловая система")]
    settings[("Системное хранилище<br/>настроек")]

    main --> app
    main --> ui
    ui -->|использует фасад и сервисы| app
    app -->|создаёт| core
    app -->|внедряет реализации| storage
    app -->|внедряет реализации| network
    app -->|создаёт| infra
    core -.->|IFileRepository| storage
    core -.->|INetworkClient| network
    core -.->|Settings| infra
    ui --> qt
    storage --> fs
    infra --> settings
```

Сплошные стрелки показывают прямое использование или создание объектов, пунктирные — зависимости бизнес-логики, передаваемые через конструктор.

## Назначение модулей

| Модуль | Ответственность | Основные элементы |
|---|---|---|
| `app` | Запуск приложения и сборка графа объектов | `main`, `App`, `AppVersion` |
| `ui` | Окна, виджеты, действия пользователя и отображение данных | `MainWindow`, `PlaceholderWidget`, `ActionIds` |
| `core/domain` | Доменные типы без логики представления | `Workspace`, `NoteId` |
| `core/usecases` | Сценарии использования приложения | `IWorkspaceService`, `WorkspaceService` |
| `storage` | Абстракция и реализация доступа к файлам | `IFileRepository`, `FsFileRepository` |
| `network` | Абстракция сетевого доступа и временная реализация | `INetworkClient`, `StubNetworkClient` |
| `infra` | Общие инфраструктурные службы | `Settings`, `Logger` |

## Правила зависимостей

- `app` является корнем композиции: создаёт конкретные реализации и передаёт их потребителям.
- `ui` получает доступ к возможностям приложения через `app::App` и интерфейсы сервисов.
- `core` содержит бизнес-логику и не создаёт объекты `storage`, `network` или `infra` самостоятельно.
- `storage` и `network` отделяют интерфейсы от конкретных реализаций, чтобы реализации можно было заменять.
- Доменные типы из `core/domain` не должны зависеть от UI и инфраструктурных деталей.
