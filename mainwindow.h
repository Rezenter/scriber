#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "comchatter.h"
#include "logger.h"
#include "renderarea.h"
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
    void connect(QString);
    void close();
    void send(QString);
    void setEll(qreal, qreal, qreal, qreal);
    void setNet(qreal, qreal, int);
    void setPosX(qreal);
    void setPosY(qreal);

private:
    QString path = QCoreApplication::applicationDirPath();
    QFile *save;
    QTextStream *stream;
    void setMoveControlsEnabled(bool);
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
    int substrate;
    int count;
    int speed;
    bool parking;
    void moveXRelativeMm(int);
    void moveXRelativeStep(int);
    void moveYRelativeMm(int);
    void moveYRelativeStep(int);

public slots:
    void closed();
    void error(QString);
    void in(QString);
    void print(QString);

private slots:
    void scanPorts();
    void transaction();
    void connectVoid();
    void stopX();
    void stopY();
    void moveXl();
    void moveYu();
    void moveXr();
    void moveYd();
    void calibrate();
    void park();
    void rise();
    void land();
    void setSpeed();
    void reDraw();
};

#endif // MAINWINDOW_H
