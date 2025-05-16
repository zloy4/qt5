#include "graphiccontroller.h"
#include <QInputDialog>
#include "command.h"

GraphicController::GraphicController(GraphicModel* model, QObject* parent)
    : QObject(parent), model(model), currentMode(EditorMode::Select),
    currentColor(Qt::black), currentShape(nullptr), isDrawing(false),
    isMoving(false), selectedShape(nullptr) {}

void GraphicController::setEditorMode(EditorMode mode) {
    currentMode = mode;
}

void GraphicController::setCurrentColor(const QColor& color) {
    currentColor = color;
}

void GraphicController::setCurrentText(const QString& text) {
    currentText = text;
}

QColor GraphicController::getCurrentColor() const {
    return currentColor;
}

void GraphicController::changeSelectedItemsColor(const QColor& color) {
    for (Shape* shape : model->getShapes()) {
        if (shape->isSelected()) {
            shape->setColor(color);
        }
    }
}

void GraphicController::mousePressed(const QPointF& pos) {
    if (currentMode == EditorMode::Select) {
        QList<QGraphicsItem*> items = model->getScene()->items(pos);
        for (QGraphicsItem* item : items) {
            Shape* shape = dynamic_cast<Shape*>(item);
            if (shape) {
                isMoving = true;
                selectedShape = shape;
                lastPos = shape->pos(); // Запоминаем начальную позицию
                return;
            }
        }
    }
    else {
        switch(currentMode) {
        case EditorMode::CreateLine:
            model->addShape(ShapeType::Line, pos, currentColor);
            break;
        case EditorMode::CreateRect:
            model->addShape(ShapeType::Rectangle, pos, currentColor);
            break;
        case EditorMode::CreateEllipse:
            model->addShape(ShapeType::Ellipse, pos, currentColor);
            break;
        case EditorMode::CreateTriangle:
            model->addShape(ShapeType::Triangle, pos, currentColor);
            break;
        case EditorMode::CreateText: {
            bool ok;
            QString text = QInputDialog::getText(nullptr, "Enter Text", "Text:",
                                                 QLineEdit::Normal, "", &ok);
            if (ok && !text.isEmpty()) {
                model->addShape(ShapeType::Text, pos, currentColor);
                Shape* shape = model->getShapes().last();
                shape->setText(text);
            }
            return;
        }
        default:
            return;
        }

        currentShape = model->getShapes().last();
        isDrawing = true;
    }
}

void GraphicController::mouseMoved(const QPointF& pos) {
    if (isMoving && selectedShape) {
        selectedShape->setPos(pos - selectedShape->boundingRect().center());
    }
    else if (isDrawing && currentShape) {
        currentShape->setEndPos(pos);
    }
}

void GraphicController::mouseReleased() {
    if (isMoving && selectedShape) {
        // Добавляем команду перемещения в стек отмены
        model->getUndoStack()->push(new MoveCommand(model, selectedShape,
                                                    lastPos, selectedShape->pos()));
    }
    isDrawing = false;
    isMoving = false;
    currentShape = nullptr;
    selectedShape = nullptr;
}

void GraphicController::deleteSelectedItems() {
    QList<Shape*> toRemove;
    // Сначала собираем все выделенные фигуры
    for (Shape* shape : model->getShapes()) {
        if (shape->isSelected()) {
            toRemove.append(shape);
        }
    }

    // Создаем одну команду для всех удалений
    if (!toRemove.isEmpty()) {
        model->getUndoStack()->beginMacro("Delete shapes");
        for (Shape* shape : toRemove) {
            new DeleteCommand(model, shape); // Команда сама удалит фигуру
        }
        model->getUndoStack()->endMacro();
    }
}

void GraphicController::clearAll() {
    model->clear();
}
