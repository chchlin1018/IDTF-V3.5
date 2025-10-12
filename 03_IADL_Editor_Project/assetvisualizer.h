#ifndef ASSETVISUALIZER_H
#define ASSETVISUALIZER_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>

class AssetVisualizer : public QWidget
{
    Q_OBJECT

public:
    explicit AssetVisualizer(QWidget *parent = nullptr);
    
    void showRelationships();
    void showVisualization();

private:
    void setupUI();
    void createExampleGraph();
    
    QGraphicsView *graphicsView;
    QGraphicsScene *scene;
};

#endif // ASSETVISUALIZER_H
