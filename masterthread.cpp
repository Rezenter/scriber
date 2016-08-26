/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "masterthread.h"
#include <QtSerialPort/QSerialPort>
#include <unistd.h>


QT_USE_NAMESPACE

MasterThread::MasterThread()
{
    portName = "COM39";
}


MasterThread::~MasterThread()
{
    //mutex.lock();
    //delete &portName;
    //delete &waitTimeout;
    //delete &request;
    //port.close();
    //delete &port;
    //delete &mutex;
}

void MasterThread::transaction(const QString &portNameIn, int waitTimeoutIn, const QString &requestIn)
{
    //QMutexLocker locker(&mutex);
    //portName = portNameIn;
    waitTimeout = waitTimeoutIn;
    request = requestIn;
    send();
}

void MasterThread::send(){
    //mutex.lock();

    QString portNameSaved = portName;
    QString requestSaved = request;
    int waitTimeoutSaved = waitTimeout;
    //mutex.unlock();
    port.setPortName(portNameSaved);
    if (port.open(QIODevice::ReadWrite)) {
        if (port.isOpen()){
            port.write(requestSaved.toLocal8Bit());
            QByteArray data;
            sleep(waitTimeoutSaved);
            data.append(port.readAll());
            QString res(data);
            emit response(res);
            port.close();
        } else {
            port.close();
            emit error(port.errorString());
        }
    }
    emit finished();
}


