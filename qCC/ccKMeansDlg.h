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

#ifndef CC_KMEANS_DIALOG_HEADER
#define CC_KMEANS_DIALOG_HEADER

//GUI
#include <ui_KMeansDlg.h>

//Local
#include "ccPointPickingGenericInterface.h"

//qCC_db
#include <ccHObject.h>

class cc2DLabel;

class ccKMeansDlg : public ccPointPickingGenericInterface, public Ui::KMeansDlg
{
	Q_OBJECT

public:

	explicit ccKMeansDlg(ccPickingHub* pickingHub, QWidget* parent);

	//将一个点云与对话框进行相连接
	void linkWithEntity(ccHObject* entity);

	//获取选中的点
	std::vector<size_t> getSelectedPoints();

	//清除之前所选的点
	void clearPoints();

	//获取应用状态
	bool getState();

protected slots:

	void applyAndExit();

	void cancelAndExit();

	//从列表中删除上一个点
	void removeLastEntry();

	//当标记大小改变时重新绘制窗口
	void markerSizeChanged(int);
	//开始索引发生改变
	void startIndexChanged(int);
	//更新点列表
	void updateList();

protected:

	//该函数会获取鼠标发送过来的消息
	void processPickedPoint(const PickedItem& picked) override;

	//获取当前选中的点
	unsigned getPickedPoints(std::vector<cc2DLabel*>& pickedPoints);

	ccHObject* m_associatedEntity;

	unsigned m_lastPreviousID;

	ccHObject* m_orderedLabelsContainer;
	//删除的点
	ccHObject::Container m_toBeDeleted;
	//添加的点
	ccHObject::Container m_toBeAdded;

	//选中的点
	std::vector<size_t> selectedPoints;

	//设置一个开关来表示状态
	bool state;
};

#endif
