#pragma once

#include <QtCore/qstring.h>
#include <QtCore/qexception.h>

class ScanError : public QException {
public:
	ScanError(const QString &message = "error when scan.") : message(message) {}
	QString what() { return message; }
private:
	QString message;
};

class ParserError : public QException {
public:
	ParserError(const QString &message = "error when scan.") : message(message) {}
	QString what() { return message; }
private:
	QString message;
};

class RuntimeError : public QException {
public:
	RuntimeError(const QString &message = "error when scan.") : message(message) {}
	QString what() { return message; }
private:
	QString message;
};