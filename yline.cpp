#include "yline.h"

YLine::YLine(QWidget *parent)
    : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    u = 1000;
    d = 0;
    pos = u;
    s = 1;
}

QSize YLine::minimumSizeHint() const
{
    return QSize(10, 100);
}

QSize YLine::sizeHint() const
{
    return QSize(400, 200);
}

void YLine::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.rotate(90);
    pen.setColor(QColor(0,0,0));
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.save();
    painter.drawText(QRect(0, -width(), 50, width()/2), Qt::AlignCenter, (QString::number(u/s, 'f', 1) + " mm"));
    painter.drawText(QRect(0, -width()/2, 50, width()/2), Qt::AlignCenter, (QString::number(u) + " st"));
    painter.drawText(QRect(height() - 50, -width(), 50, width()/2), Qt::AlignCenter, (QString::number(d/s, 'f', 1) + " mm"));
    painter.drawText(QRect(height() - 50, -width()/2, 50, width()/2 - 1), Qt::AlignCenter, (QString::number(d) + " st"));
    painter.drawText(QRect(height()/2 - 25, -width(), 50, width()/2), Qt::AlignCenter, (QString::number(pos/s, 'f', 1) + " mm"));
    painter.drawText(QRect(height()/2 - 25, -width()/2, 50, width()/2 - 1), Qt::AlignCenter, (QString::number(pos) + " st"));
    painter.drawLine(50, - width()/2 - 1, height() - 50, -width()/2 - 1);
    pen.setColor(QColor(255,0,0));
    painter.setPen(pen);
    painter.drawEllipse(QPoint(50 + (height() - 100)*(pos - u)/(d - u),-width()/2 - 1), 2, 2);
    painter.restore();
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(palette().dark().color());
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(0, 0, height() - 1, width() - 1));

}

void YLine::setup(qreal u, qreal s){
    this->u = u;
    this->d = 0;
    this->s = s;
    update();
}

void YLine::setPos(qreal s){
    pos = -s;
    update();
}
