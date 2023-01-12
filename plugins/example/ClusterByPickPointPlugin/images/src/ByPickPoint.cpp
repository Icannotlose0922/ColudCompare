#include "ccClusterByPickPoint.h"

//Local
#include "ccCommon.h"
#include "ccGLWindow.h"
#include "ccGuiParameters.h"

//qCC_db
#include <ccLog.h>
#include <ccPointCloud.h>
#include <ccGenericMesh.h>
#include <ccHObjectCaster.h>

//CCLib
#include <ScalarField.h>

//PCl
#include<pcl\point_types.h>
#include<pcl\kdtree\kdtree_flann.h>

//Qt
#include <QInputDialog>

//System
#include <assert.h>
#include <vector>
#include <queue>

ccClusterByPickPoin::ccClusterByPickPoin(ccPickingHub* pickingHub, QWidget* parent)
	: ccPointPickingGenericInterface(pickingHub, parent)
	, Ui::ClusterByPickPointDlg()
	, m_pickingMode(POINT_INFO)
	, selectPoint(nullptr)		//避免selectPoint称为野指针
{
	setupUi(this);

	//selectPoint = new ccPointCloud(QStringLiteral("选中点"));
	connect(closeButton, &QToolButton::clicked, this, &ccClusterByPickPoin::onClose);
	connect(pointPropertiesButton, &QToolButton::clicked, this, &ccClusterByPickPoin::activatePointPropertiesDisplay);
	connect(pointPointDistanceButton, &QToolButton::clicked, this, &ccClusterByPickPoin::activateDistanceDisplay);
	//connect(saveLabelButton, &QToolButton::clicked, this, &ccClusterByPickPoin::exportData);
	connect(razButton, &QToolButton::clicked, this, &ccClusterByPickPoin::initializeState);
}

ccClusterByPickPoin::~ccClusterByPickPoin()
{
	//if (selectPoint)		//释放资源，但如果已经将资源全部转交给主窗口了，那么这里将不需要进行重复释放资源
	//	delete selectPoint;
	//selectPoint = nullptr;
}

bool ccClusterByPickPoin::start()
{
	activatePointPropertiesDisplay();

	return ccPointPickingGenericInterface::start();
}

void ccClusterByPickPoin::stop(bool state)
{
	if (m_associatedWin)
		m_associatedWin->setInteractionMode(ccGLWindow::MODE_TRANSFORM_CAMERA);

	ccPointPickingGenericInterface::stop(state);
}

bool ccClusterByPickPoin::linkWith(ccGLWindow * win)
{
	ccGLWindow* oldWin = m_associatedWin;

	if (!ccPointPickingGenericInterface::linkWith(win))
	{
		return false;
	}

	if (oldWin)
	{
		oldWin->setInteractionMode(ccGLWindow::MODE_TRANSFORM_CAMERA);
		oldWin->disconnect(this);
	}

	return true;
}

void ccClusterByPickPoin::onClose()
{
	if (m_associatedWin && selectPoint)			//删除标签
	{
		m_associatedWin->removeFromOwnDB(selectPoint);
		m_associatedWin->redraw();
	}
	stop(false);
}

void ccClusterByPickPoin::activatePointPropertiesDisplay()
{
	if (m_associatedWin)
		m_associatedWin->setInteractionMode(ccGLWindow::MODE_TRANSFORM_CAMERA);

	m_pickingMode = POINT_INFO;
	pointPropertiesButton->setDown(true);
	pointPointDistanceButton->setDown(false);
	pointsAngleButton->setDown(false);
	rectZoneToolButton->setDown(false);
}

void ccClusterByPickPoin::activateDistanceDisplay()
{
	m_pickingMode = POINT_POINT_DISTANCE;
	pointPropertiesButton->setDown(false);
	pointPointDistanceButton->setDown(true);
	pointsAngleButton->setDown(false);
	rectZoneToolButton->setDown(false);

	if (m_associatedWin)
	{
		m_associatedWin->setInteractionMode(ccGLWindow::MODE_TRANSFORM_CAMERA);	//设置主窗口的交互模式，相机变换
		m_associatedWin->redraw(false);		//禁止重新绘制主窗口
	}
}

void ccClusterByPickPoin::initializeState()
{
	if (m_associatedWin)
		m_associatedWin->redraw(false);		//非空，则先禁止重绘
}

//void ccClusterByPickPoin::exportData()
//{
//
//}

void ccClusterByPickPoin::processPickedPoint(const PickedItem & picked)
{
	assert(picked.entity);
	assert(m_associatedWin);
	//assert(selectPoint);

	if (m_associatedWin && selectPoint)			//删除之前的标签
	{
		m_associatedWin->removeFromOwnDB(selectPoint);
		m_associatedWin->redraw();
		//m_associatedWin->setInteractionMode(ccGLWindow::TRANSFORM_CAMERA());
		//m_associatedWin->disconnect(this);		//断开关联
	}
	selectPoint = nullptr;

	ccPointCloud* pc = dynamic_cast<ccPointCloud*>(picked.entity);
	ccLog::Print(QStringLiteral("实体点数量：%1").arg(pc->size()));
	const CCVector3 *point = pc->getPoint(picked.itemIndex);
	//ccLog::Print(QStringLiteral("选中坐标：%f %f %f").toStdString().data(), point->x, point->y, point->z);
	ccLog::Print(QStringLiteral("选中坐标：%1 %2 %3")
		.arg(point->x)
		.arg(point->y)
		.arg(point->z));
	//qDebug() << QStringLiteral("选中坐标：") << point->x << " " << point->y << " " << point->z;

	if(!selectPoint)
		selectPoint = new ccPointCloud(QStringLiteral("选中点")+
			QString("(%1 %2 %3)").arg(point->x).arg(point->y).arg(point->z));
	
	selectPoint->clear();

	//根据选中的点进行聚类（点选聚类）
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
	cloud->points.resize(pc->size());
	for (size_t i = 0; i < pc->size(); i++)
	{
		const CCVector3* tmpPoint = pc->getPoint(i);
		cloud->points[i].x = tmpPoint->x;
		cloud->points[i].y = tmpPoint->y;
		cloud->points[i].z = tmpPoint->z;
	}

	pcl::KdTreeFLANN<pcl::PointXYZ> kdtree;		//构建一个kdtree便于后续查询
	kdtree.setInputCloud(cloud);
	std::queue<unsigned int> seed;
	seed.push(picked.itemIndex);
	float Neighbourhood = 0.2f;
	std::vector<int> neighbourPoints;
	std::vector<float> neighbourDistance;		//pcl中的默认距离是float
	std::vector<int> flag;
	flag.resize(pc->size());
	while (!seed.empty()) {
		unsigned int index = seed.front();		//获取堆最前面的点
		seed.pop();		//弹出最前面的那个点
		if (flag[index]) continue;
		kdtree.radiusSearch(cloud->points[index], Neighbourhood, neighbourPoints, neighbourDistance);
		selectPoint->addPoint(CCVector3(cloud->points[index].x, 
			cloud->points[index].y, 
			cloud->points[index].z));		//添加点
		for (int i = 1; i < neighbourPoints.size(); i++)
			seed.push(neighbourPoints[i]);

		flag[index] = 1;
	}

	selectPoint->setColor(ccColor::Rgba(255, 0, 0, 200));
	selectPoint->setPointSize(10);
	selectPoint->showColors(true);

	m_clusterClouds.push_back(selectPoint);

	if (m_associatedWin)
	{
		m_associatedWin->addToOwnDB(selectPoint);		//将标记添加到窗口中
		m_associatedWin->redraw();
	}
}

bool ccClusterByPickPoin::addEntity(ccHObject * entity)
{
	bool result = false;
	if (entity->isKindOf(CC_TYPES::POINT_CLOUD))		//判断是否为点云类型
	{
		ccGenericPointCloud* cloud = ccHObjectCaster::ToGenericPointCloud(entity);
		cloud->resetVisibilityArray();
		m_entities.insert(cloud);

		//递归的方式添加下一级点云实体
		for (unsigned i = 0; i < entity->getChildrenNumber(); ++i)
			result |= addEntity(entity->getChild(i));
	}
	else if (entity->isA(CC_TYPES::HIERARCHY_OBJECT))
	{
		//添加点云实体的子集
		for (unsigned i = 0; i<entity->getChildrenNumber(); ++i)
			result |= addEntity(entity->getChild(i));
	}

	return result;
}

std::vector<ccPointCloud*> ccClusterByPickPoin::getClusterClouds()
{
	return m_clusterClouds;
}

void ccClusterByPickPoin::clearCluster()
{
	m_clusterClouds.clear();
}





