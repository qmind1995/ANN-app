#ifndef CREATELAYERDIALOG_H
#define CREATELAYERDIALOG_H

#include <QDialog>
#include <QDialog>
#include "QLabel"
#include "QPushButton"
#include "QHBoxLayout"
#include "QVBoxLayout"
#include "iostream"
#include "QComboBox"
#include "QSpinBox"
#include "ann/layer.h"

namespace Ui {
class CreateLayerDialog;
}

class CreateLayerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateLayerDialog(QWidget *parent = 0);
    ~CreateLayerDialog();
    void addWidget(int nLayer);
    vector<Layer*> layers;
private slots:
    void on_acceptBtn_clicked();

private:
    Ui::CreateLayerDialog *ui;
    int nLayers;
    vector<QComboBox*> qcbActiveFunc;
    vector<QComboBox*> qcbIsBias;
    vector<QSpinBox*> qsp;

};

#endif // CREATELAYERDIALOG_H
