#include "mine.h"

Mine::Mine(QObject *parent) : QObject(parent)
{
    m_Number = 0;
    m_FontColor = QColor(0, 0, 0);
    m_LogicX = 0;
    m_LogicY = 0;

    m_bMine = false;
    m_bDisplay = false;
    m_bMarked = false;
    m_bFroze = false;
}

Mine::~Mine()
{

}

bool Mine::operator ==(Mine &m)
{
    if (this->m_LogicX == m.m_LogicX
            && this->m_LogicY == m.m_LogicY)
    {
        return true;
    }

    return false;
}

bool Mine::operator <(Mine &m)
{
    if (this->m_LogicY < m.m_LogicY)
    {
        return true;
    }
    else if (this->m_LogicY == m.m_LogicY)
    {
        return this->m_LogicX < m.m_LogicX;
    }

    return false;
}

bool Mine::operator >(Mine &m)
{
    if (this->m_LogicY > m.m_LogicY)
    {
        return true;
    }
    else if (this->m_LogicY == m.m_LogicY)
    {
        return this->m_LogicX > m.m_LogicX;
    }


    return false;
}


int Mine::GenerateMineColor()
{
    switch(m_Number)
    {
    case 0:
    {
        m_FontColor = QColor(0, 0, 0);
    }
        break;
    case 1:
    {
        m_FontColor = QColor(200, 0, 0);
    }
        break;
    case 2:
    {
        m_FontColor = QColor(200, 150, 0);
    }
        break;
    case 3:
    {
        m_FontColor = QColor(0, 0, 200);
    }
        break;
    case 4:
    {
        m_FontColor = QColor(200, 0, 50);
    }
        break;
    case 5:
    {
        m_FontColor = QColor(0, 150, 200);
    }
        break;
    case 6:
    {
        m_FontColor = QColor(50, 200, 0);
    }
        break;
    default:
    {
        m_FontColor = QColor(10, 10, 10);
    }
        break;
    }

    return 1;
}


