#include "widget.h"
#include "chessboard.h"
#include "./ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget) {
    ui->setupUi(this);
    this->setWindowTitle("中国象棋");
    new ChessBoard(ui->chessboard);
}

Widget::~Widget() {
    delete ui;
}