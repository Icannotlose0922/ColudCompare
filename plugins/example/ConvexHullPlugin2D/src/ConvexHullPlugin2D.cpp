//##########################################################################
//#                                                                        #
//#                CLOUDCOMPARE PLUGIN: ExamplePlugin                      #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 of the License.               #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#                             COPYRIGHT: XXX                             #
//#                                                                        #
//##########################################################################

// First:
//	Replace all occurrences of 'ExamplePlugin' by your own plugin class name in this file.
//	This includes the resource path to info.json in the constructor.

// Second:
//	Open ExamplePlugin.qrc, change the "prefix" and the icon filename for your plugin.
//	Change the name of the file to <yourPluginName>.qrc

// Third:
//	Open the info.json file and fill in the information about the plugin.
//	 "type" should be one of: "Standard", "GL", or "I/O" (required)
//	 "name" is the name of the plugin (required)
//	 "icon" is the Qt resource path to the plugin's icon (from the .qrc file)
//	 "description" is used as a tootip if the plugin has actions and is displayed in the plugin dialog
//	 "authors", "maintainers", and "references" show up in the plugin dialog as well

#include <QtGui>
#include <QInputDialog>

#include <ccMesh.h>
#include "ConvexHullPlugin2D.h"
#include "ccProgressDialog.h"
#include "ccPointCloud.h"

#include<fstream>
#include<string>
#include<sstream>

//ConvexHull
#include"Point2.h"
#include"ConvexHull2D.h"

//pcl
#include<pcl/point_types.h>
#include<pcl/visualization/cloud_viewer.h>
#include <pcl/visualization/pcl_plotter.h>

// Default constructor:
//	- pass the Qt resource path to the info.json file (from <yourPluginName>.qrc file) 
//  - constructor should mainly be used to initialize actions and other members
ConvexHullPlugin2D::ConvexHullPlugin2D(QObject* parent)
	: QObject(parent)
	, ccStdPluginInterface(":/CC/plugin/ConvexHullPlugin2D/info.json")
	, m_action(nullptr)
{
}

// This method should enable or disable your plugin actions
// depending on the currently selected entities ('selectedEntities').
void ConvexHullPlugin2D::onNewSelection(const ccHObject::Container& selectedEntities)
{
	if (m_action == nullptr)
	{
		return;
	}

	// If you need to check for a specific type of object, you can use the methods
	// in ccHObjectCaster.h or loop and check the objects' classIDs like this:
	//
	//	for ( ccHObject *object : selectedEntities )
	//	{
	//		if ( object->getClassID() == CC_TYPES::VIEWPORT_2D_OBJECT )
	//		{
	//			// ... do something with the viewports
	//		}
	//	}

	// For example - only enable our action if something is selected.
	m_action->setEnabled(!selectedEntities.empty());
}

// This method returns all the 'actions' your plugin can perform.
// getActions() will be called only once, when plugin is loaded.
QList<QAction*> ConvexHullPlugin2D::getActions()
{
	// default action (if it has not been already created, this is the moment to do it)
	if (!m_action)
	{
		// Here we use the default plugin name, description, and icon,
		// but each action should have its own.
		m_action = new QAction(getName(), this);
		m_action->setToolTip(getDescription());
		m_action->setIcon(getIcon());

		// Connect appropriate signal
		connect(m_action, &QAction::triggered, this, &ConvexHullPlugin2D::doAction);
	}

	return { m_action };
}

// This is an example of an action's method called when the corresponding action
// is triggered (i.e. the corresponding icon or menu entry is clicked in CC's
// main interface). You can access most of CC's components (database,
// 3D views, console, etc.) via the 'm_app' variable (see the ccMainAppInterface
// class in ccMainAppInterface.h).
void ConvexHullPlugin2D::doAction()
{
	if (m_app == nullptr)		//这里的m_app其实就是指主窗体本身MainWindow
	{
		// m_app should have already been initialized by CC when plugin is loaded
		Q_ASSERT(false);

		return;
	}

	/*** HERE STARTS THE ACTION ***/

	// Put your code here
	//得到点云
	const ccHObject::Container& selectedEntities = m_app->getSelectedEntities();		//其实就是从主窗口中获取实体对象集合
	size_t num = selectedEntities.size();

	//进度条
	/*ccProgressDialog pDlg(true, 0);
	CCLib::GenericProgressCallback* pCallBack = &pDlg;
	if (pCallBack) {
		if(pCallBack->textCanBeEdited()) {
			pCallBack->setMethodTitle("Compute");
			pCallBack->setInfo(qPrintable(QString("wait......")));
		}
		pCallBack->update(0);
		pCallBack->start();
	}*/

	std::vector<ccHObject*> allCloud;

	for (size_t i = 0; i < num; i++) {
		ccHObject* ent = selectedEntities[i];	//遍历每一个实体对象
		if (!ent->isA(CC_TYPES::POINT_CLOUD)) {
			continue;
		}

		ccPointCloud* cloud = ccHObjectCaster::ToPointCloud(ent);

		//二维凸包算法实现
		std::vector<double> x, y, z;		//存储点集中的x和z值
		std::vector<Point2> point, convexPoint;
		int minIndex, maxIndex;
		double zMin, zMax;
		z.resize(cloud->size());		//创建存储z坐标值的向量
		for (int i = 0; i < cloud->size(); i++)
		{
			z[i] = (cloud->getPoint(i))->z;
			x.push_back((cloud->getPoint(i))->x);
			y.push_back((cloud->getPoint(i))->y);
			point.push_back(Point2((cloud->getPoint(i))->x, (cloud->getPoint(i))->y));
		}

		//获取点云数据的高程的最大最小值
		minIndex = min_element(z.begin(), z.end()) - z.begin();		//获取最小值和最大值索引
		maxIndex = max_element(z.begin(), z.end()) - z.begin();
		zMin = z[minIndex];
		zMax = z[maxIndex];

		//显示切片
		
		pcl::visualization::PCLPlotter plot;
		plot.addPlotData(x, y, "Y Axis", vtkChart::POINTS);
	
		//获取凸包点
		ConvexHull2D convexHull(point);
		convexPoint = convexHull.getConvexPoints();

		std::vector<double> x0, y0;
		for (size_t i = 0; i < convexPoint.size(); i++)
		{
			x0.push_back(convexPoint[i].x);
			y0.push_back(convexPoint[i].y);
		}
		x0.push_back(convexPoint[0].x);
		y0.push_back(convexPoint[0].y);

		plot.addPlotData(x0, y0);		//默认的情况就是按顺序进行连线，vtkChart::LINE
		plot.plot();
		//pCallBack->update((float)100.0*i / num);
	}

	/*if (pCallBack)
		pCallBack->stop();*/

		//新建一个文件夹存放点云
		//ccHObject* cloudGroup = new ccHObject(QString("CloudGroup"));		//ccHObject更像是一个对象节点
		//for (size_t i = 0; i < allCloud.size(); i++) {
		//	cloudGroup->addChild(allCloud[i]);
		//}

		//m_app->addToDB(cloudGroup);
		//刷新
	m_app->refreshAll();
	m_app->updateUI();

	/*** HERE ENDS THE ACTION ***/
}
