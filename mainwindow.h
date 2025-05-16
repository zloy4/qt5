#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QToolBar>
#include <QColorDialog>
#include <QFontDialog>
#include <QInputDialog>
#include <QKeyEvent>
#include "graphicmodel.h"
#include "graphiccontroller.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onSelectAction();
    void onLineAction();
    void onRectAction();
    void onEllipseAction();
    void onTextAction();
    void onTriangleAction();
    void onColorAction();
    void onDeleteAction();
    void onClearAction();
    void onEditTextAction(); // Новый слот для редактирования текста

    void handleMousePressed(const QPointF& pos);
    void handleMouseMoved(const QPointF& pos);
    void handleMouseReleased();
    void onUndoAction();
    void onRedoAction();

private:
    void setupUI();
    void setupToolBar();
    void setupConnections();

    QGraphicsView* view;
    QToolBar* toolBar;
    QToolBar* textToolBar;

    GraphicModel* model;
    GraphicController* controller;

    Shape* getSelectedTextShape();
};

#endif // MAINWINDOW_H
