#include "connectto.h"
#include "ui_connectto.h"

connectTo::connectTo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::connectTo)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->input_address->setFocus();
}

connectTo::~connectTo()
{
    delete ui;
}

QString connectTo::address()
{
    return ui->input_address->text();
}
