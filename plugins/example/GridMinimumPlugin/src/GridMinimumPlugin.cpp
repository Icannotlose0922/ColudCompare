// 系统头文件
#include <iostream>
// Qt头文件
#include <QtGui>
#include <QInputDialog>
// CCCoreLib头文件
#include "GridMinimumPlugin.h"
#include "ccProgressDialog.h"
#include "ccPointCloud.h"
// PCL头文件
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/filters/grid_minimum.h>// 最低点滤波


//默认构造函数:
// -将Qt资源路径传递给info. json文件(from <yourPluginName>. qrc file)
// -构造函数应该主要用于初始化操作和其他成员
GridMinimumPlugin::GridMinimumPlugin(QObject* parent)
	: QObject(parent)
	, ccStdPluginInterface(":/CC/plugin/GridMinimumPlugin/info.json")
	, m_action(nullptr)
{
}

//这个方法应该启用或禁用你的插件动作
//取决于当前选择的实体('selectedEntities')。
void GridMinimumPlugin::onNewSelection(const ccHObject::Container& selectedEntities)
{
	if (m_action == nullptr)
	{
		return;
	}
	// 选择一个点云实体
	m_action->setEnabled(!selectedEntities.empty());
}

//这个方法返回你的插件可以执行的所有“动作”。
// getActions()只会在插件加载时被调用一次。
QList<QAction*> GridMinimumPlugin::getActions()
{
	//默认动作(如果它还没有被创建，则此时执行)
	if (!m_action)
	{
		// 这里添加插件名称、描述和图标
		m_action = new QAction(getName(), this);
		m_action->setToolTip(getDescription());
		m_action->setIcon(getIcon());

		// 连接信号
		connect(m_action, &QAction::triggered, this, &GridMinimumPlugin::doAction);
	}

	return { m_action };
}

// 添加自己代码要实现的功能
void GridMinimumPlugin::doAction()
{
	if (m_app == nullptr)
	{
		// m_app在加载插件时已经被CC初始化
		Q_ASSERT(false);

		return;
	}

	/*** 核心算法功能 ***/
	// 检查是否只选择了一个点云实体
	if (!m_app->haveOneSelection())
	{
		m_app->dispToConsole("Select only one cloud!", ccMainAppInterface::ERR_CONSOLE_MESSAGE);
		return;
	}

	// ---------------------------从前端显示界面获取点云实体对象-------------------------------
	const ccHObject::Container& selectedEntities = m_app->getSelectedEntities();
	ccHObject* ent = selectedEntities[0];
	assert(ent);// 检测是否为空
	if (!ent || !ent->isA(CC_TYPES::POINT_CLOUD))
	{
		m_app->dispToConsole("Select a real point cloud!", ccMainAppInterface::ERR_CONSOLE_MESSAGE);
		return;
	}
	// ------------------------------CCcoreLib支持的数据格式----------------------------------
	ccPointCloud* ccCloud = ccHObjectCaster::ToPointCloud(ent);
	// --------------------------------转换成PCL支持的数据------------------------------------
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
	cloud->resize(ccCloud->size());
	for (size_t i = 0; i < ccCloud->size(); ++i)
	{
		CCVector3 point = *ccCloud->getPoint(i);
		(*cloud)[i] = pcl::PointXYZ(point.x, point.y, point.z);
	}
	ccLog::Print("PointCloud has: %d data points.", cloud->points.size());

	// -----------------------------对话框--------------------------------------
	bool isOK;
	QString resolutionThreshold = QInputDialog::getText(nullptr, QStringLiteral("参数"),
		QStringLiteral("格网尺寸："),
		QLineEdit::Normal,
		"",
		&isOK);
	double resolution = resolutionThreshold.trimmed().toDouble();

	if (!isOK)
	{
		ccLog::Print(QStringLiteral("已取消提取过程！"));;
		return;
	}
	// 进度条
	ccProgressDialog pDlg(true, 0);
	CCLib::GenericProgressCallback* pCallBack = &pDlg;
	if (pCallBack)
	{
		if (pCallBack->textCanBeEdited())
		{
			pCallBack->setMethodTitle("Compute");
			pCallBack->setInfo(qPrintable(QString("Wait......")));
		}
		pCallBack->update(0);
		pCallBack->start();
	}
	// --------------------------格网最低点滤波----------------------------------
	pcl::PointCloud<pcl::PointXYZ>::Ptr filtered(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::GridMinimum<pcl::PointXYZ> gm(resolution);
	gm.setInputCloud(cloud);
	gm.filter(*filtered);
	// -------------------------PCL->CloudCompare---------------------------------
	ccPointCloud* newPointCloud = new ccPointCloud(QString("GridMinumin"));
	for (int i = 0; i < filtered->size(); ++i)
	{
		double x = filtered->points[i].x;
		double y = filtered->points[i].y;
		double z = filtered->points[i].z;
		newPointCloud->addPoint(CCVector3(x, y, z));
	}
	newPointCloud->setColor(ccColor::Rgba(255, rand() % 255, 0, 200));
	newPointCloud->showColors(true);
	newPointCloud->setPointSize(3);
	if (ccCloud->getParent())
		ccCloud->getParent()->addChild(newPointCloud);
	ccCloud->setEnabled(false);
	m_app->addToDB(newPointCloud);
	newPointCloud->prepareDisplayForRefresh();

	pCallBack->update(100);
	if (pCallBack)
		pCallBack->stop();

	//刷新
	m_app->refreshAll();
	m_app->updateUI();

	// This is how you can output messages
	// 在控制台中显示一条标准消息
	m_app->dispToConsole("[GridMinimumPlugin] Hello world!", ccMainAppInterface::STD_CONSOLE_MESSAGE);

	// 在控制台中显示一条警告消息
	m_app->dispToConsole("[GridMinimumPlugin] Warning: example plugin shouldn't be used as is", ccMainAppInterface::WRN_CONSOLE_MESSAGE);

	// 在控制台中显示错误消息并弹出错误框
	m_app->dispToConsole("Example plugin shouldn't be used - it doesn't do anything!", ccMainAppInterface::ERR_CONSOLE_MESSAGE);

	/*** 算法结束 ***/
}

