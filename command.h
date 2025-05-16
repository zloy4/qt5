#ifndef COMMAND_H
#define COMMAND_H

#include <QUndoCommand>
#include "graphicmodel.h"
#include "shape.h"

class AddCommand : public QUndoCommand
{
public:
    AddCommand(GraphicModel* model, ShapeType type, const QPointF& startPos,
               const QColor& color, QUndoCommand* parent = nullptr);
    void undo() override;
    void redo() override;

private:
    GraphicModel* model;
    Shape* shape;
    ShapeType type;
    QPointF startPos;
    QColor color;
    bool myFirstTime;
};

class DeleteCommand : public QUndoCommand
{
public:
    DeleteCommand(GraphicModel* model, Shape* shape, QUndoCommand* parent = nullptr);
    void undo() override;
    void redo() override;

private:
    GraphicModel* model;
    Shape* shape;
    bool wasAdded; // Флаг, указывающий, была ли фигура добавлена в модель
};

class MoveCommand : public QUndoCommand
{
public:
    MoveCommand(GraphicModel* model, Shape* shape, const QPointF& oldPos,
                const QPointF& newPos, QUndoCommand* parent = nullptr);
    void undo() override;
    void redo() override;
    bool mergeWith(const QUndoCommand* command) override;
    int id() const override { return 1; }

private:
    GraphicModel* model;
    Shape* shape;
    QPointF myOldPos;
    QPointF myNewPos;
};

#endif // COMMAND_H
