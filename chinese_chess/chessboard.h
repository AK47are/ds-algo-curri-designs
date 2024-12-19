#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include "chesspieces.h"
#include <QGraphicsView>
#include <QLabel>
#include <QPushButton>
#include <QTcpServer>

class AbstractChessPiece;

// 可以直接 new 该类，也有胜利逻辑和阵营操作逻辑，但是不会有任何输出。
class SimpleChessBoard : public QGraphicsScene {
public:
    enum class Winner {none, black, red};
    static const int COLS = 9;
    static const int ROWS = 10;
    // 格子的长度宽度大小。
    const double WIDTH;
    const double HEIGHT;

    SimpleChessBoard(QGraphicsView *board);
    // 提供给棋子类访问的接口。
    QPointF getPoint(QPoint coord) const;
    AbstractChessPiece *getSelPie() const;
    AbstractChessPiece *getPiece(QPoint coord) const;
    Camp getCurrCamp() const;
    Winner getWinner() const;

protected:
    // 额外提供给派生棋盘类的接口。
    // 设置 virtual 的主要原因是让派生类可以监控特定函数的触发。
    virtual void setView(QGraphicsView *board);
    virtual void drawBoard();
    virtual void initPieces();

    void setSelPie(AbstractChessPiece *piece);

    // friend 函数需要额外调用下一行函数。
    friend void AbstractChessPiece::setCoord(QPoint coord);
    void setPiece(QPoint coord, AbstractChessPiece *piece);

    friend void AbstractChessPiece::notifyChess();
    virtual void handlePieceNotice(AbstractChessPiece *piece);
    virtual void movePiece(QPoint start, QPoint end);

    // 选择、移动、击杀逻辑
    void execSelect(AbstractChessPiece *piece);
    void execMove(QPoint start, QPoint end);
    void execKill(AbstractChessPiece *target);

private:
    // 用来添加自定义的棋子。
    void addOtherPiece();

    // 图形坐标与逻辑坐标的映射
    QPointF points[ROWS][COLS];
    AbstractChessPiece *pieces[ROWS][COLS];

    // 记录选择操作选择的 Item 对象。
    AbstractChessPiece *sel_piece = nullptr;

    // 选择逻辑用
    Camp curr_camp = Camp::black;
    // 胜利逻辑用
    Winner winner = Winner::none;
};

// 主要补充与窗口中的其他组件的交互。
// 例如实现文本显示，实现重置，悔棋，复盘功能等。
class IntActChessBoard : public SimpleChessBoard {
public:
    IntActChessBoard(QGraphicsView *board, QLabel *show_text, QPushButton *reset,
                     QPushButton *back);

protected:
    // 监控函数，通知组件，记录移动。
    void initPieces() override;
    virtual void movePiece(QPoint start, QPoint end) override;
    virtual void backMove();

    // 工具
    bool getIsBack() const;
    auto getChange() { return change; };

    // 通知组件
    void updateAll();
    void updateText();
    void updateWinner();

private:
    QLabel *text_ui;

    // 上一个动作是否为悔棋操作。
    bool is_back = false;
    QVector<std::array<QPoint, 2>> change;
};


// 添加客户端与服务端的初始化连接和判定当前客户端是否允许操作。
// 添加同步客户端会影响棋局的操作，使用 sendData(...) 和 handleData(...) 处理。
class NetActChessBoard : public IntActChessBoard {
public:
    NetActChessBoard(QGraphicsView *board, QLabel *show_text, QPushButton *reset,
                     QPushButton *back, Camp cp, QHostAddress ip, quint16 port);

protected:
    // 添加当前客户端阵营判定、人数判定
    void handlePieceNotice(AbstractChessPiece *piece) override;

    // 当前客户端进行操作后通知另一个客户端
    void movePiece(QPoint start, QPoint end) override;
    void initPieces() override;
    void backMove() override;

template <typename... Args>
    void sendData(QString command, Args... args);
    void sendData(QString command);

private slots:
    void handleData();
    void handleConnection();
    void handleDisconnected();

private:
    // 当前客户端阵营
    const Camp camp;

    // 默认黑棋为服务器端。
    QTcpServer *black = nullptr;
    QTcpSocket *red = nullptr;
};

#endif // CHESSBOARD_H
