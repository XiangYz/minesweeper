#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QTime>
#include <QMouseEvent>
#include <QString>
#include <QMessageBox>

#include "mine.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowMaximizeButtonHint);

    qsrand(QTime(0, 0, 0).msecsTo(QTime::currentTime()));

    StartNewGame(10, 10, 10);

    //connect(this, SIGNAL(MousePressInMineArea(QPoint,Qt::MouseButton)), this, SLOT(HandleMineShow(QPoint,Qt::MouseButton)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    // 刷新显示
    LayoutMineUI(&painter);

}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_timerID)
    {
        m_SecCnt++;
    }

    return;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (m_bOver)
    {
        return;
    }

    if (m_bStart && m_bPause)
    {
        return;
    }

    if (!m_bStart)
    {
        m_bStart = true;

        // 开启定时器
        m_timerID = startTimer(1000);
    }

    if (event->pos().x() < m_rectMineArea.left()
            || event->pos().x() > m_rectMineArea.right()
            || event->pos().y() < m_rectMineArea.top()
            || event->pos().y() > m_rectMineArea.bottom())
    {
        return;
    }

    int iLogicX = (event->pos().x() - m_rectMineArea.left()) / m_SizePerMine;
    int iLogicY = (event->pos().y() - m_rectMineArea.top()) / m_SizePerMine;

    int iXLeftPixs = (event->pos().x() - m_rectMineArea.left()) % m_SizePerMine;
    int iYLeftPixs = (event->pos().y() - m_rectMineArea.top()) % m_SizePerMine;

    if (iXLeftPixs <= 2 || iXLeftPixs >= m_SizePerMine - 2)
    {
        return;
    }

    if (iYLeftPixs <= 2 || iYLeftPixs >= m_SizePerMine - 2)
    {
        return;
    }

    //emit MousePressInMineArea(QPoint(iLogicX, iLogicY), event->button());
    int iGameResult = HandleMineShow(QPoint(iLogicX, iLogicY), event->button());

    this->update();

    if (iGameResult == 0)
    {
        return;
    }

    m_bOver = true;
    killTimer(m_timerID);

    if (iGameResult > 0)
    {
        QString sWinInfo
                = QString("You win: %1s").arg(m_SecCnt);
        QMessageBox::information(this, ":)", sWinInfo);
    }
    else if (iGameResult < 0)
    {
        QMessageBox::information(this, ":(", "You lose");
    }
}

int MainWindow::StartNewGame(int xcnt, int ycnt, int MinesCnt)
{
    // 规定总方块数
    m_XCnt = xcnt;
    m_YCnt = ycnt;
    m_ContainersCnt = m_XCnt * m_YCnt;

    if (MinesCnt > m_ContainersCnt)
    {
        return -1;
    }
    // 规定雷数
    m_MinesCnt = MinesCnt;
    m_MinesFound = 0;

    // 每个方块的size，该值需要计算出来
    m_SizePerMine = 0;

    //
    m_rectMineArea = QRect(0, 0, 0, 0);

    // 清空方块数据
    m_vecContainers.clear();

    m_bOver = false;
    m_bStart = false;
    m_bPause = false;

    m_timerID = -1;
    m_SecCnt = 0;

    // 布雷
    LayoutMines();

    return 1;
}

bool MainWindow::CompareMinePosition(Mine *mineA, Mine *mineB)
{
    if (*mineA < *mineB)
    {
        return true;
    }
    else if (*mineA == *mineB)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int MainWindow::LayoutMines()
{
    m_bOver = false;

    m_ContainersCnt = m_XCnt * m_YCnt;

    QVector<int> posTmpVec;
    posTmpVec.clear();

    int iMinesCnt = m_MinesCnt;
    while(iMinesCnt--)
    {
        int iPos;

        do
        {
            iPos = qrand() % (m_XCnt * m_YCnt);
        }
        while (posTmpVec.contains(iPos));

        posTmpVec.push_back(iPos);

        int iYPos = iPos / m_XCnt;
        int iXPos = iPos % m_XCnt;

        Mine *newMine = new Mine();
        newMine->m_bMine = true;
        newMine->m_LogicX = iXPos;
        newMine->m_LogicY = iYPos;
        newMine->m_Number = -1;
        newMine->m_FontColor = newMine->GenerateMineColor();

        m_vecContainers.append(newMine);
    }

    int indexOfWholeArea = 0;
    for (indexOfWholeArea = 0
         ; indexOfWholeArea < m_XCnt * m_YCnt
         ; indexOfWholeArea++ )
    {
        if (posTmpVec.contains(indexOfWholeArea))
        {
            continue;
        }

        int iYPos = indexOfWholeArea / m_XCnt;
        int iXPos = indexOfWholeArea % m_XCnt;

        Mine *newMine = new Mine();
        newMine->m_bMine = false;
        newMine->m_LogicX = iXPos;
        newMine->m_LogicY = iYPos;

        newMine->m_Number = -1;

        m_vecContainers.append(newMine);

    }

    qSort(m_vecContainers.begin(), m_vecContainers.end(), CompareMinePosition);

    for (indexOfWholeArea = 0
         ; indexOfWholeArea < m_vecContainers.size()
         ; indexOfWholeArea++ )
    {
        Mine *pUnit = m_vecContainers.at(indexOfWholeArea);

        if(pUnit->m_bMine)
        {
            continue;
        }

        int iMinesAroundit = 0;
        int iYPos = pUnit->m_LogicY;
        int iXPos = pUnit->m_LogicX;
        iMinesAroundit = CalcMinesAroundCurrent(QPoint(iXPos, iYPos));

        pUnit->m_Number = iMinesAroundit;
        pUnit->GenerateMineColor();
    }


    return 1;
}

int MainWindow::CalcMinesAroundCurrent(QPoint point)
{
    int iYPos = point.y();
    int iXPos = point.x();

    int iMinesAroundit = 0;
    for (int i = iYPos - 1; i <= iYPos + 1; i++)
    {
        for (int j = iXPos - 1; j <= iXPos + 1; j++)
        {
            if (i < 0 || i >= m_XCnt || j < 0 || j >= m_YCnt)
            {
                continue;
            }

            int index = i * this->m_XCnt + j;

            Mine *pAround = m_vecContainers.at(index);

            if (pAround->m_bMine)
            {
                iMinesAroundit++;
            }
        }
    }

    return iMinesAroundit;
}

int MainWindow::HandleMineShow(QPoint point, Qt::MouseButton button)
{

    int vecIndex = 0;
    for( vecIndex = 0; vecIndex < m_vecContainers.size(); vecIndex++)
    {
        if (m_vecContainers[vecIndex]->m_LogicX == point.x()
                && m_vecContainers[vecIndex]->m_LogicY == point.y())
        {
            break;
        }
    }

    if (vecIndex == m_vecContainers.size())
    {
        return 0;
    }

    Mine *pUnit = m_vecContainers[vecIndex];

    if (pUnit->m_bFroze || pUnit->m_bDisplay)
    {
        return 0;
    }


    if (button == Qt::LeftButton)
    {
        if (pUnit->m_bMarked)
        {
            return 0;
        }

        if (pUnit->m_Number == 0)  // 周围8个都不是雷，则要递归地全部翻开
        {
            HandleAreaAround(QPoint(pUnit->m_LogicX, pUnit->m_LogicY));
        }
        else
        {
            pUnit->m_bDisplay = true;  // 翻开
        }


    }
    else if (button == Qt::RightButton)
    {
        if (pUnit->m_bMarked)
        {
            pUnit->m_bMarked = false;
            m_MinesFound -= 1;
        }
        else
        {
            pUnit->m_bMarked = true;
            m_MinesFound += 1;
        }

    }

    // 计算游戏是否结束
    if (pUnit->m_bDisplay && pUnit->m_bMine)
    {
        return -1;
    }


    if (m_MinesFound == m_MinesCnt)
    {
        return 1;
    }



    return 0;
}

int MainWindow::HandleAreaAround(QPoint point)
{
    if (point.x() < 0 || point.x() >= m_XCnt
            || point.y() < 0 || point.y() >= m_YCnt)
    {
        return -1;
    }

    int iX = point.x();
    int iY = point.y();

    Mine *pUnit = m_vecContainers[iY * m_XCnt + iX];
    if (pUnit->m_bFroze || pUnit->m_bDisplay)
    {
        return 0;
    }

    if (pUnit->m_Number == 0)
    {
        pUnit->m_bFroze = true;
        pUnit->m_bDisplay = true;
        pUnit->m_bMarked = false;

        // 递归处理周围8个点
        HandleAreaAround(QPoint(iX - 1, iY - 1));
        HandleAreaAround(QPoint(iX    , iY - 1));
        HandleAreaAround(QPoint(iX + 1, iY - 1));
        HandleAreaAround(QPoint(iX - 1, iY    ));
        HandleAreaAround(QPoint(iX + 1, iY    ));
        HandleAreaAround(QPoint(iX - 1, iY + 1));
        HandleAreaAround(QPoint(iX    , iY + 1));
        HandleAreaAround(QPoint(iX + 1, iY + 1));

    }
    else if (pUnit->m_Number > 0)
    {
        pUnit->m_bDisplay = true;
        return 1;
    }
    else
    {
        return 1;
    }

}



int MainWindow::LayoutMineUI(QPainter *painter)
{
    QRect rectThis = this->rect();

    rectThis.setTop(rectThis.top() + 100);
    rectThis.setLeft(rectThis.left() + 20);
    rectThis.setBottom(rectThis.bottom() - 30);
    //rectThis.setRight(rectThis.right() - 10);

    m_rectMineArea = rectThis;

    int Height = rectThis.height();
    int Width = rectThis.width();

    int SizePerMineInX = Width / m_XCnt;
    int SizePerMineInY = Height / m_YCnt;

    m_SizePerMine = SizePerMineInX < SizePerMineInY ? SizePerMineInX : SizePerMineInY;

    QPen pen(QColor(55, 55, 55));
    QBrush brush(Qt::transparent);
    //brush.setStyle(Qt::NoBrush);

    painter->setPen(pen);
    painter->setBrush(brush);

    qSort(m_vecContainers.begin(), m_vecContainers.end(), CompareMinePosition);

    for (int i = 0; i < m_XCnt; i++)
    {
        for (int j = 0; j < m_YCnt; j++)
        {
            int indexOfVec = j * m_XCnt + i;
            Mine *pUnit = m_vecContainers[indexOfVec];

            if (!pUnit->m_bFroze)
            {
                pen.setStyle(Qt::SolidLine);
                painter->setPen(pen);

            }
            else
            {
                pen.setStyle(Qt::DotLine);
                painter->setPen(pen);
            }

            painter->drawRect(rectThis.left() + i * m_SizePerMine
                              , rectThis.top() + j * m_SizePerMine
                              , m_SizePerMine
                              , m_SizePerMine);


            if (!pUnit->m_bFroze)  // 冻结方块不做3D处理
            {
                pen.setStyle(Qt::SolidLine);
                painter->setPen(pen);
                painter->drawRect(rectThis.left() + i * m_SizePerMine + 2
                                  , rectThis.top() + j * m_SizePerMine + 2
                                  , m_SizePerMine - 2
                                  , m_SizePerMine - 2);
            }
        }
    }

    // 画已翻开的方块
    for(int vecIndex = 0; vecIndex < m_ContainersCnt; vecIndex++)
    {
        Mine *pMine = m_vecContainers[vecIndex];

        QRect rectToDraw(rectThis.left() + pMine->m_LogicX * m_SizePerMine
                         , rectThis.top() + pMine->m_LogicY * m_SizePerMine
                         , m_SizePerMine, m_SizePerMine);

        if (pMine->m_bFroze)
        {
            continue;
        }

        if (pMine->m_bDisplay)
        {

            QPen pen(pMine->m_FontColor);
            painter->setPen(pen);

            if (!pMine->m_bMine)
            {

                QString sNumber = QString::asprintf("%d", pMine->m_Number);
                painter->drawText(rectToDraw
                                 , Qt::AlignHCenter | Qt::AlignCenter
                                 , sNumber);
            }
            else
            {
                rectToDraw.setBottom(rectToDraw.bottom() - (rectToDraw.height() + 4)/ 5);
                rectToDraw.setTop(rectToDraw.top() + (rectToDraw.height() + 4)/ 5 + 1);
                rectToDraw.setLeft(rectToDraw.left() + (rectToDraw.width() + 4)/ 5 + 1);
                rectToDraw.setRight(rectToDraw.right() - (rectToDraw.width() + 4)/ 5);

                QBrush brush(pMine->m_FontColor);
                painter->setBrush(brush);
                painter->drawEllipse(rectToDraw);
            }
        }

        if (pMine->m_bMarked)
        {
            QPen pen(QColor(255, 0, 0));
            painter->setPen(pen);
            painter->drawText(rectToDraw
                             , Qt::AlignHCenter | Qt::AlignCenter
                             , "★");
        }
    }


    return 1;

}

void MainWindow::on_actionStart_triggered()
{

    StartNewGame(20, 20, 60);

    this->update();
}

void MainWindow::on_actionPause_triggered()
{
    if (m_bOver || !m_bStart)
    {
        return;
    }

    m_bPause = !m_bPause;

    if (m_bPause)
    {
        ui->actionPause->setText("恢复");
        //暂停定时器
        killTimer(m_timerID);
    }
    else
    {
        ui->actionPause->setText("暂停");
        //恢复定时器
        m_timerID = startTimer(1000);
    }
}

void MainWindow::on_actionExit_triggered()
{

}
