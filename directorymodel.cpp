#include "directorymodel.h"

DirectoryModel::DirectoryModel(QObject *parent)
    : QAbstractItemModel(parent)
    //, rootItem(std::make_unique<TreeItem>(QVariantList{tr("Title"), tr("Summary")}))
{
    //setupModelData(QStringView{data}.split(u'\n'), rootItem.get());
}
DirectoryModel::~DirectoryModel() = default;

QModelIndex DirectoryModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return {};

    Node_t* parentItem = parent.isValid() ? static_cast<Node_t *>(parent.internalPointer()): rootItem.get();

    if (auto *childItem = (*parentItem)[row])
        return createIndex(row, column, childItem);
    return {};
}
QModelIndex DirectoryModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return {};

    Node_t* childItem = static_cast<Node_t *>(index.internalPointer());
    Node_t* parentItem = childItem->parentItem();

    return parentItem != rootItem.get() ? createIndex(parentItem->row(), 0, parentItem) : QModelIndex{};
}
int DirectoryModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    const Node_t* parentItem = parent.isValid() ? static_cast<const Node_t*>(parent.internalPointer()): rootItem.get();

    return parentItem->childCount();
}
int DirectoryModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return static_cast<Node_t*>(parent.internalPointer())->columnCount();
    return rootItem->columnCount();
}
QVariant DirectoryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return {};

    const auto *item = static_cast<const Node_t*>(index.internalPointer());
    return item->data(index.column());
}
Qt::ItemFlags DirectoryModel::flags(const QModelIndex &index) const
{
    return index.isValid()
    ? QAbstractItemModel::flags(index) : Qt::ItemFlags(Qt::NoItemFlags);
}
QVariant DirectoryModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    return orientation == Qt::Horizontal && role == Qt::DisplayRole
               ? rootItem->data(section) : QVariant{};
}
