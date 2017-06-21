#ifndef XLINE_H
#define XLINE_H

#include <QBrush>
#include <QPen>
#include <QWidget>
#include <QPainter>

class XLine : public QWidget
{
    Q_OBJECT

public:
    XLine(QWidget *parent = 0);
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;
    void setup(qreal, qreal);

public slots:
    void setPos(qreal);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    QPen pen;
    QBrush brush;
    qreal s;
    qreal l;
    qreal r;
    qreal pos;
};

#endif // XLINE_H
