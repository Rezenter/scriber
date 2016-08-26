#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "comchatter.h"
#include "logger.h"
#include <QFile>
#include <QHash>
#include <QDoubleValidator>
#include <QIntValidator>
#include <QCoreApplication>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void connect(QString name);
    void close();
    void send(QString data);

private:
    QDoubleValidator *validator;
    QIntValidator *validator1;
    QDoubleValidator *validator2;
    QDoubleValidator *validator3;
    QDoubleValidator *validator4;
    QDoubleValidator *validator5;
    QString path = QCoreApplication::applicationDirPath();
    QFile *save;
    QTextStream *stream;
    void setMoveControlsEnabled(bool enable);
    Ui::MainWindow *ui;
    bool opened = false;
    QThread *comThread;
    ComChatter *port;
    int xLength;
    int yLength;
    int coordX;
    int coordY;
    Logger *log;
    void saveDefaults();
    QHash<QString, QString> settings;
    double realLengthX;
    double realLengthY;
    bool movingY;
    bool movingX;
    double scaleX;
    double scaleY;
    int drop;
    int cutStart;
    int substrate;
    int cutting = 0;
    int cuttingRound = 0;
    int cuttingRectangle = 0;
    void cutRectangle();
    int count;

public slots:
    void closed();
    void error(QString error);
    void in(QString data);
    void print(QString data);

private slots:
    void scanPorts();
    void transaction();
    void connectVoid();
    void stopX();
    void stopY();
    void moveXAbsoluteMm();
    void moveXAbsoluteStep();
    void moveXRelativeMm();
    void moveXRelativeStep();
    void moveYAbsoluteMm();
    void moveYAbsoluteStep();
    void moveYRelativeMm();
    void moveYRelativeStep();
    void calibrate();
    void park();
    void cut();
    void next();
    void back();
    void autoCut();
    void yes();
    void no();
    void abort();
};

#endif // MAINWINDOW_H
