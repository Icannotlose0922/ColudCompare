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

//�򵥵ĵ�ѡ����
class ccClusterByPickPoin : public ccPointPickingGenericInterface, public Ui::ClusterByPickPointDlg
{
	Q_OBJECT

public:

	//! Default constructor
	explicit ccClusterByPickPoinDlg(ccPickingHub* pickingHub, QWidget* parent);
	//! Default destructor
	virtual ~ccClusterByPickPoinDlg();

	//�̳�ccPointPickingGenericInterface�ӿ�
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

	//�ú����̳���ccPointPickingGenericInterface�࣬��Ҫ���ڴ���ѡ�����һ�¼�
	void processPickedPoint(const PickedItem& picked) override;

	//��ǰѡ���ģʽ
	Mode m_pickingMode;

	//ѡ�е�
	ccPointCloud* selectPoint;

	//���������ʵ��
	QSet<ccHObject*> m_entities;

	//�������
	std::vector<ccPointCloud*> m_clusterClouds;
public:
	//ͨ���ú�����ӵ���ʵ��
	bool addEntity(ccHObject* entity);

	//��ȡ������
	std::vector<ccPointCloud*> getClusterClouds();
	void clearCluster();
};

#endif
