#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "IL_VA.h"
#include <thread>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_VA_initPBtn_clicked();

    void on_VA_set_angle_clicked();

    void on_Stop_io_service_clicked();

    void on_PICam_acquire_to_save_clicked();

private:
    Ui::MainWindow *ui;

private:
    VA VA_unit;

private:
    std::thread *init_thread;
};

#endif // MAINWINDOW_H
