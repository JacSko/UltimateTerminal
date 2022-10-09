#include "MainApplication.h"
#include "ui_MainWindow.h"

MainApplication::MainApplication(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainApplication::~MainApplication()
{
    delete ui;
}
