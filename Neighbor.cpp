#include "Neighbor.h"

Neighbor::Neighbor(QUrl url)
    :QObject(),
      m_url(url),
      m_reliabilityNumber(1000)
{
}

Neighbor::Neighbor(const Neighbor &in)
    :QObject(),
      m_url(in.m_url),
      m_reliabilityNumber(in.m_reliabilityNumber)
{

}

void Neighbor::operator=(const Neighbor &in)
{
    m_url = in.m_url;
    m_reliabilityNumber = in.m_reliabilityNumber;
}
