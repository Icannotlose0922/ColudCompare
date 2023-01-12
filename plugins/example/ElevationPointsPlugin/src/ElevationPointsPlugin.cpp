//##########################################################################
//#                                                                        #
//#                CLOUDCOMPARE PLUGIN: ElevationPointsPlugin                      #
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
//	Replace all occurrences of 'ElevationPointsPlugin' by your own plugin class name in this file.
//	This includes the resource path to info.json in the constructor.

// Second:
//	Open ElevationPointsPlugin.qrc, change the "prefix" and the icon filename for your plugin.
//	Change the name of the file to <yourPluginName>.qrc

// Third:
//	Open the info.json file and fill in the information about the plugin.
//	 "type" should be one of: "Standard", "GL", or "I/O" (required)
//	 "name" is the name of the plugin (required)
//	 "icon" is the Qt resource path to the plugin's icon (from the .qrc file)
//	 "description" is used as a tootip if the plugin has actions and is displayed in the plugin dialog
//	 "authors", "maintainers", and "references" show up in the plugin dialog as well

#include <QtGui>
#include <QInputDialog> //对话框
#include "ElevationPointsPlugin.h"
#include <ccPointCloud.h>
// Default constructor:
//	- pass the Qt resource path to the info.json file (from <yourPluginName>.qrc file) 
//  - constructor should mainly be used to initialize actions and other members
ElevationPointsPlugin::ElevationPointsPlugin(QObject* parent)
	: QObject(parent)
	, ccStdPluginInterface(":/CC/plugin/ElevationPointsPlugin/info.json")
	, m_action(nullptr)
{
}

// This method should enable or disable your plugin actions
// depending on the currently selected entities ('selectedEntities').
void ElevationPointsPlugin::onNewSelection(const ccHObject::Container& selectedEntities)
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
QList<QAction*> ElevationPointsPlugin::getActions()
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
		connect(m_action, &QAction::triggered, this, &ElevationPointsPlugin::doAction);
	}

	return { m_action };
}

// This is an example of an action's method called when the corresponding action
// is triggered (i.e. the corresponding icon or menu entry is clicked in CC's
// main interface). You can access most of CC's components (database,
// 3D views, console, etc.) via the 'm_app' variable (see the ccMainAppInterface
// class in ccMainAppInterface.h).
void ElevationPointsPlugin::doAction()
{
	if (m_app == nullptr)
	{
		// m_app should have already been initialized by CC when plugin is loaded
		Q_ASSERT(false);

		return;
	}

	/*** HERE STARTS THE ACTION ***/
	// 1.获取所选点云
	ccPointCloud* cloud;
	const ccHObject::Container& selectedEntities = m_app->getSelectedEntities();
	if (selectedEntities.size() != 1)
	{
		m_app->dispToConsole("[ElevationPointsPlugin] selectedEntities.size() != 1", ccMainAppInterface::ERR_CONSOLE_MESSAGE);
		return;
	}
	ccHObject* entity = selectedEntities[0];
	if (entity->isA(CC_TYPES::POINT_CLOUD))
	{
		cloud = static_cast<ccPointCloud*>(entity);
	}
	else {
		m_app->dispToConsole("[ElevationPointsPlugin] not a CC_TYPES::POINT_CLOUD", ccMainAppInterface::ERR_CONSOLE_MESSAGE);
		return;
	}

	// 2.遍历点云坐标

	QElapsedTimer eTimer;                       // 统计时间
	eTimer.start();
	//CCVector3 bbmin, bbmax;
	//cloud->getBoundingBox(bbmin, bbmax);        // 获取边界框
	//float th = (bbmin[2] + bbmax[2]) / 2;       // 设定阈值

	float precision = 0.01;         // 设置点云坐标的精度

	bool isOK;
	float Hz = QInputDialog::getDouble(nullptr, "参数设置", "高程阈值：", 154.39, -2147483647, 2147483647, 2, &isOK);
	if (!isOK)
	{
		ccLog::Print("已取消提取过程！");;
		return;
	}

	CCCoreLib::ReferenceCloud sampledIndex(cloud);
	for (size_t i = 0; i < cloud->size(); ++i)
	{
		if ((cloud->getPoint(i)->z >= Hz - precision) && (cloud->getPoint(i)->z <= Hz + precision))
		{
			sampledIndex.addPointIndex(i);      // 取大于阈值的坐标
		}
	}
	ccLog::Print("[ElevationPointsPlugin] Timing: %3.3f s.", eTimer.elapsed() / 1000.0);

	// 3.根据索引提取生成新的点云
	int warnings = 0;
	ccPointCloud* newPointCloud = cloud->partialClone(&sampledIndex, &warnings);
	if (warnings)
	{
		ccLog::Warning("[ElevationPointsPlugin] Not enough memory: colors, normals or scalar fields may be missing!");
	}
	if (newPointCloud)
	{
		newPointCloud->setName(cloud->getName() + QString(".ElevationPointsPlugin"));
		newPointCloud->setGlobalShift(cloud->getGlobalShift());
		newPointCloud->setGlobalScale(cloud->getGlobalScale());
		newPointCloud->setDisplay(cloud->getDisplay());
		newPointCloud->setColor(ccColor::Rgba(255, rand() % 255, 0, 200));
		newPointCloud->showColors(true);
		newPointCloud->setPointSize(3);
		newPointCloud->prepareDisplayForRefresh();
		if (cloud->getParent())
			cloud->getParent()->addChild(newPointCloud);
		cloud->setEnabled(false);
		m_app->addToDB(newPointCloud);
		newPointCloud->prepareDisplayForRefresh();
	}

	// 4.结尾 Hello world!

	// Put your code here
	// --> you may want to start by asking for parameters (with a custom dialog, etc.)

	// This is how you can output messages
	// Display a standard message in the console
	m_app->dispToConsole("[ElevationPointsPlugin] Hello world!", ccMainAppInterface::STD_CONSOLE_MESSAGE);

	// Display a warning message in the console
	m_app->dispToConsole("[ElevationPointsPlugin] Warning: example plugin shouldn't be used as is", ccMainAppInterface::WRN_CONSOLE_MESSAGE);

	// Display an error message in the console AND pop-up an error box
	//m_app->dispToConsole( "Example plugin shouldn't be used - it doesn't do anything!", ccMainAppInterface::ERR_CONSOLE_MESSAGE );
	m_app->dispToConsole("指定高程的点获取完毕！！！", ccMainAppInterface::STD_CONSOLE_MESSAGE);
	/*** HERE ENDS THE ACTION ***/
}

