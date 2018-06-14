#include "createlayerdialog.h"
#include "ui_createlayerdialog.h"

CreateLayerDialog::CreateLayerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateLayerDialog){
    ui->setupUi(this);
}

CreateLayerDialog::~CreateLayerDialog(){
    delete ui;
}

void CreateLayerDialog::addWidget(int nLayer){
    for(int i=0; i<nLayer; i++){
        QLabel *li =new QLabel("Layer "+  QString::number(i+1));

        QComboBox *cBiasi = new QComboBox;
        cBiasi->addItem("bias");
        cBiasi->addItem("no bias");

        QSpinBox *si = new QSpinBox;
        si->setMaximum(10000);
        si->setMinimum(1);
        QComboBox *cActiFunc = new QComboBox;
        cActiFunc->addItem("None");
        cActiFunc->addItem("Sigmoid");
        cActiFunc->addItem("Tanh");

        QHBoxLayout * hi = new QHBoxLayout;
        hi->addWidget(li);
        hi->addWidget(si);
        qsp.push_back(si);
        hi->addWidget(cBiasi);
        qcbIsBias.push_back(cBiasi);
        hi->addWidget(cActiFunc);
        qcbActiveFunc.push_back(cActiFunc);
        ui->boundVerticalLayout->addLayout(hi);
    }

    this->nLayers = nLayer;

}

void CreateLayerDialog::on_acceptBtn_clicked(){

    for (int i = 0; i < nLayers; ++i){
        int nNeurali = qsp[i]->value();
        int biasSelecIndexi =qcbIsBias[i]->currentIndex();
        int activeSelecIndexi =qcbActiveFunc[i]->currentIndex();

        bool isBias = true;
        if(biasSelecIndexi == 1){
            isBias = false;
        }

        Layer * l = new Layer(nNeurali, isBias, activeSelecIndexi);
        layers.push_back(l);
    }
    this->accept();
}
