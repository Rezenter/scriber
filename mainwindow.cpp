#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort/QSerialPortInfo>
#include <stdlib.h>

QT_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
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
    ui->stackedWidget->setCurrentIndex(0);
    if(settings.value("cutter").compare("L") == 0){
        ui->cutterL->setChecked(true);
    }else{
        ui->cutterR->setChecked(true);
    }
    if(settings.value("shape").compare("round") == 0){
        ui->shapeRound->setChecked(true);
    }else{
        if(settings.value("shape").compare("else") == 0){
            ui->shapeElse->setChecked(true);
        }else{
            ui->shapeRectangle->setChecked(true);
        }
    }
    validator = new QDoubleValidator(0.0, 99.99, 2, this);
    ui->rectangleXLine->setText(settings.value("rectangleX"));
    ui->rectangleXLine->setValidator(validator);
    ui->rectangleYLine->setText(settings.value("rectangleY"));
    ui->rectangleYLine->setValidator(validator);
    ui->roundLine->setText(settings.value("radius"));
    ui->roundLine->setValidator(validator);
    realLengthX = settings.value("realLengthX", "0").toDouble();
    realLengthY = settings.value("realLengthY", "0").toDouble();
    setMoveControlsEnabled(false);
    ui->calibrateButton->setEnabled(false);
    ui->coordLLabelMm->setText(QString::number(-realLengthX/2) + " [mm]");
    ui->coordRLabelMm->setText(QString::number(realLengthX/2) + " [mm]");
    ui->coordULabelMm->setText(QString::number(realLengthY) + " [mm]");
    ui->coordDLabelMm->setText(QString::number(0) + " [mm]");
    ui->stepLine->setText(settings.value("step"));
    scanPorts();
    comThread = new QThread();
    port = new ComChatter();
    port->moveToThread(comThread);
    port->port.moveToThread(comThread);
    int comboBoxDefault = ui->comboBoxx->findText(settings.value("lastUsedPort", "COM1"));
    if(comboBoxDefault != -1){
        ui->comboBoxx->setCurrentIndex(comboBoxDefault);
    }
    validator1 = new QIntValidator(-10000, 10000, this);
    ui->moveXLineAbsoluteStep->setValidator(validator1);
    ui->moveYLineAbsoluteStep->setValidator(validator1);
    ui->moveXLineRelativeStep->setValidator(validator1);
    ui->moveYLineRelativeStep->setValidator(validator1);
    validator2 = new QDoubleValidator(-realLengthX/2, realLengthX/2, 2, this);
    validator3 = new QDoubleValidator(0, realLengthY, 2, this);
    validator4 = new QDoubleValidator(-realLengthY, realLengthY, 2, this);
    ui->moveXLineAbsoluteMm->setValidator(validator2);
    ui->moveYLineAbsoluteMm->setValidator(validator3);
    ui->moveXLineRelativeMm->setValidator(validator2);
    ui->moveYLineRelativeMm->setValidator(validator4);
    validator5 = new QDoubleValidator(0, realLengthX, 2, this);
    ui->stepLine->setValidator(validator5);
    QObject::connect(ui->runButton, &QPushButton::clicked, this, &MainWindow::transaction);
    QObject::connect(ui->scanButton, &QPushButton::clicked, this, &MainWindow::scanPorts);
    QObject::connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::connectVoid);
    QObject::connect(ui->stopXButton, &QPushButton::clicked, this, &MainWindow::stopX);
    QObject::connect(ui->stopYButton, &QPushButton::clicked, this, &MainWindow::stopY);
    QObject::connect(ui->moveXButtonAbsoluteMm, &QPushButton::clicked, this, &MainWindow::moveXAbsoluteMm);
    QObject::connect(ui->moveXButtonAbsoluteStep, &QPushButton::clicked, this, &MainWindow::moveXAbsoluteStep);
    QObject::connect(ui->moveXButtonRelativeMm, &QPushButton::clicked, this, &MainWindow::moveXRelativeMm);
    QObject::connect(ui->moveXButtonRelativeStep, &QPushButton::clicked, this, &MainWindow::moveXRelativeStep);
    QObject::connect(ui->moveYButtonAbsoluteMm, &QPushButton::clicked, this, &MainWindow::moveYAbsoluteMm);
    QObject::connect(ui->moveYButtonAbsoluteStep, &QPushButton::clicked, this, &MainWindow::moveYAbsoluteStep);
    QObject::connect(ui->moveYButtonRelativeMm, &QPushButton::clicked, this, &MainWindow::moveYRelativeMm);
    QObject::connect(ui->moveYButtonRelativeStep, &QPushButton::clicked, this, &MainWindow::moveYRelativeStep);
    QObject::connect(ui->calibrateButton, &QPushButton::clicked, this, &MainWindow::calibrate);
    QObject::connect(ui->backButton, &QPushButton::clicked, this, &MainWindow::back);
    QObject::connect(ui->parkButton, &QPushButton::clicked, this, &MainWindow::park);
    QObject::connect(ui->cutButton, &QPushButton::clicked, this, &MainWindow::cut);
    QObject::connect(ui->autoCutButton, &QPushButton::clicked, this, &MainWindow::autoCut);
    QObject::connect(ui->nextButton, &QPushButton::clicked, this, &MainWindow::next);
    QObject::connect(ui->yesButton, &QPushButton::clicked, this, &MainWindow::yes);
    QObject::connect(ui->noButton, &QPushButton::clicked, this, &MainWindow::no);
    QObject::connect(ui->abortButton, &QPushButton::clicked, this, &MainWindow::abort);
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
    comThread->start();
}

MainWindow::~MainWindow()
{
    comThread->quit();
    comThread->requestInterruption();
    delete validator;
    delete validator1;
    delete validator2;
    delete validator3;
    delete validator4;
    delete validator5;

    log->print("QThread dead : ");
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
    settings["shape"] = ui->buttonGroup->checkedButton()->text();
    settings["radius"] = ui->roundLine->text();
    settings["rectangleX"] = ui->rectangleXLine->text();
    settings["rectangleY"] = ui->rectangleYLine->text();
    settings["step"] = ui->stepLine->text();
    saveDefaults();
    delete stream;
    save->close();
    delete save;
    delete log;
    delete ui;
}

void MainWindow::setMoveControlsEnabled(bool enable)
{
    ui->cutButton->setEnabled(enable);
    ui->moveXButtonAbsoluteMm->setEnabled(enable);
    ui->moveXButtonAbsoluteStep->setEnabled(enable);
    ui->moveXButtonRelativeMm->setEnabled(enable);
    ui->moveXButtonRelativeStep->setEnabled(enable);
    ui->moveYButtonAbsoluteMm->setEnabled(enable);
    ui->moveYButtonAbsoluteStep->setEnabled(enable);
    ui->moveYButtonRelativeMm->setEnabled(enable);
    ui->moveYButtonRelativeStep->setEnabled(enable);
    ui->moveXLineAbsoluteMm->setEnabled(enable);
    ui->moveXLineAbsoluteStep->setEnabled(enable);
    ui->moveXLineRelativeMm->setEnabled(enable);
    ui->moveXLineRelativeStep->setEnabled(enable);
    ui->moveYLineAbsoluteMm->setEnabled(enable);
    ui->moveYLineAbsoluteStep->setEnabled(enable);
    ui->moveYLineRelativeMm->setEnabled(enable);
    ui->moveYLineRelativeStep->setEnabled(enable);
    ui->parkButton->setEnabled(enable);
    ui->nextButton->setEnabled(enable);
    ui->autoCutButton->setEnabled(enable);
    ui->backButton->setEnabled(enable);

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
                ui->coordLLabelStep->setText(QString::number(-xLength/2) + " [step]");
                ui->coordRLabelStep->setText(QString::number(xLength/2) + " [step]");
                ui->coordULabelStep->setText(QString::number(yLength) + " [step]");
                ui->coordDLabelStep->setText(QString::number(0) + " [step]");
                scaleX = xLength/realLengthX;
                scaleY = yLength/realLengthY;
                setMoveControlsEnabled(true);
                ui->cutButton->setEnabled(false);
                ui->autoCutButton->setEnabled(false);
            }else{
                if(command.at(0).compare("stop") == 0){
                    if(cutting > 0){
                        cut();
                    }else{
                        if(cuttingRectangle > 0){
                            cutRectangle();
                        }
                    }
                    if(params.at(0).trimmed().compare("x") == 0){
                        coordX = params.at(1).trimmed().toInt();
                        movingX = false;
                    }else{
                        coordY = params.at(1).trimmed().toInt();
                        movingY = false;
                    }
                    if(!movingX && !movingY){
                        ui->cutButton->setEnabled(true);
                        ui->autoCutButton->setEnabled(true);
                    }
                }else{
                    if(command.at(0).compare("moving") == 0){
                        ui->cutButton->setEnabled(false);
                        ui->autoCutButton->setEnabled(false);
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

void MainWindow::moveXAbsoluteStep()
{
    emit send("move(2, x, " + QString::number(-(int)(ui->moveXLineAbsoluteStep->text().toInt() + (xLength/2))) + ')');
}

void MainWindow::moveXAbsoluteMm()
{
    emit send("move(2, x, " + QString::number((int)( - (ui->moveXLineAbsoluteMm->text().toDouble()*scaleX) - (xLength/2))) + ')');
}

void MainWindow::moveXRelativeStep()
{
    emit send("move(2, x, " + QString::number(coordX - ui->moveXLineRelativeStep->text().toInt()) +  ')');
}

void MainWindow::moveXRelativeMm()
{
    emit send("move(2, x, " + QString::number(coordX - (int)(ui->moveXLineRelativeMm->text().toDouble()*scaleX)) + ')');
}

void MainWindow::moveYAbsoluteStep()
{
    emit send("move(2, y, " + QString::number(-(ui->moveYLineAbsoluteStep->text().toInt())) + ')');
}

void MainWindow::moveYAbsoluteMm()
{
    emit send("move(2, y, " + QString::number(-((int)(ui->moveYLineAbsoluteMm->text().toDouble()*scaleY))) + ')');
}

void MainWindow::moveYRelativeStep()
{
    emit send("move(2, y, " + QString::number(coordY - ui->moveYLineRelativeStep->text().toInt()) + ')');
}

void MainWindow::moveYRelativeMm()
{
    emit send("move(2, y, " + QString::number(coordY - (int)(ui->moveYLineRelativeMm->text().toDouble()*scaleY)) + ')');
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

void MainWindow::cut(){
    log->print("cut");
    ui->cutButton->setEnabled(false);
    ui->autoCutButton->setEnabled(false);
    if(cutting == 0){
        cutting = 1;
        cutStart = coordY;
        setMoveControlsEnabled(false);
        if(ui->cutterL->isChecked()){
            emit send("land(1, l)");
        }else{
            emit send("land(1, r)");
        }
        emit send("speed(2, y, 50)");
        emit send("move(2, y, " + QString::number(0) + ')');
    }else{
        if(cutting == 1){
            cutting = 2;
            emit send("speed(2, y, 100)");
            emit send("move(2, y, " + QString::number(cutStart) +')');
        }else{
            if(cutting == 2){
                cutting = 3;
                if(ui->cutterL->isChecked()){
                    emit send("land(1, l)");
                }else{
                    emit send("land(1, r)");
                }
                emit send("speed(2, y, 50)");
                emit send("move(2, y, " + QString::number(-yLength) +')');
            }else{
                if(cutting == 3){
                    cutting = 4;
                    emit send("speed(2, y, 100)");
                    emit send("move(2, y, " + QString::number(cutStart) +')');
                    setMoveControlsEnabled(true);
                    emit send("move(2, x, " + QString::number(coordX + (int)(ui->stepLine->text().toDouble()*scaleX)) + ')');
                }else{
                    cutting = 0;
                }
            }
        }
    }
}

void MainWindow::next(){
    if(ui->buttonGroup->checkedButton()->text().compare("else") == 0){
        ui->stackedWidget->setCurrentIndex(1);
        ui->shapeCutButtonsStacked->setCurrentIndex(0);
    }else{
        ui->stackedWidget->setCurrentIndex(1);
        ui->shapeCutButtonsStacked->setCurrentIndex(2);
    }
}

void MainWindow::back(){
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::print(QString data){
    ui->textBrowser->append(data);
}

void MainWindow::autoCut(){
    log->print("autoCut");
    if(ui->buttonGroup->checkedButton()->text().compare("round") == 0){
        count = floor((2*ui->roundLine->text().toDouble())/ui->stepLine->text().toDouble());
        log->print("round " + ui->roundLine->text());
    }else{
        count = floor(ui->rectangleXLine->text().toDouble()/ui->stepLine->text().toDouble());
        log->print("rectangle " + ui->rectangleXLine->text());
    }
    if(count > 0){
        //count++;
    }
    cutRectangle();
}

void MainWindow::cutRectangle(){

    ui->autoCutButton->setEnabled(false);
    if(cuttingRectangle == 0){
        setMoveControlsEnabled(false);
        cuttingRectangle = 1;
        cutRectangle();
    }else{
        if(cuttingRectangle == 1){
            if(count > 0){
                cut();
                count--;
            }else{
                cuttingRectangle = 0;
                setMoveControlsEnabled(true);
                ui->shapeCutButtonsStacked->setCurrentIndex(1);
            }
        }
    }
}

void MainWindow::yes(){
    log->print("swapping x/y");
    park();
    QString tmp = ui->rectangleXLine->text();
    ui->rectangleXLine->setText(ui->rectangleYLine->text());
    ui->rectangleYLine->setText(tmp);
    ui->shapeCutButtonsStacked->setCurrentIndex(2);
}

void MainWindow::no(){
    log->print("no swapping x/y");
    ui->shapeCutButtonsStacked->setCurrentIndex(2);
}

void MainWindow::abort(){
    log->print("abort");
    cutting = 0;
    count = 0;
    stopX();
    stopY();
    setMoveControlsEnabled(true);
}
