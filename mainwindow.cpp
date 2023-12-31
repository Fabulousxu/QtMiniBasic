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
        qint64 value;
        QEventLoop loop;
        connect(this, &MainWindow::inputed, this, [&](const QString &str) { value = str.toLongLong(); loop.quit(); });
        loop.exec();
        disconnect(this, &MainWindow::inputed, 0, 0);
        return value;
        };

    ui->cmd->setFocus();
    ui->console->append("Qt MiniBasic Interpreter");
    ui->console->append("Developer: XU Peigong email:xupeigong@sjtu.edu.cn");
    ui->console->append("Shanghai Jiao Tong University. 2023/12/08");
    ui->console->append("******************************");
    ui->console->append("Type \"help\" to get help about how to use it.");
    ui->console->append(makeFont("lightgreen", "minibasic>&nbsp;"));

    connect(ui->btnRun, &QPushButton::clicked, this, [this] {
        ui->console->moveCursor(QTextCursor::End);
        if (!basic.isRunning()) {
            ui->console->insertHtml(makeFont("white", "run"));
            run();
        } else {
            ui->console->insertHtml(makeFont("white", "kill"));
            kill();
            ui->console->append(makeFont("lightgreen", "minibasic>&nbsp;"));
        }
        });

    connect(ui->btnLoad, &QPushButton::clicked, this, [this] {
        QString filename = QFileDialog::getOpenFileName();
        ui->console->moveCursor(QTextCursor::End);
        ui->console->insertHtml(makeFont("white", "load&nbsp;" + filename));
        loadCode(filename);
        ui->console->append(makeFont("lightgreen", "minibasic>&nbsp;"));
        });

    connect(ui->btnClear, &QPushButton::clicked, this, [this] {
        ui->console->moveCursor(QTextCursor::End);
        ui->console->insertHtml(makeFont("white", "clear"));
        clear();
        ui->console->append(makeFont("lightgreen", "minibasic>&nbsp;"));
        });

    connect(ui->btnSave, &QPushButton::clicked, this, [this] {
        QString filename = QFileDialog::getSaveFileName();
        ui->console->moveCursor(QTextCursor::End);
        ui->console->insertHtml(makeFont("white", "save&nbsp;" + filename));
        saveCode(filename);
        ui->console->append(makeFont("lightgreen", "minibasic>&nbsp;"));
        });

    connect(ui->cmd, &QLineEdit::returnPressed, this, &MainWindow::on_cmd_returnPressed);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::update_code() {
    ui->code->clear();
    for (auto &line : basic.codeHTML()) { ui->code->append("<div>" + line + "</div>"); }
    update_tree();
}

void MainWindow::update_tree() {
    ui->tree->clear();
    for (auto &line : basic.treeHTML()) { ui->tree->append("<div>" + line + "</div>"); }
}

void MainWindow::setCode(qsizetype line, const QString &code) {
    if (basic.isRunning()) { kill(); }
    if (line < 0) {
        ui->console->append(makeFont(errorColor, "error:&nbsp;") + "line number must be positive.");
        return;
    }
    try { basic.setCode(line, code); }
    catch (QString err) {
        ui->console->append(makeFont(warningColor, "warning:&nbsp;") + "code exists syntax error -- " + err);
    }
    update_code();
}

void MainWindow::removeCode(size_t line) {
    if (basic.isRunning()) { kill(); }
    try { basic.removeCode(line); }
    catch (QString err) {
        ui->console->append(makeFont(errorColor, "error:&nbsp;") + err);
        return;
    }
    update_code();
}

void MainWindow::loadCode(const QString &filename) {
    if (basic.isRunning()) { kill(); }
    try { basic.loadCode(filename); }
    catch (QString error) {
        ui->console->append(makeFont(errorColor, "error:&nbsp;") + error);
        return;
    } 
    update_code();
}

void MainWindow::saveCode(const QString &filename) {
    QFile *file = new QFile(filename);
    if (!file->open(QIODevice::WriteOnly | QIODevice::Text)) {
        ui->console->append(makeFont(errorColor, "error:&nbsp;") + "no such file.");
    }
    QTextStream stream(file);
    stream << ui->code->toPlainText();
    file->close();
}

void MainWindow::clear() {
    if (basic.isRunning()) { kill(); }
    basic.clearCode();
    ui->run->clear();
    update_code();
}

void MainWindow::run() {
    if (basic.isRunning()) {
        ui->console->append(makeFont(warningColor, "warning:&nbsp;") + "The program is running.");
        ui->console->append(makeFont("lightgreen", "minibasic>&nbsp;"));
        return;
    }
    ui->console->append(makeFont("lightgreen", "minibasic>&nbsp;"));
    ui->btnRun->setStyleSheet("QPushButton {border: 0px solid white; image:url(:/MainWindow/image/kill.png);}"
        "QPushButton:hover {border: 0px solid white; image:url(:/MainWindow/image/kill_hover.png);}"
        "QPushButton:pressed {border: 0px solid white; image:url(:/MainWindow/image/kill_pressed.png);}");
    ui->run->clear();
    QString state;
    try { basic.run(); }
    catch (QString err) { state = err; }
    if (state.isEmpty()) {
        state = makeFont("lightgreen", "process exited without error.");
    } else { state = makeFont(errorColor, "process exited with 1 error:&nbsp;") + state; };
    ui->run->append(state);
    update_tree();
    ui->btnRun->setStyleSheet("QPushButton {border: 0px solid white; image:url(:/MainWindow/image/run.png);}"
        "QPushButton:hover {border: 0px solid white; image:url(:/MainWindow/image/run_hover.png);}"
        "QPushButton:pressed {border: 0px solid white; image:url(:/MainWindow/image/run_pressed.png);}");
}

void MainWindow::kill() {
    if (!basic.isRunning()) {
        ui->console->append(makeFont(warningColor, "warning:&nbsp;") + "The program is not running.");
        return;
    }
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

void MainWindow::immediatelyPrint(const QString &code) {
    QList<Token *> tokenList;
    Expression *expression = nullptr;
    qint64 value;

    try { value = EvaluationState().getValue(expression = parserExpression(tokenList = scan(code))); }
    catch (QPair<QList<Token *>, ScanError> scanError) {
        clearTokenList(scanError.first);
        ui->console->append(makeFont(errorColor, "error:&nbsp;") + scanError.second.what());
        return;
    }
    catch (ParserError parserError) {
        clearTokenList(tokenList);
        ui->console->append(makeFont(errorColor, "error:&nbsp;") + parserError.what());
        return;
    }
    catch (RuntimeError runtimeError) {
        clearTokenList(tokenList);
        delete expression;
        ui->console->append(makeFont(errorColor, "error:&nbsp;") + runtimeError.what());
        return;
    }
    ui->console->append(QString::number(value));
    clearTokenList(tokenList);
    delete expression;
}

void MainWindow::immediatelyLet(const QString &code) {
    QList<Token *> tokenList;
    Statement *statement = nullptr;

    try { EvaluationState().getValue(((LetStatement *)(statement = parser(tokenList = scan(code))))->expression); }
    catch (QPair<QList<Token *>, ScanError> scanError) {
        clearTokenList(scanError.first);
        ui->console->append(makeFont(errorColor, "error:&nbsp;") + scanError.second.what());
        return;
    }
    catch (ParserError parserError) {
        clearTokenList(tokenList);
        ui->console->append(makeFont(errorColor, "error:&nbsp;") + parserError.what());
        return;
    }
    catch (RuntimeError runtimeError) {
        clearTokenList(tokenList);
        delete statement;
        ui->console->append(makeFont(errorColor, "error:&nbsp;") + runtimeError.what());
        return;
    }
    clearTokenList(tokenList);
    delete statement;
}

void MainWindow::immediatelyInput(const QString &code) {
    QList<Token *> tokenList;
    Statement *statement = nullptr;

    try { statement = parser(tokenList = scan(code)); }
    catch (QPair<QList<Token *>, ScanError> scanError) {
        clearTokenList(scanError.first);
        ui->console->append(makeFont(errorColor, "error:&nbsp;") + scanError.second.what());
        return;
    }
    catch (ParserError parserError) {
        clearTokenList(tokenList);
        ui->console->append(makeFont(errorColor, "error:&nbsp;") + parserError.what());
        return;
    }
    ui->cmd->setText("? ");
    clearTokenList(tokenList);
    delete statement;
}

void MainWindow::on_cmd_returnPressed() {
    QString text = ui->cmd->text();
    ui->console->moveCursor(QTextCursor::End);
    ui->console->insertHtml(makeFont("white", text));
    ui->cmd->clear();
    text = text.trimmed();
    QString cmd = text.section(' ', 0, 0);
    QString str = text.section(' ', 1, -1).trimmed();
    bool isNumber;
    qsizetype lineList = cmd.toLongLong(&isNumber);
    if (isNumber) {
        if (!str.isEmpty()) {
            setCode(lineList, str);
        } else { removeCode(lineList); }
    } else if (cmd == "clear" || cmd == "CLEAR" || cmd == "c") {
        clear();
    } else if (cmd == "run" || cmd == "RUN" || cmd == "r") {
        run(); return;
    } else if (cmd == "kill" || cmd == "KILL") {
        kill();
    } else if (cmd == "load" || cmd == "LOAD" || cmd == "l") {
        loadCode(str);
    } else if (cmd == "save" || cmd == "SAVE" || cmd == "s") {
        saveCode(str);
    } else if (cmd == "help" || cmd == "HELP" || cmd == "h") {
        help();
    } else if (cmd == "quit" || cmd == "QUIT" || cmd == "q") {
        exit(0);
    } else if (cmd == "PRINT") {
        immediatelyPrint(str);
    } else if (cmd == "LET") {
        immediatelyLet(text);
    } else if (cmd == "INPUT") {
        immediatelyInput(text);
    } else if (cmd == "?") {
        emit inputed(str);
    } else { ui->console->append(makeFont(errorColor, "error:&nbsp;") + "illegal command."); }
    ui->console->append(makeFont("lightgreen", "minibasic>&nbsp;"));
}
