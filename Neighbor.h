#ifndef NEIGHBOR_H
#define NEIGHBOR_H

#include <QObject>
#include <QString>
#include <QUrl>

class Neighbor : public QObject
{
public:
    Neighbor(QUrl url);
    Neighbor(const Neighbor &in);
    void operator=(const Neighbor &in);



private:
    QUrl m_url;
    unsigned int m_reliabilityNumber;
};

#endif // NEIGHBOR_H
