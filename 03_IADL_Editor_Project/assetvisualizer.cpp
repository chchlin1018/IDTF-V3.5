#include "assetvisualizer.h"
#include <QVBoxLayout>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QPen>
#include <QBrush>

AssetVisualizer::AssetVisualizer(QWidget *parent) :
    QWidget(parent)
{
    setupUI();
    createExampleGraph();
}

void AssetVisualizer::setupUI()
{
    // Create graphics view and scene
    graphicsView = new QGraphicsView(this);
    scene = new QGraphicsScene(this);
    graphicsView->setScene(scene);
    graphicsView->setRenderHint(QPainter::Antialiasing);
    
    // Set layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(graphicsView);
    setLayout(layout);
}

void AssetVisualizer::createExampleGraph()
{
    // Clear previous scene
    scene->clear();
    
    // Create nodes
    QGraphicsEllipseItem *pumpNode = new QGraphicsEllipseItem(0, 0, 80, 80);
    pumpNode->setBrush(QBrush(Qt::lightGray));
    pumpNode->setPen(QPen(Qt::black, 2));
    pumpNode->setPos(100, 100);
    scene->addItem(pumpNode);
    
    QGraphicsTextItem *pumpText = new QGraphicsTextItem("Pump\nP-101");
    pumpText->setPos(115, 125);
    scene->addItem(pumpText);
    
    QGraphicsEllipseItem *motorNode = new QGraphicsEllipseItem(0, 0, 80, 80);
    motorNode->setBrush(QBrush(Qt::lightGray));
    motorNode->setPen(QPen(Qt::black, 2));
    motorNode->setPos(300, 100);
    scene->addItem(motorNode);
    
    QGraphicsTextItem *motorText = new QGraphicsTextItem("Motor\nM-101");
    motorText->setPos(315, 125);
    scene->addItem(motorText);
    
    QGraphicsEllipseItem *valveNode = new QGraphicsEllipseItem(0, 0, 80, 80);
    valveNode->setBrush(QBrush(Qt::lightGray));
    valveNode->setPen(QPen(Qt::black, 2));
    valveNode->setPos(100, 300);
    scene->addItem(valveNode);
    
    QGraphicsTextItem *valveText = new QGraphicsTextItem("Valve\nV-101");
    valveText->setPos(115, 325);
    scene->addItem(valveText);
    
    // Create edges
    QGraphicsLineItem *pumpToMotor = new QGraphicsLineItem(180, 140, 300, 140);
    pumpToMotor->setPen(QPen(Qt::black, 2));
    scene->addItem(pumpToMotor);
    
    QGraphicsTextItem *pumpToMotorText = new QGraphicsTextItem("drives");
    pumpToMotorText->setPos(220, 120);
    scene->addItem(pumpToMotorText);
    
    QGraphicsLineItem *pumpToValve = new QGraphicsLineItem(140, 180, 140, 300);
    pumpToValve->setPen(QPen(Qt::black, 2));
    scene->addItem(pumpToValve);
    
    QGraphicsTextItem *pumpToValveText = new QGraphicsTextItem("connects to");
    pumpToValveText->setPos(145, 230);
    scene->addItem(pumpToValveText);
    
    // Fit the view to the scene content
    graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void AssetVisualizer::showRelationships()
{
    // For prototype, just recreate the example graph
    createExampleGraph();
}

void AssetVisualizer::showVisualization()
{
    // For prototype, just recreate the example graph with different style
    scene->clear();
    
    // Create a simple process flow diagram
    QGraphicsRectItem *tankRect = new QGraphicsRectItem(0, 0, 100, 150);
    tankRect->setBrush(QBrush(Qt::lightGray));
    tankRect->setPen(QPen(Qt::black, 2));
    tankRect->setPos(50, 100);
    scene->addItem(tankRect);
    
    QGraphicsTextItem *tankText = new QGraphicsTextItem("Tank\nT-101");
    tankText->setPos(75, 170);
    scene->addItem(tankText);
    
    QGraphicsEllipseItem *pumpCircle = new QGraphicsEllipseItem(0, 0, 80, 80);
    pumpCircle->setBrush(QBrush(Qt::lightGray));
    pumpCircle->setPen(QPen(Qt::black, 2));
    pumpCircle->setPos(250, 135);
    scene->addItem(pumpCircle);
    
    QGraphicsTextItem *pumpText = new QGraphicsTextItem("Pump\nP-101");
    pumpText->setPos(265, 160);
    scene->addItem(pumpText);
    
    QGraphicsRectItem *heatExRect = new QGraphicsRectItem(0, 0, 150, 100);
    heatExRect->setBrush(QBrush(Qt::lightGray));
    heatExRect->setPen(QPen(Qt::black, 2));
    heatExRect->setPos(400, 125);
    scene->addItem(heatExRect);
    
    QGraphicsTextItem *heatExText = new QGraphicsTextItem("Heat Exchanger\nHE-101");
    heatExText->setPos(415, 160);
    scene->addItem(heatExText);
    
    // Create connecting lines
    QGraphicsLineItem *tankToPump = new QGraphicsLineItem(150, 175, 250, 175);
    tankToPump->setPen(QPen(Qt::black, 2));
    scene->addItem(tankToPump);
    
    QGraphicsLineItem *pumpToHeatEx = new QGraphicsLineItem(330, 175, 400, 175);
    pumpToHeatEx->setPen(QPen(Qt::black, 2));
    scene->addItem(pumpToHeatEx);
    
    // Add flow direction arrows
    QPolygonF arrow1;
    arrow1 << QPointF(240, 175) << QPointF(230, 170) << QPointF(230, 180);
    QGraphicsPolygonItem *arrowItem1 = new QGraphicsPolygonItem(arrow1);
    arrowItem1->setBrush(QBrush(Qt::black));
    scene->addItem(arrowItem1);
    
    QPolygonF arrow2;
    arrow2 << QPointF(390, 175) << QPointF(380, 170) << QPointF(380, 180);
    QGraphicsPolygonItem *arrowItem2 = new QGraphicsPolygonItem(arrow2);
    arrowItem2->setBrush(QBrush(Qt::black));
    scene->addItem(arrowItem2);
    
    // Fit the view to the scene content
    graphicsView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}
