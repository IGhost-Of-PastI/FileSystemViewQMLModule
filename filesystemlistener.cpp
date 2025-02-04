#include <iostream>  
#include <filesystem>
//#include "DirectoresTree.h"
#include <Windows.h>
#include <memory>
#include <locale>
#include <thread>
#include <format>
#include <functional>
#include <efsw/efsw.hpp>
#include "filesystemlistener.h"
#include <boost/describe.hpp>
#include <regex>
using namespace std;

std::string thisThreadIdToString() {
    std::ostringstream oss;
    oss << this_thread::get_id();
    return oss.str();
}

namespace efsw::Actions
{
    BOOST_DESCRIBE_ENUM(Action, Add, Delete, Modified, Moved)
}

int main() {
    using namespace std::filesystem;
    using namespace std;
    using namespace efsw;
      
    locale::global(locale(""));

    const path currentFolder = current_path().parent_path()/"x64"/"Debug";
    string formatedString = format("Главный поток процесса: {}", thisThreadIdToString());
    cout << formatedString << endl;

    std::function<void(efsw::Action, path, string, string)> onDirectoryOps = [](efsw::Action action,const path& path, string filename, string old_filename) {
        string action_str = boost::describe::enum_to_string<Action>(action, "unknown");
        cout << format("Поток исполнения обработки: {}, Данные {}: {} / {} >> {}", thisThreadIdToString(), action_str, path.string(), filename, old_filename) << endl;
        };

    //недостатки предиката он использует функцию которая просматривает файловую систему при событии удаления и и перемещения которые исполльзуеються для опознания старого файла уже нет поэтмоу она вернёт false
    std::function<bool(Action,const path&)> dirPredicate = [](Action action, const path& path) {
            switch (action)
            {
                case Action::Delete:
                {
                    std::regex pattern(R"(^[^.]+$)");
                    return std::regex_match(path.filename().string(), pattern);
                    break; 
                }
                default:
                {
                    return is_directory(path);
                }
            }  
        };

    const unique_ptr<FileWatcher> fileWatcher = make_unique<FileWatcher>();
    const unique_ptr<FileSystemListener> directoryListener = make_unique<FileSystemListener>();
    directoryListener->AddPredicateCallback(dirPredicate, onDirectoryOps);
    WatcherOption option(efsw::Options::Option::WinBufferSize, 64 * 1024);
    WatchID watchID = fileWatcher->addWatch(currentFolder.string(), directoryListener.get(), true, {option});
    fileWatcher->watch();

    bool isExecuted = false;
    while (true)
    {
        if (!isExecuted)
        {
            create_directory(currentFolder/"new");
            cout<<is_directory(currentFolder / "new")<<endl;
            cout << "Директория new создана" << endl;
            create_directory(currentFolder / "Новая");
            cout << "Директория Новая создана" << endl;
            rename(currentFolder / "new", currentFolder / "new2");
            cout << "Директория new переимнована в new2" << endl;
            rename(currentFolder / "Новая", currentFolder / "Новая2");
            cout << "Директория Новая переимнована в Новая2" << endl;
            remove_all(currentFolder/"new2");
            cout << "Директория new2 удалена" << endl;
            remove_all(currentFolder / "Новая2");
            cout << "Директория Новая2 удалена" << endl;

            isExecuted = true;
        }
        
    }
    //--Провести тесты с русскими названиями папок чтобы они были синхронизированы у вотчера и filesystem, вместо использования пути как строки используй путь файл система
    //Замечания по тестам
    /*
    У директории нет понятия модифайд у него есть понятие перемещения
    --Пока по наблюдениям наблюдать всегда видит изменения позже его применения но это не значит что обработчик будет вызван до того как информация о изменении посутп вотчеру
    --следвоательно в вотчера должен быть мультимап изменений перед внесением изменений к нему необходимо блокировать доступ, и при изменении записывать утда даныне и особождать, так гаранитровано алогритм ихзменений в директориях успеет оповестить наблюдателя об этом до того как он увидит изменение
    */
    
    //TO DO
    /*
   -- 1. Изменить сам класс ноды и его шаблон нет никакого смысла в применении дефолтныхз значений поскольку у классво и так в большинстве своём есть консткруторы по умолчанию
    2. Класс ноды должен обладать блокировками в методах связаных с пересозданием каталогов.
    3. Класс ноды должен обновляться а не пересоздаваться, поскольку directory_entry поддерживает реврешь, нужно лишь узнать поддерживате ли он рефреш при иззменении родительского каталога
    4. Класс наблюдатель, явно работает в другом потоке (проверить через this_thread.id) по этой причине ему нужно обеспечить безопасное обновление каталогов, также поскольку само приложение может менять каталоги
    то требуеться очередь где приложение вносит всё что оно сделало а при обработке изменений вотчер смотрит нет ли связаных с данным  типом изменения и с данным файлом (мульт индекс) (следует выяснить ктаже порядок вызова изменения и наблюдателя чтобы понятьк ак записывать адныне в очередь)
    5. Нода может быть возвращена из дерева но она обязательно должна быть указателем на константыне данные.
    6. (древовидная структура не труебуеться но относиться к ней) Требуеться иметь поиск именно файлов в дереве директорий, поиск можно осущестлветяь через recursive_directory итератор а потом по нужным файлам брать их родительский каталог, и упаковывать это все в список резхультатов поиска. 
    Примечение: естсвтенно могут попадаться одинаковые файлы, для игры это не так критично поскльку в файлах будут содержаться разнообразные шаблоны компонетов или объектов, они не являютсья лкассами а инсутркциями по их созданию, и они уже обладают уникальным путём.
    7. Сериализация данных каталогов и файлов
    */
    return 0;
}