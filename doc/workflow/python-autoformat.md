---
tags:
  - python
---

# Python Code AutoFormatting

```table-of-contents
minLevel: 2
```

## Описание

Этот документ описывает процесс настройки и использования **pre-commit** для проверки и автоформатирования Python кода.

## Полезные ссылки

- [Pre-commit](https://pre-commit.com/)
- [isort](https://pycqa.github.io/isort/)
- [Black](https://black.readthedocs.io/en/stable/)

## Настройка форматирования кода перед коммитом

### 1. Установка pre-commit

Убедитесь, что `Python` установлен в системе. Затем выполните команду:

```bash
pip install pre-commit
```

Также **pre-commit** может быть добавлен в `requirements.txt`, тогда он будет установлен при запуске команды:

```bash
pip install -r requirements.txt
```

### 2. Проверка наличия конфигурации

Pre-commit ищет файл конфигурации `.pre-commit-config.yaml` в корне проекта. Убедитесь, что файл существует. Если файл отсутствует - создайте его в корне проекта и добавьте конфигурацию, описанную ниже. Если файл уже существует, но в нем отсутствуют хуки для форматирования Python-кода (например, `isort`, `black`), добавьте их:

```yaml
repos:
  - repo: https://github.com/pycqa/isort
    rev: 5.13.2
    hooks:
      - id: isort
        args:
          [
            "--profile=black",
          ]
  - repo: https://github.com/psf/black
    rev: 24.10.0
    hooks:
      - id: black
        args: 
          [
            "--line-length=100",
            "--target-version=py39",
            "--target-version=py310",
            "--target-version=py311",
            "--target-version=py312",
          ]
```

В этом конфиге указаны хуки для форматирования Python-кода с использованием `isort` и `black`:
    - **isort** - утилита для сортировки импортов
    - **black** - утилита для форматирования кода, совместимая с PEP 8

### 3. Установка хуков pre-commit

После создания или проверки конфигурационного файла установите хуки:

```bash
pre-commit install
```

Теперь при каждом коммите `pre-commit` будет автоматически проверять и форматировать файлы в соответствии с указанными хуками.

`Pre-commit` запустит все хуки, указанные в `.pre-commit-config.yaml`, для всех файлов.

### 4. Пример работы

**Если код соответствует требованиям:**

`Pre-commit` не покажет ошибок, и коммит будет выполнен успешно.

**Если есть ошибки:**

`Pre-commit` выдаст список проблем, которые необходимо исправить. Например:

```bash
isort....................................................................Failed
- hook id: isort
- files were modified by this hook

Fixing lambda/iot-metrics/test_code.py

black....................................................................Failed
- hook id: black
- files were modified by this hook

reformatted lambda/iot-metrics/test_code.py
reformatted lambda/iot-metrics/main.py

All done! ✨ 🍰 ✨
2 files reformatted.
```

Необходимо добавить в stash исправленные файлы и снова выполнить коммит.

### 5. Обновление версий хуков

Если хуки устарели, их можно обновить до последних версий:

```bash
pre-commit autoupdate
```

Затем выполните:

```bash
pre-commit install
```

### 6. Полезные команды

```bash
pre-commit run --all-files  # запустить pre-commit для всех файлов в проекте
pre-commit clean            # очистить кэш pre-commit
pre-commit uninstall        # удаление pre-commit хуков
```

## Настройка форматирования кода при сохранении файлов (VS Code)

### 1. Настройка расширений для VS Code

В корне проекта можно создать или обновить файл `.vscode/extensions.json`, чтобы рекомендовать разработчикам установить необходимые расширения для форматирования и работы с Python-кодом:

```json
{
    "recommendations": [
        "ms-python.python",
        "ms-python.vscode-pylance",
        "ms-python.black-formatter",
        "ms-python.isort",
        "donjayamanne.python-environment-manager",
    ]
}
```

**Что это даёт?**

- При открытии проекта в VS Code разработчик увидит рекомендации по установке этих расширений.
- Это обеспечивает консистентное форматирование кода и упрощает настройку рабочей среды.

### 2. Настройка форматирования в VS Code

Настройки форматирования можно добавить в файл `.vscode/settings.json` для автоматического форматирования кода при сохранении и сортировки импортов. Пример:

```json
{
    "[python]": {
        "editor.defaultFormatter": "ms-python.black-formatter",
        "editor.formatOnSave": true,
        "editor.codeActionsOnSave": {
        "source.organizeImports": "always"
        },
    },
    "black-formatter.args": ["--line-length", "100", "--target-version", "py39", "--target-version", "py310", "--target-version", "py311", "--target-version", "py312"],
    "isort.args": ["--profile", "black"]
}
```

Описание настроек:

- `"editor.defaultFormatter": "ms-python.black-formatter"` — использование Black как форматтера по умолчанию.
- `"editor.formatOnSave": true` — автоматическое форматирование кода при сохранении.
- `"editor.codeActionsOnSave": {"source.organizeImports": "always"}` — автоматическая сортировка импортов при сохранении.
- `"black-formatter.args": ["--line-length", "100"]` — настройка длины строки для Black.
- `"isort.args": ["--profile", "black"]` — настройка совместимости с Black для isort.

### 3. Пример работы в связке с pre-commit и VS Code

- При сохранении файла в `VS Code` форматирование и сортировка импортов выполняются автоматически.
- Если включены хуки `pre-commit`, то код проверяется и доформатируется в соответствии с конфигурацией .`pre-commit-config.yaml`.
- Это обеспечивает единообразие стиля кода и автоматизирует его форматирование.
