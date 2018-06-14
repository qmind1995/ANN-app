#include "netgraphicsscene.h"

NetGraphicsScene::NetGraphicsScene(QObject *parent){
}

void NetGraphicsScene::visualNet(vector<Layer *> layers){
    vector<Node *> prevLayerNodes, currLayerNodes;

    for(int l=0; l< layers.size(); l++){
        Layer * layeri = layers[l];
        int nNeural = layeri->nNeurals;
        Qt::GlobalColor color0 = Qt::GlobalColor(l %5 +8),
            color1= Qt::GlobalColor(l %5 + 14);
        for(int j=0; j< nNeural; j++){
            Node *node = new Node(this);
            this->addItem(node);
            node->setPos(l*200, j*30);
            node->setColor(color0, color1);
            currLayerNodes.push_back(node);
        }

        if(layeri->isBias){
            Node *bias = new Node(this);
            this->addItem(bias);
            bias->setColor(Qt::red, Qt::darkRed);
            bias->setPos(l*200, nNeural * 30);
            currLayerNodes.push_back(bias);
        }

        if(prevLayerNodes.size() >0){
            for(int i=0; i< prevLayerNodes.size(); i++){
                for(int j=0; j< nNeural; j++){
                    this->addItem(new Edge(prevLayerNodes[i], currLayerNodes[j]));
                }
            }
        }
        prevLayerNodes.clear();
        prevLayerNodes = currLayerNodes;
        currLayerNodes.clear();
    }
}
