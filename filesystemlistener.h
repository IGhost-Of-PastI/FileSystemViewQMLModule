#pragma once
#include <efsw/efsw.hpp>
#include <functional>
#include <mutex>
#include <string>
#include <deque>
#include <unordered_map>
#include <filesystem>
#include <utility>
#include <list>
#include <boost/locale.hpp>

struct EventInfo
{
    std::filesystem::path dir;
    std::string filename;
    std::string old_filename;
    bool operator==(const EventInfo& other) const {
        return (dir == other.dir) && (filename == other.filename) && (old_filename == other.old_filename);
    }
};

using fscallback_t = std::function<void(efsw::Action, const std::filesystem::path&, std::string, std::string)>;
using fspath_predicate_t = std::function<bool(efsw::Action,const std::filesystem::path&)>;
using event_info_container_t = std::unordered_map<efsw::Action, std::deque<EventInfo>>;

class FileSystemWatcherEventProxy
{
public:
    FileSystemWatcherEventProxy(event_info_container_t& container, std::mutex& mutex)
        : _container(container), _guard(mutex) {
    }
    FileSystemWatcherEventProxy(FileSystemWatcherEventProxy&& other) noexcept
        : _container(other._container), _guard(std::move(other._guard)){}

    FileSystemWatcherEventProxy& operator=(FileSystemWatcherEventProxy&& other) noexcept
    {
        if (this != &other)
        {
            _container = other._container;
            _guard = std::move(other._guard);
        }
        return *this;
    }
    void AddEventInfo(efsw::Action action, EventInfo eventInfo) {
        _container[action].push_back(eventInfo);
    };
private:
    event_info_container_t& _container;
    std::unique_lock<std::mutex> _guard;
};

class FileSystemListener : public efsw::FileWatchListener
{
public:

private:   
    std::list<std::pair<fspath_predicate_t, fscallback_t>> _filesystemEntryCallback;
    event_info_container_t _event_container;
    std::mutex _access;

    inline void _OnFileSystemEvent(efsw::Action, const std::filesystem::path&, std::string, std::string);
public:
    FileSystemWatcherEventProxy&& Lock();
    void AddPredicateCallback(fspath_predicate_t,fscallback_t);
    void handleFileAction(efsw::WatchID watchid, const std::string& dir,
        const std::string& filename, efsw::Action action,
        std::string oldFilename) override;
};

