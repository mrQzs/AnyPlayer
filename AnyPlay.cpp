
#include "AnyPlay.h"
#include "ui_AnyPlay.h"


AnyPlay::AnyPlay(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AnyPlay)
{
    ui->setupUi(this);
}

AnyPlay::~AnyPlay()
{
    delete ui;
}


