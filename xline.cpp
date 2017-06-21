#include "xline.h"
#include <QDebug>

XLine::XLine(QWidget *parent)
    : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    l = -1234;
    r = -l;
    pos = 0;
    s = 1;
}

QSize XLine::minimumSizeHint() const
{
    return QSize(100, 10);
}

QSize XLine::sizeHint() const
{
    return QSize(400, 200);
}

void XLine::paintEvent(QPaintEvent *)
{ 
    QPainter painter(this);
    pen.setColor(QColor(0,0,0));
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.save();
    painter.drawText(QRect(0, 0, 50, height()/2), Qt::AlignCenter, (QString::number(l/s, 'f', 2) + " mm"));
    painter.drawText(QRect(0,height()/2, 50, height()/2 - 1), Qt::AlignCenter, (QString::number(l) + " st"));
    painter.drawText(QRect(width() - 50, 0, 50, height()/2), Qt::AlignCenter, (QString::number(r/s, 'f', 2) + " mm"));
    painter.drawText(QRect(width() - 50,height()/2, 50, height()/2 - 1), Qt::AlignCenter, (QString::number(r) + " st"));
    painter.drawText(QRect(width()/2 - 25, 0, 50, height()/2), Qt::AlignCenter, (QString::number(pos/s, 'f', 2) + " mm"));
    painter.drawText(QRect(width()/2 - 25,height()/2, 50, height()/2 - 1), Qt::AlignCenter, (QString::number(pos) + " st"));
    painter.drawLine(50, height()/2 - 1, width() - 50, height()/2 - 1);
    pen.setColor(QColor(255,0,0));
    painter.setPen(pen);
    painter.drawEllipse(QPoint(50 + (width() - 100)*(pos - l)/(r - l),height()/2 - 1), 2, 2);
    painter.restore();
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(palette().dark().color());
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));
}

void XLine::setup(qreal r, qreal s){
    this->r = r;
    this->l = -r;
    this->s = s;
    update();
}

void XLine::setPos(qreal s){
    pos = -s - r;
    update();
}
