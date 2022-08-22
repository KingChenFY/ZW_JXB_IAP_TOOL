#ifndef IAPACTION_H
#define IAPACTION_H

#include <QObject>

class IapAction : public QObject
{
    Q_OBJECT
public:
    explicit IapAction(QObject *parent = nullptr);

signals:

};

#endif // IAPACTION_H
