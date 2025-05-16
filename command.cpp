#include "command.h"

AddCommand::AddCommand(GraphicModel* model, ShapeType type, const QPointF& startPos,
                       const QColor& color, QUndoCommand* parent)
    : QUndoCommand(parent), model(model), type(type), startPos(startPos),
    color(color), shape(nullptr), myFirstTime(true)
{
    setText("Add shape");
}

void AddCommand::undo()
{
    model->removeShape(shape);
}

void AddCommand::redo()
{
    if (myFirstTime) {
        shape = new Shape(type, startPos, color);
        model->addShape(shape);
        myFirstTime = false;
    } else {
        model->addShape(shape);
    }
}

DeleteCommand::DeleteCommand(GraphicModel* model, Shape* shape, QUndoCommand* parent)
    : QUndoCommand(parent), model(model), shape(shape), wasAdded(true)
{
    setText("Delete shape");
    // При создании команды сразу удаляем фигуру
    model->removeShape(shape);
}

void DeleteCommand::undo()
{
    // Возвращаем фигуру на сцену
    model->addShape(shape);
    wasAdded = true;
}

void DeleteCommand::redo()
{
    // Снова удаляем фигуру
    if (wasAdded) {
        model->removeShape(shape);
        wasAdded = false;
    }
}

MoveCommand::MoveCommand(GraphicModel* model, Shape* shape, const QPointF& oldPos,
                         const QPointF& newPos, QUndoCommand* parent)
    : QUndoCommand(parent), model(model), shape(shape),
    myOldPos(oldPos), myNewPos(newPos)
{
    setText("Move shape");
}

void MoveCommand::undo()
{
    shape->setPos(myOldPos);
}

void MoveCommand::redo()
{
    shape->setPos(myNewPos);
}

bool MoveCommand::mergeWith(const QUndoCommand* command)
{
    const MoveCommand* moveCommand = static_cast<const MoveCommand*>(command);
    if (moveCommand->shape != shape)
        return false;

    myNewPos = moveCommand->myNewPos;
    return true;
}
