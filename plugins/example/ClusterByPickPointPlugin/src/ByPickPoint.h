//##########################################################################
//#                                                                        #
//#                              CLOUDCOMPARE                              #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 or later of the License.      #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
//#                                                                        #
//##########################################################################

#ifndef CC_CLUSTER_BY_POINT_HEADER
#define CC_CLUSTER_BY_POINT_HEADER

#include "ccPointPickingGenericInterface.h"

//Local
#include <ui_ClusterByPickPointDlg.h>

//Qt
#include <QSet>

class ccHObject;

//简单的点选窗口
class ccClusterByPickPoin : public ccPointPickingGenericInterface, public Ui::ClusterByPickPointDlg
{
	Q_OBJECT

public:

	//! Default constructor
	explicit ccClusterByPickPoinDlg(ccPickingHub* pickingHub, QWidget* parent);
	//! Default destructor
	virtual ~ccClusterByPickPoinDlg();

	//继承ccPointPickingGenericInterface接口
	virtual bool start() override;
	virtual void stop(bool state) override;
	virtual bool linkWith(ccGLWindow* win) override;

protected slots:

	void onClose();
	void activatePointPropertiesDisplay();
	void activateDistanceDisplay();
	void initializeState();
	//void exportData();

protected:

	//! Picking mode
	enum Mode
	{
		POINT_INFO,
		POINT_POINT_DISTANCE,
	};

	//该函数继承自ccPointPickingGenericInterface类，主要用于处理选择点这一事件
	void processPickedPoint(const PickedItem& picked) override;

	//当前选择的模式
	Mode m_pickingMode;

	//选中点
	ccPointCloud* selectPoint;

	//主窗体点云实体
	QSet<ccHObject*> m_entities;

	//聚类点云
	std::vector<ccPointCloud*> m_clusterClouds;
public:
	//通过该函数添加点云实体
	bool addEntity(ccHObject* entity);

	//获取聚类结果
	std::vector<ccPointCloud*> getClusterClouds();
	void clearCluster();
};

#endif
