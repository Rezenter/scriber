#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort/QSerialPortInfo>
#include <stdlib.h>
#include <QDebug>

QT_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->widget = new RenderArea;
    log = new Logger();
    log->print(path);
    ui->setupUi(this);
    movingX = true;
    save = new QFile(path + "/default.save");
    save->open(QIODevice::ReadOnly);
    stream = new QTextStream(save);
    while(!stream->atEnd()) {
        QString line = stream->readLine();
        if(line.length() > 1 && line.at(0) == '#'){
            line.remove(0, 1);
            QStringList args = line.split("=");
            settings.insert(args.at(0).trimmed(), args.at(1).trimmed());
        }
    }
    save->close();
    resize(settings.value("windowSizeX").toInt(), settings.value("windowSizeY").toInt());
    move(settings.value("positionX").toInt(), settings.value("positionY").toInt());
    setWindowTitle(settings.value("windowTitle", "windowTitleNotFound"));
    if(settings.value("cutter").compare("L") == 0){
        ui->cutterL->setChecked(true);
    }else{
        ui->cutterR->setChecked(true);
    }

    realLengthX = settings.value("realLengthX", "0").toDouble();
    realLengthY = settings.value("realLengthY", "0").toDouble();
    setMoveControlsEnabled(false);
    ui->calibrateButton->setEnabled(false);
    scanPorts();
    comThread = new QThread();
    port = new ComChatter();
    port->moveToThread(comThread);
    port->port.moveToThread(comThread);
    int comboBoxDefault = ui->comboBoxx->findText(settings.value("lastUsedPort", "COM1"));
    ui->aBox->setValue(settings.value("2a", "74").toDouble());
    ui->bBox->setValue(settings.value("2b", "74").toDouble());
    ui->lBox->setValue(settings.value("l", "1000").toDouble());
    ui->hBox->setValue(settings.value("h", "5").toDouble());
    ui->xBox->setValue(settings.value("x", "22").toDouble());
    ui->yBox->setValue(settings.value("y", "14").toDouble());
    if(comboBoxDefault != -1){
        ui->comboBoxx->setCurrentIndex(comboBoxDefault);
    }
    QObject::connect(ui->runButton, &QPushButton::clicked, this, &MainWindow::transaction);
    QObject::connect(ui->scanButton, &QPushButton::clicked, this, &MainWindow::scanPorts);
    QObject::connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::connectVoid);
    QObject::connect(ui->stopXButton, &QPushButton::clicked, this, &MainWindow::stopX);
    QObject::connect(ui->stopYButton, &QPushButton::clicked, this, &MainWindow::stopY);
    QObject::connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::stopY);
    QObject::connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::stopX);
    QObject::connect(ui->leftButton, &QPushButton::clicked, this, &MainWindow::moveXl);
    QObject::connect(ui->upButton, &QPushButton::clicked, this, &MainWindow::moveYu);
    QObject::connect(ui->rightButton, &QPushButton::clicked, this, &MainWindow::moveXr);
    QObject::connect(ui->downButton, &QPushButton::clicked, this, &MainWindow::moveYd);
    QObject::connect(ui->calibrateButton, &QPushButton::clicked, this, &MainWindow::calibrate);
    QObject::connect(ui->parkButton, &QPushButton::clicked, this, &MainWindow::park);
    QObject::connect(port, SIGNAL(dead()), comThread, SLOT(quit()));
    QObject::connect(comThread, SIGNAL(finished()), port, SLOT(deleteLater()));
    QObject::connect(port, SIGNAL(dead()), comThread, SLOT(deleteLater()));
    QObject::connect(this, SIGNAL(connect(QString)), port, SLOT(connect(QString)));
    QObject::connect(this, SIGNAL(close()), port, SLOT(close()));
    QObject::connect(this, SIGNAL(send(QString)), port, SLOT(send(QString)));
    QObject::connect(port, SIGNAL(closed()), this, SLOT(closed()));
    QObject::connect(port, SIGNAL(error(QString)), this, SLOT(error(QString)));
    QObject::connect(port, SIGNAL(out(QString)), this, SLOT(in(QString)));
    QObject::connect(this, SIGNAL(send(QString)), log, SLOT(sending(QString)));
    QObject::connect(this, SIGNAL(connect(QString)), log, SLOT(sending(QString)));
    QObject::connect(port, SIGNAL(error(QString)), log, SLOT(error(QString)));
    QObject::connect(port, SIGNAL(out(QString)), log, SLOT(received(QString)));
    QObject::connect(log, SIGNAL(logEvent(QString)), this, SLOT(print(QString)));
    QObject::connect(ui->riseButton, &QPushButton::clicked, this, &MainWindow::rise);
    QObject::connect(ui->landButton, &QPushButton::clicked, this, &MainWindow::land);
    QObject::connect(ui->speedButton, &QPushButton::clicked, this, &MainWindow::setSpeed);
    QObject::connect(this, SIGNAL(setEll(qreal, qreal, qreal, qreal)), ui->widget, SLOT(setEll(qreal, qreal, qreal, qreal)));
    QObject::connect(ui->aBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reDraw);
    QObject::connect(ui->bBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reDraw);
    QObject::connect(ui->lBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reDraw);
    QObject::connect(ui->hBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reDraw);
    QObject::connect(ui->xBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reDraw);
    QObject::connect(ui->yBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::reDraw);
    QObject::connect(ui->modeBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainWindow::reDraw);
    QObject::connect(this, SIGNAL(setNet(qreal, qreal, int)), ui->widget, SLOT(setNet(qreal, qreal, int)));
    QObject::connect(this, SIGNAL(setPosX(qreal)), ui->xWidget, SLOT(setPos(qreal)));
    QObject::connect(this, SIGNAL(setPosY(qreal)), ui->yWidget, SLOT(setPos(qreal)));
    comThread->start();
    reDraw();
}

MainWindow::~MainWindow()
{
    comThread->quit();
    comThread->requestInterruption();
    log->print("QThread dead ? : ");
    if(comThread->wait()){
        log->print("True");
    }else{
        log->print("False");
    }
    delete comThread;
    log->print("saving");
    settings["lastUsedPort"] = ui->comboBoxx->currentText();
    if(ui->cutterL->isChecked()){
        settings["cutter"] = "L";
    }else{
        settings["cutter"] = "R";
    }
    settings["windowSizeX"] = QString::number(this->width());
    settings["windowSizeY"] = QString::number(this->height());
    settings["positionX"] = QString::number(this->pos().x());
    settings["positionY"] = QString::number(this->pos().y());
    settings["2a"] = QString::number(ui->aBox->value());
    settings["2b"] = QString::number(ui->bBox->value());
    settings["l"] = QString::number(ui->lBox->value());
    settings["h"] = QString::number(ui->hBox->value());
    settings["x"] = QString::number(ui->xBox->value());
    settings["y"] = QString::number(ui->yBox->value());
    saveDefaults();
    delete stream;
    save->close();
    delete save;
    delete log;
    delete ui;
}

void MainWindow::setMoveControlsEnabled(bool enable)
{
    ui->line->setEnabled(enable);
    ui->parkButton->setEnabled(enable);
    //modify
}


void MainWindow::transaction()
{
    emit send(ui->textLine->text());
}

void MainWindow::scanPorts()
{
    log->print("ports scanned");
    QStringList items;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        items.append(info.portName());
    }
    ui->comboBoxx->clear();
    ui->comboBoxx->insertItems(0, items);
}

void MainWindow::connectVoid()
{
    emit close();
    emit connect(ui->comboBoxx->currentText());
}

void MainWindow::in(QString data){
    QStringList commands = data.split("\r\n", QString::SkipEmptyParts);
    for (auto line : commands){
        QStringList command;
        command.append(line.split("("));
        QStringList params = command.at(1).split(",");
        params.replaceInStrings(")", "");
        if(command.at(0).compare("connected") == 0){
            opened = true;
            ui->calibrateButton->setEnabled(true);
        }else{
            if(command.at(0).compare("calibrated") == 0){
                xLength = params.at(0).toInt();
                yLength = params.at(1).toInt();
                scaleX = xLength/realLengthX;
                scaleY = yLength/realLengthY;
                ui->xWidget->setup(xLength/2, scaleX);
                ui->yWidget->setup(yLength, scaleY);
                setMoveControlsEnabled(true);
            }else{
                if(command.at(0).compare("stop") == 0){
                    if(params.at(0).trimmed().compare("x") == 0){
                        coordX = params.at(1).trimmed().toInt();
                        movingX = false;
                        emit setPosX(coordX);
                    }else{
                        coordY = params.at(1).trimmed().toInt();
                        movingY = false;
                        emit setPosY(coordY);
                    }

                    if(!movingX && !movingY){
                        if(parking){
                            parking = false;
                            setSpeed();
                        }
                    }
                }else{
                    if(command.at(0).compare("moving") == 0){
                        if(params.at(0).trimmed().compare("x") == 0){
                            movingX = true;
                        }else{
                            movingY = true;
                        }
                    }
                }
            }
        }
    }
}

void MainWindow::closed(){
    ui->calibrateButton->setEnabled(false);
    setMoveControlsEnabled(false);
    opened = false;
}

void MainWindow::error(QString data){
    log->print("error from MW : " + data);
}

void MainWindow::stopX()
{
    emit send("stop(1, x)");
}

void MainWindow::stopY()
{
    emit send("stop(1, y)");
}

void MainWindow::moveXr(){
    if(ui->mmBox->isChecked()){
        moveXRelativeMm(1);
    }else{
        moveXRelativeStep(1);
    }
}

void MainWindow::moveYu(){
    if(ui->mmBox->isChecked()){
        moveYRelativeMm(1);
    }else{
        moveYRelativeStep(1);
    }
}

void MainWindow::moveXl(){
    if(ui->mmBox->isChecked()){
        moveXRelativeMm(-1);
    }else{
        moveXRelativeStep(-1);
    }
}

void MainWindow::moveYd(){
    if(ui->mmBox->isChecked()){
        moveYRelativeMm(-1);
    }else{
        moveYRelativeStep(-1);
    }
}

void MainWindow::moveXRelativeStep(int sign)
{
    emit send("move(2, x, " + QString::number(coordX - ui->line->text().toInt()*sign) +  ')');
}

void MainWindow::moveXRelativeMm(int sign)
{
    emit send("move(2, x, " + QString::number(coordX - (int)(ui->line->text().toDouble()*scaleX*sign)) + ')');
}

void MainWindow::moveYRelativeStep(int sign)
{
    emit send("move(2, y, " + QString::number(coordY - ui->line->text().toInt()*sign) + ')');
}

void MainWindow::moveYRelativeMm(int sign)
{
    emit send("move(2, y, " + QString::number(coordY - (int)(ui->line->text().toDouble()*scaleY*sign)) + ')');
}

void MainWindow::calibrate(){
    log->print("calibration");
    emit send("calibrate(0)");
}

void MainWindow::park(){
    log->print("parking");
    emit send("speed(2, x, 500)");
    emit send("speed(2, y, 500)");
    emit send("move(2, x, " + QString::number(-(int)(xLength/2)) + ")");
    emit send("move(2, y, " + QString::number(-(yLength)) + ")");
    parking = true;
}

void MainWindow::saveDefaults(){
    log->print("savingDefaults");
    save->open(QIODevice::WriteOnly);
    delete stream;
    stream = new QTextStream(save);
    for(QHash<QString, QString>::const_iterator iter = settings.cbegin(), end = settings.cend(); iter != end; ++iter){
        *stream << "# " << iter.key() << " = " << iter.value() << endl << endl;
    }
}

void MainWindow::print(QString data){
    ui->textBrowser->append(data);
}

void MainWindow::rise(){
    if(ui->cutterL->isChecked()){
        log->print("rise L");
        emit send("rise(1, l)");
    }else{
        log->print("rise R");
        emit send("rise(1, r)");
    }
}

void MainWindow::land(){
    if(ui->cutterL->isChecked()){
        log->print("land L");
        emit send("land(1, l)");
    }else{
        log->print("land R");
        emit send("land(1, r)");
    }
}

void MainWindow::setSpeed(){
    speed = ui->speedBox->value();
    log->print("set Y speed = " + QString::number(speed));
    emit send("speed(2, y, " + QString::number(speed) + ")");
}

void MainWindow::reDraw(){
    if(ui->bBox->value() > ui->aBox->value()){
        ui->bBox->setValue(ui->aBox->value());
    }
    if(ui->lBox->value() > ui->aBox->value()){
        ui->lBox->setValue(ui->aBox->value());
    }
    if(ui->hBox->value() > ui->lBox->value()){
        ui->hBox->setValue(ui->lBox->value());
    }
    if(ui->xBox->value() != 0 && ui->yBox->value() != 0){
        emit setEll(ui->aBox->value()/2, ui->bBox->value()/2, ui->lBox->value(), ui->hBox->value());
        emit setNet(ui->xBox->value(), ui->yBox->value(), ui->modeBox->currentIndex());
    }
}
