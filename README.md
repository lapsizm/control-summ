# File Checksum Utility

## Описание

Этот проект представляет собой утилиту командной строки для проверки и отображения информации о файлах и директориях, включая их размеры, права доступа, владельцев и контрольные суммы (MD5, SHA1, cksum). Программа также поддерживает создание и обновление файла с контрольными суммами.

## Зависимости

- [Boost Program Options](https://www.boost.org/doc/libs/release/libs/program_options/) - Для обработки аргументов командной строки.

## Функции

### Основные функции

- **`DoCKSUM(const std::string &p, const std::string& var)`**: Вычисляет контрольную сумму файла, используя указанную команду (md5, sha1, cksum).
- **`GetOwner(const std::string &p)`**: Получает владельца файла.
- **`Print(const fs::path &p, std::ostream &os)`**: Выводит информацию о файле или директории: имя, размер, время последнего изменения, права доступа, владелец и контрольные суммы.
- **`number_of_files_in_directory(std::filesystem::path path)`**: Подсчитывает количество файлов в указанной директории.
- **`CheckExistFile(fs::path path, std::string file)`**: Проверяет наличие файла в списке файлов.
- **`CheckCKSUM(const std::string &p, const std::string& var_sum)`**: Проверяет контрольные суммы файлов в указанном файле.

### Командная строка

Программа принимает следующие аргументы командной строки:

- **`--help`**: Выводит справку по использованию программы.
- **`--filename`**: Имя файла или директории для проверки или отображения информации.
- **`--to_file`**: Имя файла для записи информации.
- **`-c`**: Указывает, что нужно проверить контрольные суммы файлов.
- **`--from_file`**: Имя файла, содержащего список файлов и директорий для проверки.
- **`--var_sum`**: Определяет тип контрольной суммы (md5, sha1, cksum).

### Примеры использования

#### Отображение информации о файле или директории

```sh
./file_utility --filename <путь_к_файлу_или_директории> --to_file <путь_к_файлу_вывода>
```

#### Проверка контрольных сумм

```sh
./file_utility --filename <путь_к_файлу> -c --var_sum md5
```

#### Обновление контрольных сумм

```sh
./file_utility --from_file <путь_к_файлу_со_списком> --to_file <путь_к_файлу_для_обновления> --var_sum sha1
```

## Сборка и установка

1. Убедитесь, что у вас установлены Boost и необходимые библиотеки.
2. Создайте файл `CMakeLists.txt` для сборки, если его нет:

    ```cmake
    cmake_minimum_required(VERSION 3.10)
    project(FileChecksumUtility)

    set(CMAKE_CXX_STANDARD 17)

    find_package(Boost REQUIRED COMPONENTS program_options)

    add_executable(file_utility main.cpp)
    target_link_libraries(file_utility Boost::program_options)
    ```

3. Выполните следующие команды для сборки:

    ```sh
    mkdir build
    cd build
    cmake ..
    make
    ```
