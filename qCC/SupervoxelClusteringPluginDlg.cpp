#include "SupervoxelClusteringPluginDlg.h"
//#include "ui_testplugindlg.h"

SupervoxelClusteringPluginDlg::SupervoxelClusteringPluginDlg(QWidget *parent) :
    QDialog(parent),
    Ui::SupervoxelClusteringPluginDlg()		//���ø���Ĺ��캯����ʵ�������ӻ����沿�֣���Ϊ����Ĺ��캯��Ϊprivate��������Ҫ��һ��
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
