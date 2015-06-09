#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "IL_global.h"

#include "IL_VA.h"

#include "IL_PICamera_over_UDT.h"

#include <thread>

extern boost::asio::io_service io_service;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_VA_initPBtn_clicked()
{
    //VA_unit.initialize();

    std::thread *va_init_thread = new std::thread(&VA::initialize,&VA_unit);
    //init_thread = new std::thread(&VA::initialize,&VA_unit);
}

void MainWindow::on_VA_set_angle_clicked()
{
    //VA_unit.set_angle(43.8);
    std::thread *va_set_angle_thread = new std::thread(&VA::set_angle,&VA_unit,43.8);
}

void MainWindow::on_Stop_io_service_clicked()
{
    //io_service.stop();
    //ggNet::stop_io_service(&ggNet::io_service);
    ggNet::vx_Net.stop_send_recv_thread();
}

void MainWindow::on_PICam_acquire_to_save_clicked()
{
    PICamera_over_UDT piObj;

    piObj.acquire_to_save("test.png");

}
