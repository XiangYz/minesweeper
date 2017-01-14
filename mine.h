#ifndef MINE_H
#define MINE_H

#include <QObject>
#include <QColor>

class Mine : public QObject
{
    Q_OBJECT
public:
    explicit Mine(QObject *parent = 0);
    virtual ~Mine();

    int GenerateMineColor(); // 给出数字颜色

public:

    QColor m_FontColor; // 若为数字，则存储颜色，雷用黑色
    int m_LogicX; // 逻辑横坐标
    int m_LogicY; // 逻辑纵坐标

    bool m_bMine; // 是否是雷
    int m_Number; // 若不是雷，则是数字几
    bool m_bDisplay; // 是否已经被翻开
    bool m_bMarked; // 是否被右键标记
    bool m_bFroze; // 0需要被冻结


public:
    bool operator ==(Mine &m);
    bool operator <(Mine &m);
    bool operator >(Mine &m);


signals:

public slots:
};

#endif // MINE_H
