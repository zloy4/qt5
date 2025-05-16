#include "graphicmodel.h"
#include "command.h"

GraphicModel::GraphicModel(QObject* parent) : QObject(parent) {
    scene = new CustomGraphicsScene(this);
    scene->setSceneRect(-500, -500, 1000, 1000);
    undoStack = new QUndoStack(this);
}

GraphicModel::~GraphicModel() {
    clear();
}

void GraphicModel::addShape(ShapeType type, const QPointF& startPos, const QColor& color) {
    Shape* shape = new Shape(type, startPos, color);
    undoStack->push(new AddCommand(this, type, startPos, color));
    emit sceneUpdated();
}

void GraphicModel::addShape(Shape* shape) {
    shapes.append(shape);
    scene->addItem(shape);
    emit sceneUpdated();
}

void GraphicModel::removeShape(Shape* shape) {
    if (shapes.removeOne(shape)) {
        scene->removeItem(shape);
        emit sceneUpdated();
    }
}

void GraphicModel::clear() {
    undoStack->clear();
    for (Shape* shape : shapes) {
        scene->removeItem(shape);
        delete shape;
    }
    shapes.clear();
    emit sceneUpdated();
}

QList<Shape*> GraphicModel::getShapes() const {
    return shapes;
}

CustomGraphicsScene* GraphicModel::getScene() const {
    return scene;
}

QUndoStack* GraphicModel::getUndoStack() const {
    return undoStack;
}
