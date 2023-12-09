#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindowClass()) {
    ui->setupUi(this);

    basic.print = [this](qint64 num) {ui->run->append(QString::number(num)); };
    basic.input = [this]()->qint64 {
        ui->cmd->setText("? ");
        ui->cmd->setFocus();
        qint64 val;
        QEventLoop loop;
        connect(this, &MainWindow::inputed, this, [&](const QString &str) { val = str.toLongLong(); loop.quit(); });
        loop.exec();
        disconnect(this, &MainWindow::inputed, 0, 0);
        return val;
        };

    isRun = false;
    ui->cmd->setFocus();
    ui->console->append("Qt MiniBasic Interpreter");
    ui->console->append("Developer: XU Peigong  xupeigong@sjtu.edu.cn");
    ui->console->append("Shanghai Jiao Tong University. 2023/12/8");
    ui->console->append("******************************");
    ui->console->append("Type \"help\" to get help about how to use it.");
    ui->console->append("<font color=lightgreen>minibasic>&nbsp;</font>");

    connect(ui->btnRun, &QPushButton::clicked, this, [this] {
        if (!isRun) {
            ui->console->insertPlainText("run");
            run();
        } else {
            ui->console->insertPlainText("kill");
            ui->console->append("<font color=lightgreen>minibasic>&nbsp;</font>");
            kill();
        }
        });

    connect(ui->btnLoad, &QPushButton::clicked, this, [this] {
        QString filename = QFileDialog::getOpenFileName();
        ui->console->insertPlainText("load " + filename);
        ui->console->append("<font color=lightgreen>minibasic>&nbsp;</font>");
        load(filename);
        });

    connect(ui->btnClear, &QPushButton::clicked, this, [this] {
        ui->console->insertPlainText("clear");
        ui->console->append("<font color=lightgreen>minibasic>&nbsp;</font>");
        clear();
        });

    connect(ui->cmd, &QLineEdit::returnPressed, this, &MainWindow::on_cmd_returnPressed);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::update_code() {
    ui->code->clear();
    for (auto &line : basic.code_strlist()) { ui->code->append("<div>" + line + "</div>"); }
    update_tree();
}

void MainWindow::update_tree() {
    ui->tree->clear();
    for (auto &line : basic.tree_strlist()) { ui->tree->append("<div>" + line + "</div>"); }
}

void MainWindow::set_code(qsizetype line, const QString &code) {
    if (line < 0) {
        ui->console->append("<font color=red>error:&nbsp;</font>line number must be positive.");
        return;
    }
    try { basic.set_code(line, code); }
    catch (QString err) {
        ui->console->append("<font color=red>warning:&nbsp;</font>code exist syntax error -- " + err);
    } update_code();
}

void MainWindow::rm_code(const QString &line) {
    bool isNumber;
    qsizetype line_num = line.toLongLong(&isNumber);
    if (isNumber) {
        try { basic.rm_code(line_num); }
        catch (QString err) {
            ui->console->append("<font color=red>error:&nbsp;</font>" + err);
            return;
        } update_code();
    } else { ui->console->append("<font color=red>error:&nbsp;</font>the line number must be constant number."); }
}

void MainWindow::load(const QString &filename) {
    kill();
    try { basic.load(filename); }
    catch (QString err) {
        ui->console->append("<font color=red>error:&nbsp;</font>" + err);
        return;
    } update_code();
}

void MainWindow::clear() {
    kill();
    basic.clear();
    ui->run->clear();
    update_code();
}

void MainWindow::run() {
    kill();
    ui->console->append("<font color=lightgreen>minibasic>&nbsp;</font>");
    ui->btnRun->setStyleSheet("QPushButton {image:url(:/MainWindow/image/kill.png);}"
        "QPushButton:hover {image:url(:/MainWindow/image/kill_hover.png);}"
        "QPushButton:pressed {image:url(:/MainWindow/image/kill_pressed.png);}");
    ui->run->clear();
    QString state;
    isRun = true;
    try { basic.run(); }
    catch (QString err) {
        state = err;
        basic.kill();
    }
    if (state.isEmpty()) {
        state = "<font color=lightgreen>process exited without error.</font>";
    } else { state = "<font color=red>process exited with 1 error:&nbsp;</font>" + state; };
    ui->run->append(state);
    update_tree();
    kill();
}

void MainWindow::kill() {
    isRun = false;
    basic.kill();
    emit inputed("");
    ui->btnRun->setStyleSheet("QPushButton {image:url(:/MainWindow/image/run.png);}"
        "QPushButton:hover {image:url(:/MainWindow/image/run_hover.png);}"
        "QPushButton:pressed {image:url(:/MainWindow/image/run_pressed.png);}");
    ui->cmd->clear();
}

void MainWindow::help() {
    ui->console->append("[line number] [code]  ---insert code in some line");
    ui->console->append("rm/RM [line number]   ---remove code in some line");
    ui->console->append("run/RUN               ---run the basic program");
    ui->console->append("clear/CLEAR           ---clear the codes");
    ui->console->append("load/LOAD [file path] ---load a basic code file");
    ui->console->append("help/HELP             ---get help");
    ui->console->append("quit/QUIT             ---quit this program");
}

void MainWindow::on_cmd_returnPressed() {
    QString str = ui->cmd->text();
    ui->console->moveCursor(QTextCursor::End);
    ui->console->insertHtml("<font color=white>" + str + "</font>");
    ui->cmd->clear();
    qDebug() << str;
    str = str.trimmed();
    QString cmd = str.section(' ', 0, 0);
    str = str.section(' ', 1).trimmed();
    bool isNumber;
    qsizetype line = cmd.toLongLong(&isNumber);
    if (isNumber) {
        set_code(line, str);
    } else if (cmd == "rm" || cmd == "RM") {
        rm_code(str);
    } else if (cmd == "clear" || cmd == "CLEAR") {
        clear();
    } else if (cmd == "run" || cmd == "RUN") {
        run(); return;
    } else if (cmd == "kill" || cmd == "KILL") {
        basic.kill();
        emit inputed(QString());
    } else if (cmd == "load" || cmd == "LOAD") {
        load(str);
    } else if (cmd == "save" || cmd == "SAVE") {

    } else if (cmd == "help" || cmd == "HELP") {
        help();
    } else if (cmd == "quit" || cmd == "QUIT" || cmd == "q" || cmd == "Q") {
        exit(0);
    } else if (cmd == "?") {
        emit inputed(str);
    } else if (cmd == "LET") {

    } else if (cmd == "PRINT") {

    } else if (cmd == "INPUT") {

    } else { ui->console->append("<font color=red>error:&nbsp;</font>illegal command."); }
    ui->console->append("<font color=lightgreen>minibasic>&nbsp;</font>");
}