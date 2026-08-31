## Стратегия коммита
---

### Ветки
main      — стабильная версия

develop   — текущая разработка

feature/* — новые фичи

fix/*     — исправления

refactor/* — рефакторинг


## Формат коммита (по Conventional Commits)
---
```
<type>(<scope>): <short description>
```


>[!important]
> Один коммит = одна логическая идея

#### Набор типов

feat
fix
refactor
docs
chore

#### Так делать НЕ надо
feat: add workspace + refactor storage + fix crash

#### Так делать надо

feat(workspace): add workspace switching

fix(storage): prevent deletion outside root

refactor(core): split WorkspaceService responsibilities

#### Если описание длинное - добавляй тело

feat(workspace): add workspace deletion

Add ability to delete workspace and move its files to trash.
Deletion is prevented if workspace is active.

#### Когда НЕ нужен scope

feat: initial project setup

#### Если поддержка чего-либо убирается - добавляй BREAKING CHANGE

feat(core): saving files doesn't support Windows 7

BREAKING CHANGE!: files' save doesn't support Windows 7 anymore


