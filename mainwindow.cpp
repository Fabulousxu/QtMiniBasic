#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindowClass()) {
    ui->setupUi(this);

    basic.print = [this](qint64 num) { ui->run->append(QString::number(num)); };
    basic.input = [this]()->qint64 {
        ui->cmd->setText("? ");
        ui->cmd->setFocus();
        update_tree();
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
    ui->console->append("Shanghai Jiao Tong University. 2023/12/08");
    ui->console->append("******************************");
    ui->console->append("Type \"help\" to get help about how to use it.");
    ui->console->append("<font color=lightgreen>minibasic>&nbsp;</font>");

    connect(ui->btnRun, &QPushButton::clicked, this, [this] {
        ui->console->moveCursor(QTextCursor::End);
        if (!isRun) {
            ui->console->insertHtml("<font color=white>run</font>");
            run();
        } else {
            ui->console->insertHtml("<font color=white>kill</font>");
            kill();
            ui->console->append("<font color=lightgreen>minibasic>&nbsp;</font>");
        }
        });

    connect(ui->btnLoad, &QPushButton::clicked, this, [this] {
        QString filename = QFileDialog::getOpenFileName();
        ui->console->moveCursor(QTextCursor::End);
        ui->console->insertHtml("<font color=white>load " + filename + "</font>");
        load(filename);
        ui->console->append("<font color=lightgreen>minibasic>&nbsp;</font>");
        });

    connect(ui->btnClear, &QPushButton::clicked, this, [this] {
        ui->console->moveCursor(QTextCursor::End);
        ui->console->insertHtml("<font color=white>clear</font>");
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
    for (auto &line : basic.code_HTML()) { ui->code->append("<div>" + line + "</div>"); }
    update_tree();
}

void MainWindow::update_tree() {
    ui->tree->clear();
    for (auto &line : basic.tree_HTML()) { ui->tree->append("<div>" + line + "</div>"); }
}

void MainWindow::set_code(size_t line, const QString &code) {
    if (isRun) { kill(); }
    if (line < 0) {
        ui->console->append("<font color=red>error:&nbsp;</font>line number must be positive.");
        return;
    }
    try { basic.set_code(line, code); }
    catch (QString err) {
        ui->console->append("<font color=red>warning:&nbsp;</font>code exist syntax error -- " + err);
    }
    update_code();
}

void MainWindow::rm_code(size_t line) {
    if (isRun) { kill(); }
    try { basic.rm_code(line); }
    catch (QString err) {
        ui->console->append("<font color=red>error:&nbsp;</font>" + err);
        return;
    }
    update_code();
}

void MainWindow::load(const QString &filename) {
    if (isRun) { kill(); }
    try { basic.load(filename); }
    catch (QString err) {
        ui->console->append("<font color=red>error:&nbsp;</font>" + err);
        return;
    } 
    update_code();
}

void MainWindow::clear() {
    if (isRun) { kill(); }
    basic.clear();
    ui->run->clear();
    update_code();
}

void MainWindow::run() {
    if (isRun) {
        ui->console->append("<font color=red>warning:&nbsp;</font>The program is running.");
        ui->console->append("<font color=lightgreen>minibasic>&nbsp;</font>");
        return;
    }
    ui->console->append("<font color=lightgreen>minibasic>&nbsp;</font>");
    ui->btnRun->setStyleSheet("QPushButton {border: 0px solid white; image:url(:/MainWindow/image/kill.png);}"
        "QPushButton:hover {border: 0px solid white; image:url(:/MainWindow/image/kill_hover.png);}"
        "QPushButton:pressed {border: 0px solid white; image:url(:/MainWindow/image/kill_pressed.png);}");
    ui->run->clear();
    QString state;
    isRun = true;
    try { basic.run(); }
    catch (QString err) { state = err; }
    isRun = false;
    if (state.isEmpty()) {
        state = "<font color=lightgreen>process exited without error.</font>";
    } else { state = "<font color=red>process exited with 1 error:&nbsp;</font>" + state; };
    ui->run->append(state);
    update_tree();
    ui->btnRun->setStyleSheet("QPushButton {border: 0px solid white; image:url(:/MainWindow/image/run.png);}"
        "QPushButton:hover {border: 0px solid white; image:url(:/MainWindow/image/run_hover.png);}"
        "QPushButton:pressed {border: 0px solid white; image:url(:/MainWindow/image/run_pressed.png);}");
}

void MainWindow::kill() {
    if (!isRun) {
        ui->console->append("<font color=red>warning:&nbsp;</font>The program is not running.");
        return;
    }
    isRun = false;
    basic.kill();
    emit inputed("");
    ui->btnRun->setStyleSheet("QPushButton {border: 0px solid white; image:url(:/MainWindow/image/run.png);}"
        "QPushButton:hover {border: 0px solid white; image:url(:/MainWindow/image/run_hover.png);}"
        "QPushButton:pressed {border: 0px solid white; image:url(:/MainWindow/image/run_pressed.png);}");
    ui->cmd->clear();
}

void MainWindow::help() {
    ui->console->append("[line number] [code]  ---insert code in some line");
    ui->console->append("[line number]  ---remove code in some line");
    ui->console->append("r/run/RUN  ---run the basic program");
    ui->console->append("c/clear/CLEAR  ---clear the codes");
    ui->console->append("l/load/LOAD [file path]  ---load a basic code file");
    ui->console->append("s/save/SAVE [file path]  ---save a basic code file");
    ui->console->append("h/help/HELP  ---get help");
    ui->console->append("q/quit/QUIT  ---quit this program");
}

void MainWindow::on_cmd_returnPressed() {
    QString str = ui->cmd->text();
    ui->console->moveCursor(QTextCursor::End);
    ui->console->insertHtml("<font color=white>" + str + "</font>");
    ui->cmd->clear();
    str = str.trimmed();
    QString cmd = str.section(' ', 0, 0);
    str = str.section(' ', 1).trimmed();
    bool isNumber;
    qsizetype line_seq = cmd.toLongLong(&isNumber);
    if (isNumber) {
        if (!str.isEmpty()) { 
            set_code(line_seq, str); 
        } else { rm_code(line_seq); }
    } else if (cmd == "clear" || cmd == "CLEAR") {
        clear();
    } else if (cmd == "run" || cmd == "RUN" || cmd == "r") {
        run(); return;
    } else if (cmd == "kill" || cmd == "KILL") {
        kill();
    } else if (cmd == "load" || cmd == "LOAD" || cmd == "l") {
        load(str);
    } else if (cmd == "save" || cmd == "SAVE" || cmd == "s") {

    } else if (cmd == "help" || cmd == "HELP" || cmd == "h") {
        help();
    } else if (cmd == "quit" || cmd == "QUIT" || cmd == "q") {
        exit(0);
    } else if (cmd == "?") {
        emit inputed(str);
    } else { ui->console->append("<font color=red>error:&nbsp;</font>illegal command."); }
    ui->console->append("<font color=lightgreen>minibasic>&nbsp;</font>");
}
