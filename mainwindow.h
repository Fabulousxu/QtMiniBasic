#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"
#include "minibasic.h"
#include <qfiledialog.h>
#include <qeventloop.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindowClass; };
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void update_code();
    void update_tree();
    void set_code(size_t line, const QString &code);
    void rm_code(size_t line);
    void load(const QString &filename);
    void clear();
    void run();
    void kill();
    void help();
    void on_cmd_returnPressed();

private:
    Ui::MainWindowClass *ui;
    MiniBasic basic;
    bool isRun;

signals:
    void inputed(const QString &str);
};
