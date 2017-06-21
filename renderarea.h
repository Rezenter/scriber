#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QBrush>
#include <QPen>
#include <QWidget>
#include <QMouseEvent>

class RenderArea : public QWidget
{
    Q_OBJECT

public:
    RenderArea(QWidget *parent = 0);

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

public slots:
    void setEll(qreal, qreal, qreal, qreal);
    void setNet(qreal, qreal, int);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    QPair<qreal, qreal> ell;
    QPen pen;
    QBrush brush;
    qreal l;
    void mouseMoveEvent(QMouseEvent *);
    QPoint pos;
    qreal maxMm = 90;
    qreal maxPix = 250;
    qreal h = 0;
    qreal moveX;
    qreal moveY;
    qreal cut;
    qreal ang;
    qreal x;
    qreal y;
    int mode;
};

#endif // RENDERAREA_H
