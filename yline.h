#ifndef YLINE_H
#define YLINE_H

#include <QBrush>
#include <QPen>
#include <QWidget>
#include <QPainter>

class YLine : public QWidget
{
    Q_OBJECT

public:
    YLine(QWidget *parent = 0);
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
    qreal u;
    qreal d;
    qreal pos;
};

#endif // YLINE_H
