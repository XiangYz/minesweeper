#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mine.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    int LayoutMineUI(QPainter *);
    int LayoutMines();
    int CalcMinesAroundCurrent(QPoint point);
    int HandleAreaAround(QPoint point);
    static bool CompareMinePosition(Mine *mineA, Mine *mineB);

    int StartNewGame(int xcnt, int ycnt, int MinesCnt);

public:
    QRect m_rectMineArea;
    int m_XCnt;
    int m_YCnt;
    int m_SizePerMine;
    int m_ContainersCnt;
    int m_MinesCnt;
    int m_MinesFound;

    bool m_bOver;

    QVector<Mine *> m_vecContainers; // 存储


    bool m_bStart;
    bool m_bPause;

    int m_timerID;
    int m_SecCnt;

signals:
    void MousePressInMineArea(QPoint point, Qt::MouseButton button);

private slots:
    int HandleMineShow(QPoint point, Qt::MouseButton button);

    void on_actionStart_triggered();

    void on_actionPause_triggered();

    void on_actionExit_triggered();

protected:
    void paintEvent(QPaintEvent* );
    void timerEvent(QTimerEvent *);
    void mousePressEvent(QMouseEvent *event);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
