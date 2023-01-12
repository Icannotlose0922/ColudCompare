#ifndef SUPERVOXELCLUSTERINGDLGPLUGIN_H
#define SUPERVOXELCLUSTERINGDLGPLUGIN_H

#include "ui_supervoxelclusteringplugindlg.h"

class SupervoxelClusteringPluginDlg : public QDialog,public Ui::SupervoxelClusteringPluginDlg
{
    Q_OBJECT

public:
    explicit SupervoxelClusteringPluginDlg(QWidget *parent = 0);
    ~SupervoxelClusteringPluginDlg();

private slots:
	void actionOK();
};

#endif // TESTPLUGINDLG_H
