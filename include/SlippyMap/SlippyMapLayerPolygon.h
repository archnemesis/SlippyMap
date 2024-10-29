#ifndef SLIPPYMAPWIDGETPOLYGON_H
#define SLIPPYMAPWIDGETPOLYGON_H

#include <SlippyMap/SlippyMapLayerObject.h>

#define SLIPPYMAPSHARED_EXPORT

#include <QObject>
#include <QVector>
#include <QPen>
#include <QBrush>
#include <QPainter>


using namespace SlippyMap;

class SlippyMapLayerPolygon : public SlippyMapLayerObject
{
    Q_OBJECT
public:
    explicit SlippyMapLayerPolygon(QVector<QPointF> points, QObject *parent = nullptr);
    ~SlippyMapLayerPolygon() override;
    void draw(QPainter *painter, const QTransform &transform, ObjectState state) override;
    QVector<QPointF> points();
    bool isIntersectedBy(const QRectF& rect) const override;
    bool contains(const QPointF& point, int zoom) const override;
    const QPointF position() const override;
    const QSizeF size() const override;

protected:
    QVector<QPointF> m_points;

    bool test_point(const QPointF& a, const QPointF& b, const QPointF& p) const;
};

#endif // SLIPPYMAPWIDGETPOLYGON_H
