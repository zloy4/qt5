#include "shape.h"
#include <QCursor>
#include <QGraphicsSceneMouseEvent>

Shape::Shape(ShapeType type, const QPointF& startPos, const QColor& color, QGraphicsItem* parent)
    : QGraphicsItem(parent), type(type), startPos(startPos), endPos(startPos),
    color(color), isEditing(false), currentHandle(None), isResizing(false) {
    font = QFont("Arial", 12); // Устанавливаем шрифт по умолчанию
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    setAcceptHoverEvents(true);
}

QRectF Shape::boundingRect() const {
    if (type == ShapeType::Text) {
        QFontMetricsF metrics(font);
        // Вычисляем границы текста с учетом выравнивания
        QRectF textRect = metrics.boundingRect(QRectF(), Qt::AlignLeft | Qt::AlignTop, text);
        // Смещаем прямоугольник к startPos (верхний левый угол текста)
        textRect.moveTo(startPos);
        // Добавляем отступы для рамки
        return textRect.adjusted(-5, -5, 5, 5); // Уменьшаем отступы для более плотной рамки
    }
    QRectF rect(startPos, endPos);
    rect = rect.normalized().adjusted(-10, -10, 10, 10);
    return rect;
}

void Shape::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(QPen(color, 2));
    painter->setFont(font);

    switch(type) {
    case ShapeType::Line:
        painter->drawLine(startPos, endPos);
        break;
    case ShapeType::Rectangle:
        painter->drawRect(QRectF(startPos, endPos));
        break;
    case ShapeType::Ellipse:
        painter->drawEllipse(QRectF(startPos, endPos));
        break;
    case ShapeType::Triangle: {
        QPolygonF triangle;
        triangle << QPointF((startPos.x() + endPos.x()) / 2, startPos.y()) // Верхняя вершина
                 << QPointF(endPos.x(), endPos.y())                        // Правая нижняя
                 << QPointF(startPos.x(), endPos.y());                      // Левая нижняя
        painter->drawPolygon(triangle);
        break;
    }
    case ShapeType::Text:
        if (!isEditing) {
            // Рисуем текст, интерпретируя startPos как верхний левый угол
            painter->drawText(QRectF(startPos, QSizeF(1000, 1000)), Qt::AlignLeft | Qt::AlignTop, text);
        }
        break;
    }

    if (isSelected() || isEditing) {
        painter->setPen(QPen(Qt::blue, 1, Qt::DashLine));
        QRectF rect = boundingRect().adjusted(5, 5, -5, -5);
        painter->drawRect(rect);

        if (type != ShapeType::Text) {
            painter->setBrush(Qt::white);
            painter->setPen(QPen(Qt::black, 1));

            for (int i = 1; i <= 4; ++i) {
                QRectF handle = getHandleRect(static_cast<ResizeHandle>(i));
                painter->drawRect(handle);
            }
        }
    }
}

// Остальные методы остаются без изменений
Shape::ResizeHandle Shape::getResizeHandle(const QPointF& pos) const {
    if (type == ShapeType::Text) return None;

    for (int i = 1; i <= 4; ++i) {
        ResizeHandle handle = static_cast<ResizeHandle>(i);
        if (getHandleRect(handle).contains(pos)) {
            return handle;
        }
    }
    return None;
}

QRectF Shape::getHandleRect(ResizeHandle handle) const {
    QRectF rect(startPos, endPos);
    rect = rect.normalized();
    const qreal handleSize = 8;

    switch(handle) {
    case TopLeft:     return QRectF(rect.topLeft() - QPointF(handleSize/2, handleSize/2),
                      QSizeF(handleSize, handleSize));
    case TopRight:    return QRectF(rect.topRight() - QPointF(handleSize/2, handleSize/2),
                      QSizeF(handleSize, handleSize));
    case BottomLeft:  return QRectF(rect.bottomLeft() - QPointF(handleSize/2, handleSize/2),
                      QSizeF(handleSize, handleSize));
    case BottomRight: return QRectF(rect.bottomRight() - QPointF(handleSize/2, handleSize/2),
                      QSizeF(handleSize, handleSize));
    default: return QRectF();
    }
}

void Shape::setEndPos(const QPointF& endPos) {
    prepareGeometryChange();
    this->endPos = endPos;
    update();
}

void Shape::setText(const QString& text) {
    prepareGeometryChange();
    this->text = text;
    update();
}

void Shape::setColor(const QColor& color) {
    this->color = color;
    update();
}

void Shape::setEditing(bool editing) {
    isEditing = editing;
    update();
}

void Shape::setFont(const QFont& font) {
    prepareGeometryChange();
    this->font = font;
    update();
}

QFont Shape::getFont() const {
    return font;
}

ShapeType Shape::getType() const { return type; }
QColor Shape::getColor() const { return color; }
QString Shape::getText() const { return text; }

void Shape::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        currentHandle = getResizeHandle(event->pos());
        isResizing = (currentHandle != None);

        if (isResizing) {
            resizeStartPos = startPos;
            resizeStartEnd = endPos;
        }
    }
    QGraphicsItem::mousePressEvent(event);
}

void Shape::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    prepareGeometryChange();
    if (isResizing && (event->buttons() & Qt::LeftButton)) {
        QPointF delta = event->scenePos() - event->lastScenePos();

        switch(currentHandle) {
        case TopLeft:
            startPos += delta;
            break;
        case TopRight:
            endPos.setX(endPos.x() + delta.x());
            startPos.setY(startPos.y() + delta.y());
            break;
        case BottomLeft:
            startPos.setX(startPos.x() + delta.x());
            endPos.setY(endPos.y() + delta.y());
            break;
        case BottomRight:
            endPos += delta;
            break;
        default:
            break;
        }
        update();
    } else {
        QGraphicsItem::mouseMoveEvent(event);
    }
}

void Shape::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    isResizing = false;
    currentHandle = None;
    QGraphicsItem::mouseReleaseEvent(event);
}

void Shape::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
    ResizeHandle handle = getResizeHandle(event->pos());

    switch(handle) {
    case TopLeft:
    case BottomRight:
        setCursor(Qt::SizeFDiagCursor);
        break;
    case TopRight:
    case BottomLeft:
        setCursor(Qt::SizeBDiagCursor);
        break;
    default:
        setCursor(Qt::ArrowCursor);
    }

    QGraphicsItem::hoverMoveEvent(event);
}
