#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"
#include "minibasic.h"
#include <qfiledialog.h>
#include <qeventloop.h>
#include <QtCore/qtextstream.h>

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
    void setCode(qsizetype line, const QString &code);
    void removeCode(size_t line);
    void loadCode(const QString &filename);
    void saveCode(const QString &filename);
    void clear();
    void run();
    void kill();
    void help();
    void immediatelyPrint(const QString &code);
    void immediatelyLet(const QString &code);
    void immediatelyInput(const QString &code);
    void on_cmd_returnPressed();

private:
    Ui::MainWindowClass *ui;
    MiniBasic basic;

signals:
    void inputed(const QString &str);
};
