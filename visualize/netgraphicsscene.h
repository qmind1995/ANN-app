#ifndef NETGRAPHICSSCENE_H
#define NETGRAPHICSSCENE_H

#include "QGraphicsScene"
#include "ann/layer.h"
#include "graphLib/node.h"
#include "graphLib/edge.h"

class NetGraphicsScene: public QGraphicsScene
{
public:
    NetGraphicsScene(QObject *parent = 0);
    void visualNet(vector<Layer *> layers);
private:

};

#endif // NETGRAPHICSSCENE_H
