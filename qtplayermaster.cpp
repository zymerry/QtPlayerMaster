#include "qtplayermaster.h"
#include "ui_qtplayermaster.h"

QtPlayerMaster::QtPlayerMaster(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QtPlayerMaster)
{
    ui->setupUi(this);
}

QtPlayerMaster::~QtPlayerMaster()
{
    delete ui;
}

