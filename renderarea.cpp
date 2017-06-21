#include "renderarea.h"

#include <QPainter>
#include <QDebug>
#include <QtMath>


RenderArea::RenderArea(QWidget *parent)
    : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    l = 60;
    ell = QPair<qreal, qreal>(40, 30);
    x = 14;
    y = 22;
    mode = 0;
    moveX = 32.65;
    moveY = 84.2/2;
    this->setMouseTracking(true);
}

QSize RenderArea::minimumSizeHint() const
{
    return QSize(300, 300);
}

QSize RenderArea::sizeHint() const
{
    return QSize(400, 300);
}

void RenderArea::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    ang = qAcos((l - ell.first)/ell.first)*180/M_PI;
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setBrush(brush);
    painter.fillRect(QRect(0, 0, width()/2 - moveX*maxPix/maxMm, height()),QBrush(QColor(220,220,220)));
    painter.fillRect(QRect(width()/2 + moveX*maxPix/maxMm, 0, width()/2 - moveX*maxPix/maxMm, height()),QBrush(QColor(200,200,200)));
    pen.setColor(QColor(100,100,100));
    painter.setPen(pen);
    //l vert right
    painter.drawLine(1 + width()/2 + (l-ell.first)*maxPix/maxMm, 2, 1 + width()/2 + (l-ell.first)*maxPix/maxMm, height()/2);
    //com vert left
    painter.drawLine(1 + (width() - 2*ell.first*maxPix/maxMm)/2, 2, 1 + (width() - 2*ell.first*maxPix/maxMm)/2, height()- 3);
    //l hor
    painter.drawLine(1 + (width() - 2*ell.first*maxPix/maxMm)/2, 5, 1 + width()/2 + (l-ell.first)*maxPix/maxMm, 5);
    //2a vert right
    painter.drawLine(1 + width()/2 + ell.first*maxPix/maxMm, height() - 3, 1 + width()/2 + ell.first*maxPix/maxMm, height()/2);
    //2a hor
    painter.drawLine(1 + width()/2 + ell.first*maxPix/maxMm, height() - 6, 1 + (width() - 2*ell.first*maxPix/maxMm)/2, height() - 6);
    //2b hor top
    painter.drawLine(2, height()/2 - ell.second*maxPix/maxMm, width()/2, height()/2 - ell.second*maxPix/maxMm);
    //2b hor bot
    painter.drawLine(2, height()/2 + ell.second*maxPix/maxMm, width()/2, height()/2 + ell.second*maxPix/maxMm);
    //2b vert
    painter.drawLine(5, height()/2 - ell.second*maxPix/maxMm, 5, height()/2 + ell.second*maxPix/maxMm);
    //full ell
    painter.drawEllipse(QRect(1 + (width() - 2*ell.first*maxPix/maxMm)/2, 1 + (height() - 2*ell.second*maxPix/maxMm)/2, 2*ell.first*maxPix/maxMm, 2*ell.second*maxPix/maxMm));
    //h vert right
    painter.drawLine(1 + width()/2 + (l - ell.first - h)*maxPix/maxMm, height()/2, 1 + width()/2 + (l - ell.first - h)*maxPix/maxMm, 18);
    //h hor
    painter.drawLine(1 + width()/2 + (l - ell.first - h)*maxPix/maxMm, 30, 1 + width()/2 + (l-ell.first)*maxPix/maxMm, 30);
    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));
    pen.setColor(QColor(0,250,0));
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawChord(QRect(1 + (width() - 2*ell.first*maxPix/maxMm)/2, 1 + (height() - 2*ell.second*maxPix/maxMm)/2, 2*ell.first*maxPix/maxMm, 2*ell.second*maxPix/maxMm), ang*16, (360 - 2*ang)*16);
    pen.setColor(QColor(0,0,0));
    painter.setPen(pen);
    painter.drawText(QRect(1,height() - 40, 50, 15), Qt::AlignCenter, ("x = " + QString::number((pos.x() - (width()/2 + (l - ell.first - h)*maxPix/maxMm))*maxMm/maxPix, 'f', 0)));
    painter.drawText(QRect(1,height() - 25, 50, 15), Qt::AlignCenter, ("y = " + QString::number((-height()/2 + pos.y())*maxMm/maxPix, 'f', 0)));
    painter.drawText(QRect(1 + (width() - 2*ell.first*maxPix/maxMm)/2, height() - 16, 2*ell.first*maxPix/maxMm, 10), Qt::AlignCenter, ("2a"));
    painter.drawText(QRect(1 + (width() - 2*ell.first*maxPix/maxMm)/2, 7, width()/2 + (l-ell.first)*maxPix/maxMm - (width() - 2*ell.first*maxPix/maxMm)/2, 10), Qt::AlignCenter, ("L") );
    painter.drawText(QRect(8, 0, 10, height()),  Qt::AlignCenter, ("2b"));
    painter.drawText(QRect(1 + width()/2 + (l - ell.first - h)*maxPix/maxMm, 18, h*maxPix/maxMm, 10), Qt::AlignCenter, "h");
    pen.setColor(QColor(250, 1,1));
    painter.setPen(pen);
    switch (mode) {
    case 0:
        cut = 1 + width()/2 + (l - ell.first - h)*maxPix/maxMm;
        while(cut > 1 + (width() - 2*ell.first*maxPix/maxMm)/2){
            painter.drawLine(cut, height()/2 + moveY*maxPix/maxMm, cut, height()/2 - moveY*maxPix/maxMm);
            cut -= x*maxPix/maxMm;
        }
        cut = height()/2;
        while(cut < height()/2 + ell.first*maxPix/maxMm){
            painter.drawLine(width()/2 - moveY*maxPix/maxMm, cut, width()/2 + moveX*maxPix/maxMm, cut);
            cut += y*maxPix/maxMm;
        }
        cut = height()/2;
        while(cut > height()/2 - ell.first*maxPix/maxMm){
            painter.drawLine(width()/2 - moveY*maxPix/maxMm, cut, width()/2 + moveY*maxPix/maxMm, cut);
            cut -= y*maxPix/maxMm;
        }
        break;
    case 1:
        cut = 1 + width()/2 + (l - ell.first - h)*maxPix/maxMm;
        while(cut > 1 + (width() - 2*ell.first*maxPix/maxMm)/2){
            painter.drawLine(cut, height()/2 + moveY*maxPix/maxMm, cut, height()/2 - moveY*maxPix/maxMm);
            cut -= y*maxPix/maxMm;
        }
        cut = height()/2;
        while(cut < height()/2 + ell.first*maxPix/maxMm){
            painter.drawLine(width()/2 - moveY*maxPix/maxMm, cut, width()/2 + moveX*maxPix/maxMm, cut);
            cut += x*maxPix/maxMm;
        }
        cut = height()/2;
        while(cut > height()/2 - ell.first*maxPix/maxMm){
            painter.drawLine(width()/2 - moveY*maxPix/maxMm, cut, width()/2 + moveY*maxPix/maxMm, cut);
            cut -= x*maxPix/maxMm;
        }
        break;
    case 2:
        cut = 1 + width()/2 + (l - ell.first - h)*maxPix/maxMm;
        while(cut > 1 + (width() - 2*ell.first*maxPix/maxMm)/2){
            painter.drawLine(cut, height()/2 + maxPix/2, cut, height()/2 - maxPix/2);
            cut -= x*maxPix/maxMm;
        }
        cut = height()/2 - y*maxPix/(maxMm*2);
        while(cut < height()/2 + ell.first*maxPix/maxMm){
            painter.drawLine(width()/2 - moveY*maxPix/maxMm, cut, width()/2 + moveY*maxPix/maxMm, cut);
            cut += y*maxPix/maxMm;
        }
        cut = height()/2 - y*maxPix/(maxMm*2);
        while(cut > height()/2 - ell.first*maxPix/maxMm){
            painter.drawLine(width()/2 - moveY*maxPix/maxMm, cut, width()/2 + moveY*maxPix/maxMm, cut);
            cut -= y*maxPix/maxMm;
        }
        break;
    case 3:
        cut = 1 + width()/2 + (l - ell.first - h)*maxPix/maxMm;
        while(cut > 1 + (width() - 2*ell.first*maxPix/maxMm)/2){
            painter.drawLine(cut, height()/2 + maxPix/2, cut, height()/2 - maxPix/2);
            cut -= y*maxPix/maxMm;
        }
        cut = height()/2 - x*maxPix/(maxMm*2);
        while(cut < height()/2 + ell.first*maxPix/maxMm){
            painter.drawLine(width()/2 - moveY*maxPix/maxMm, cut, width()/2 + moveY*maxPix/maxMm, cut);
            cut += x*maxPix/maxMm;
        }
        cut = height()/2 - x*maxPix/(maxMm*2);
        while(cut > height()/2 - ell.first*maxPix/maxMm){
            painter.drawLine(width()/2 - moveY*maxPix/maxMm, cut, width()/2 + moveY*maxPix/maxMm, cut);
            cut -= x*maxPix/maxMm;
        }
        break;
    }
}

void RenderArea::mouseMoveEvent(QMouseEvent * event){
    pos.setX(event->x());
    pos.setY(event->y());
    update();
}

void RenderArea::setEll(qreal a, qreal b, qreal l,qreal h){
    ell = QPair<qreal, qreal>(a, b);
    this->l = l;
    this->h = h;
    update();
}

void RenderArea::setNet(qreal x, qreal y, int m){
    this->x = x;
    this->y = y;
    this->mode = m;
    update();
}
