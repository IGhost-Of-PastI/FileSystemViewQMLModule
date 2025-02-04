#pragma once
#include<filesystem>
#include<unordered_map>
#include<memory>
#include<optional>
//#include<iostream>
//#include<cassert>

//class DirectoriesTree;

template<class D>
class DirectoryNode : public std::enable_shared_from_this<DirectoryNode<D>>
{
public:
    using childs_container_t = std::optional<std::unordered_map<std::string, std::shared_ptr<DirectoryNode>>>;
    using parent_ptr_t =  std::weak_ptr<DirectoryNode>;
    //friend typename F;
   // DirectoryNode() = default;
    DirectoryNode(const std::filesystem::path& folder, parent_ptr_t parent=parent_ptr_t());
    DirectoryNode* operator[](std::string folderName);

    DirectoryNode* operator[](int row);
    int childCount() const;
    int columnCount() const;
    D data(int column) const;
    int row() const;
    DirectoryNode *parentItem() const;

    //bool Rename(std::string newName);
    //bool CreateChild(std::string dirName);
    //bool DeleteChild(std::string childName);
    //DirectoryNode& operator=(D data);
private:
    void _LoadChilds();
    //void _Update();
    std::filesystem::directory_entry _currFolder;
    parent_ptr_t _parent;
    D _data;
    childs_container_t _childs;
};

template<class D>
inline DirectoryNode<D>* DirectoryNode<D>::operator[](int row)
{
    if (!_childs.has_value())
    {
        _LoadChilds();
    }
    if (row >= _childs.value().size()) {
        throw std::out_of_range("Индекс вне диапазона");
    }

    auto it = _childs.value().begin();

    for (size_t i = 0; i < row; ++i) {
        ++it;
    }

    return it->second.get();
}
template<class D>
inline int DirectoryNode<D>::childCount() const
{
//проверка инициализации
    return _childs.value().size();
}
template<class D>
inline int DirectoryNode<D>::columnCount() const
{
    return 0;
}
template<class D>
inline D DirectoryNode<D>::data(int column) const
{
    return _data;
}
template<class D>
inline int DirectoryNode<D>::row() const
{
    //неправильно нужен номре
    return 0;//_parent.lock()->_childs.value()[_currFolder.path().filename().string()];
}
template<class D>
inline DirectoryNode<D>* DirectoryNode<D>::parentItem() const
{
    //неправильно нужен номре
    return _parent.lock().get();
}

template<class D>
inline DirectoryNode<D>::DirectoryNode(const std::filesystem::path& folder, std::weak_ptr<DirectoryNode<D>> parent)
{
    _currFolder.assign(folder);
    _parent = parent;
}

template<class D>
inline DirectoryNode<D>* DirectoryNode<D>::operator[](std::string folderName)
{
    if (!_childs.has_value())
    {
        //std::unordered_map<std::string, std::shared_ptr<DirectoryNode>> map;
        //_childs = map;
        _LoadChilds();
    }
    return *_childs.value()[folderName];
}

template<class D>
inline void DirectoryNode<D>::_LoadChilds()
{
    using namespace std;
    using namespace std::filesystem;

    std::unordered_map<std::string, std::shared_ptr<DirectoryNode<D>>> childsMap;
    _childs = childsMap;
    const path currFolder = _currFolder.path();
    for (const directory_entry& entry : directory_iterator(currFolder))
    {
        if (entry.is_directory())
        {
            auto childNode = std::make_shared<DirectoryNode<D>>(entry.path(), this->weak_from_this());
            _childs.value().emplace(entry.path().filename().string(), childNode);
        }
    }
}
