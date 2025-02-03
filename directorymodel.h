#ifndef DIRECTORYMODEL_H
#define DIRECTORYMODEL_H

#include <QAbstractItemModel>
#include <QObject>
#include <QQmlEngine>

class DirectoryModel : public QAbstractItemModel
{
    Q_OBJECT
    QML_ELEMENT
public:
    DirectoryModel();
};

#endif // DIRECTORYMODEL_H
