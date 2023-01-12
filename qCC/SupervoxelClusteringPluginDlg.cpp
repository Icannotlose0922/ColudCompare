#include "SupervoxelClusteringPluginDlg.h"
//#include "ui_testplugindlg.h"

SupervoxelClusteringPluginDlg::SupervoxelClusteringPluginDlg(QWidget *parent) :
    QDialog(parent),
    Ui::SupervoxelClusteringPluginDlg()		//调用父类的构造函数，实例化可视化界面部分，因为该类的构造函数为private，所以需要这一步
{
    setupUi(this);
	connect(pushButtonOK, SIGNAL(clicked()), this, SLOT(actionOK()));
}

SupervoxelClusteringPluginDlg::~SupervoxelClusteringPluginDlg()
{
}

void SupervoxelClusteringPluginDlg::actionOK() {
	accept();
}
