#ifndef DIRECTORYMODEL_H
#define DIRECTORYMODEL_H

#include <QAbstractItemModel>
#include <QObject>
#include <QQmlEngine>
#include "directorynode.h"
#include <memory>

//struct ModelData : public QObject
//{
//    Q_OBJECT
//    int d;
//};

class DirectoryModel : public QAbstractItemModel
{
    Q_OBJECT
    QML_ELEMENT
public:
    using Node_t = DirectoryNode<QVariantList>;
    using Node_ptr_t =std::shared_ptr<Node_t>;
    explicit DirectoryModel(QObject *parent = nullptr);
    ~DirectoryModel() override;

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = {}) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;

private:
    //static void setupModelData(const QList<QStringView> &lines, TreeItem *parent);
    //С одной стороны понятно почему здесь используеться уникальный указатель всеже в нодах ценны не сами они а даныне в них и ноды из вне не должны быть доступны
    Node_ptr_t rootItem;
};

#endif // DIRECTORYMODEL_H
