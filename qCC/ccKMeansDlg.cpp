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

#include "ccKMeansDlg.h"

//Qt
#include <QApplication>
#include <QClipboard>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QSettings>

//CCLib
#include <CCConst.h>

//qCC_db
#include <cc2DLabel.h>
#include <ccLog.h>
#include <ccPointCloud.h>
#include <ccPolyline.h>

//qCC_io
#include <AsciiFilter.h>

//local
#include "ccGLWindow.h"
#include "mainwindow.h"

#include "db_tree/ccDBRoot.h"

//system
#include <cassert>
#include "ccKMeansDlg.h"

//semi persistent settings
static int s_pickedPointsStartIndex = 0;
static bool s_showGlobalCoordsCheckBoxChecked = false;
static const char s_pickedPointContainerName[] = "Picked points list";
static const char s_defaultLabelBaseName[] = "Point #";

ccKMeansDlg::ccKMeansDlg(ccPickingHub* pickingHub, QWidget* parent)
    : ccPointPickingGenericInterface(pickingHub, parent)
    , Ui::KMeansDlg()
    , m_associatedEntity(nullptr)
    , m_lastPreviousID(0)
    , m_orderedLabelsContainer(nullptr)
	, state(true)
{
    setupUi(this);

	exportToolButton->setPopupMode(QToolButton::MenuButtonPopup);
	QMenu* menu = new QMenu(exportToolButton);
	QAction* exportASCII_xyz = menu->addAction("x,y,z");
	QAction* exportASCII_ixyz = menu->addAction("local index,x,y,z");
	QAction* exportASCII_gxyz = menu->addAction("global index,x,y,z");
	QAction* exportASCII_lxyz = menu->addAction("label name,x,y,z");
	QAction* exportToNewCloud = menu->addAction("new cloud");
	QAction* exportToNewPolyline = menu->addAction("new polyline");
	exportToolButton->setMenu(menu);

	tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

	startIndexSpinBox->setValue(s_pickedPointsStartIndex);
	showGlobalCoordsCheckBox->setChecked(s_showGlobalCoordsCheckBoxChecked);

	connect(cancelToolButton,		&QAbstractButton::clicked,	this,				&ccKMeansDlg::cancelAndExit);
	connect(revertToolButton,		&QAbstractButton::clicked,	this,				&ccKMeansDlg::removeLastEntry);
	connect(validToolButton,		&QAbstractButton::clicked,	this,				&ccKMeansDlg::applyAndExit);
	connect(exportToolButton,		&QAbstractButton::clicked,	exportToolButton,	&QToolButton::showMenu);
	
	connect(markerSizeSpinBox,	static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),	this,	&ccKMeansDlg::markerSizeChanged);
	connect(startIndexSpinBox,	static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),	this,	&ccKMeansDlg::startIndexChanged);
	
	connect(showGlobalCoordsCheckBox, &QAbstractButton::clicked, this, &ccKMeansDlg::updateList);

	updateList();
}

unsigned ccKMeansDlg::getPickedPoints(std::vector<cc2DLabel*>& pickedPoints)
{
	pickedPoints.clear();

	if (m_orderedLabelsContainer)		//˳��ı�ǩ�����Ƿ�Ϊ��
	{
		//��ȡ���б�ǩ
		ccHObject::Container labels;
		unsigned count = m_orderedLabelsContainer->filterChildren(labels, false, CC_TYPES::LABEL_2D);

		try
		{
			pickedPoints.reserve(count);
		}
		catch (const std::bad_alloc&)
		{
			ccLog::Error(QStringLiteral("�ڴ治�㣡"));
			return 0;
		}
		for (unsigned i = 0; i < count; ++i)
		{
			if (labels[i]->isA(CC_TYPES::LABEL_2D))
			{
				cc2DLabel* label = static_cast<cc2DLabel*>(labels[i]);
				if (label->isVisible() && label->size() == 1)
				{
					pickedPoints.push_back(label);
				}
			}
		}
	}

	return static_cast<unsigned>(pickedPoints.size());
}

void ccKMeansDlg::linkWithEntity(ccHObject* entity)
{
	m_associatedEntity = entity;
	m_lastPreviousID = 0;

	if (m_associatedEntity)
	{
		//find default container
		m_orderedLabelsContainer = nullptr;
		ccHObject::Container groups;
		m_associatedEntity->filterChildren(groups, true, CC_TYPES::HIERARCHY_OBJECT);

		for (ccHObject::Container::const_iterator it = groups.begin(); it != groups.end(); ++it)
		{
			if ((*it)->getName() == s_pickedPointContainerName)
			{
				m_orderedLabelsContainer = *it;
				break;
			}
		}

		std::vector<cc2DLabel*> previousPickedPoints;
		unsigned count = getPickedPoints(previousPickedPoints);
		//find highest unique ID among the VISIBLE labels
		for (unsigned i = 0; i < count; ++i)
		{
			m_lastPreviousID = std::max(m_lastPreviousID, previousPickedPoints[i]->getUniqueID());
		}
	}

	ccShiftedObject* shifted = ccHObjectCaster::ToShifted(entity);
	showGlobalCoordsCheckBox->setEnabled(shifted ? shifted->isShifted() : false);
	updateList();
}

void ccKMeansDlg::cancelAndExit()
{
	ccDBRoot* dbRoot = MainWindow::TheInstance()->db();
	if (!dbRoot)
	{
		assert(false);
		return;
	}

	if (m_orderedLabelsContainer)
	{
		//����֮ǰ��״̬
		if (!m_toBeAdded.empty())
		{
			dbRoot->removeElements(m_toBeAdded);
		}

		for (auto & object : m_toBeDeleted)
		{
			object->prepareDisplayForRefresh();
			object->setEnabled(true);
		}

		if (m_orderedLabelsContainer->getChildrenNumber() == 0)
		{
			dbRoot->removeElement(m_orderedLabelsContainer);
			//m_associatedEntity->removeChild(m_orderedLabelsContainer);
			m_orderedLabelsContainer = nullptr;
		}
	}

	m_toBeDeleted.resize(0);
	m_toBeAdded.resize(0);
	m_associatedEntity = nullptr;
	m_orderedLabelsContainer = nullptr;

	MainWindow::RefreshAllGLWindow();

	updateList();
	
	//selectedPoints.clear();		//���֮ǰ��ѡ�ĵ�
	state = false;

	stop(false);
}

void ccKMeansDlg::applyAndExit()
{
	if (m_associatedEntity && !m_toBeDeleted.empty())
	{
		//apply modifications
		MainWindow::TheInstance()->db()->removeElements(m_toBeDeleted); //no need to redraw as they should already be invisible
		m_associatedEntity = nullptr;
	}

	m_toBeDeleted.resize(0);
	m_toBeAdded.resize(0);
	m_orderedLabelsContainer = nullptr;

	updateList();

	state = true;

	stop(true);
}

void ccKMeansDlg::removeLastEntry()
{
	if (!m_associatedEntity)
		return;

	//get all labels
	std::vector<cc2DLabel*> labels;
	unsigned count = getPickedPoints(labels);
	if (count == 0)
		return;

	ccHObject* lastVisibleLabel = labels.back();
	if (lastVisibleLabel->getUniqueID() <= m_lastPreviousID)
	{
		//old label: hide it and add it to the 'to be deleted' list (will be restored if process is cancelled)
		lastVisibleLabel->setEnabled(false);
		m_toBeDeleted.push_back(lastVisibleLabel);
	}
	else
	{
		if (!m_toBeAdded.empty())
		{
			assert(m_toBeAdded.back() == lastVisibleLabel);
			m_toBeAdded.pop_back();
		}

		if (m_orderedLabelsContainer)
		{
			if (lastVisibleLabel->getParent())
			{
				lastVisibleLabel->getParent()->removeDependencyWith(lastVisibleLabel);
				lastVisibleLabel->removeDependencyWith(lastVisibleLabel->getParent());
			}
			//m_orderedLabelsContainer->removeChild(lastVisibleLabel);
			MainWindow::TheInstance()->db()->removeElement(lastVisibleLabel);
		}
		else
			m_associatedEntity->detachChild(lastVisibleLabel);
	}

	updateList();

	if (m_associatedWin)
		m_associatedWin->redraw();
}

void ccKMeansDlg::startIndexChanged(int value)
{
	if (value != s_pickedPointsStartIndex)
	{
		s_pickedPointsStartIndex = value;

		updateList();
		if (m_associatedWin)
		{
			m_associatedWin->redraw();
		}
	}
}

void ccKMeansDlg::markerSizeChanged(int size)
{
	if (size < 1 || !m_associatedWin)
		return;

	//display parameters
	ccGui::ParamStruct guiParams = m_associatedWin->getDisplayParameters();

	if (guiParams.labelMarkerSize != static_cast<unsigned>(size))
	{
		guiParams.labelMarkerSize = static_cast<unsigned>(size);
		m_associatedWin->setDisplayParameters(guiParams,m_associatedWin->hasOverriddenDisplayParameters());
																		
		m_associatedWin->redraw();
	}
}

void ccKMeansDlg::updateList()
{
	//��ȡ���еı�ǩ
	std::vector<cc2DLabel*> labels;
	const int count = static_cast<int>( getPickedPoints(labels) );

	const int oldRowCount = tableWidget->rowCount();

	revertToolButton->setEnabled(count);
	validToolButton->setEnabled(count);
	exportToolButton->setEnabled(count);
	countLineEdit->setText(QString::number(count));
	tableWidget->setRowCount(count);		//���öԻ��������

	if ( count == 0 )
	{
		return;
	}

	//������µ����꣬���һ��
	if ( (count - oldRowCount) > 0 )
	{
		for ( int i = oldRowCount; i < count; ++i )
		{
			tableWidget->setVerticalHeaderItem( i, new QTableWidgetItem );		//���һ��ͷѡ��

			for ( int j = 0; j < 4; ++j )
			{
				tableWidget->setItem( i, j, new QTableWidgetItem );		//����ĸ�����
			}
		}
	}

	//��ʼ����
	const int startIndex = startIndexSpinBox->value();
	const int precision = m_associatedWin ? static_cast<int>(m_associatedWin->getDisplayParameters().displayedNumPrecision) : 6;

	const bool showAbsolute = showGlobalCoordsCheckBox->isEnabled() && showGlobalCoordsCheckBox->isChecked();

	for ( int i = 0; i < count; ++i )
	{
		cc2DLabel* label = labels[static_cast<unsigned int>( i )];

		const cc2DLabel::PickedPoint& PP = label->getPickedPoint(0);
		CCVector3 P = PP.getPointPosition();
		CCVector3d Pd = (showAbsolute ? PP.cloudOrVertices()->toGlobal3d(P) : CCVector3d::fromArray(P.u));

		//���õ��б����������0��ʼ
		tableWidget->verticalHeaderItem( i )->setText( QStringLiteral( "%1" ).arg( i + startIndex ) );

		//update name as well
		if (	label->getUniqueID() > m_lastPreviousID
		    ||	label->getName().startsWith(s_defaultLabelBaseName) ) //DGM: we don't change the name of old labels that have a non-default name
		{
			label->setName(s_defaultLabelBaseName + QString::number(i+startIndex));
		}

		//point absolute index (in cloud)
		tableWidget->item( i, 0 )->setText( QStringLiteral( "%1" ).arg( PP.index ) );

		for ( int j = 0; j < 3; ++j )
		{
			tableWidget->item( i, j + 1 )->setText( QStringLiteral( "%1" ).arg( Pd.u[j], 0, 'f', precision ) );
		}
	}

	tableWidget->scrollToBottom();
}

void ccKMeansDlg::processPickedPoint(const PickedItem& picked)
{
	if (!picked.entity || picked.entity != m_associatedEntity || !MainWindow::TheInstance())
		return;

	cc2DLabel* newLabel = new cc2DLabel();
	if (picked.entity->isKindOf(CC_TYPES::POINT_CLOUD))
	{
		newLabel->addPickedPoint(static_cast<ccGenericPointCloud*>(picked.entity), picked.itemIndex, picked.entityCenter);
	}
	else
	{
		delete newLabel;
		assert(false);
		return;
	}
	newLabel->setVisible(true);			//���ñ�ǩ�ɼ�
	newLabel->setDisplayedIn2D(false);
	newLabel->displayPointLegend(true);
	newLabel->setCollapsed(true);
	ccGenericGLDisplay* display = m_associatedEntity->getDisplay();
	if (display)
	{
		newLabel->setDisplay(display);
		QSize size = display->getScreenSize();
		newLabel->setPosition(	static_cast<float>(picked.clickPoint.x() + 20) / size.width(),
		                        static_cast<float>(picked.clickPoint.y() + 20) / size.height() );
	}

	//add default container if necessary
	if (!m_orderedLabelsContainer)
	{
		m_orderedLabelsContainer = new ccHObject(s_pickedPointContainerName);
		m_associatedEntity->addChild(m_orderedLabelsContainer);
		m_orderedLabelsContainer->setDisplay(display);
		MainWindow::TheInstance()->addToDB(m_orderedLabelsContainer, false, true, false, false);
	}
	assert(m_orderedLabelsContainer);
	m_orderedLabelsContainer->addChild(newLabel);
	MainWindow::TheInstance()->addToDB(newLabel, false, true, false, false);
	m_toBeAdded.push_back(newLabel);

	//����һЩ�µĵ����굽���а�
	QClipboard* clipboard = QApplication::clipboard();
	if (clipboard)
	{
		const cc2DLabel::PickedPoint& PP = newLabel->getPickedPoint(0);
		CCVector3 P = PP.getPointPosition();
		int precision = m_associatedWin ? m_associatedWin->getDisplayParameters().displayedNumPrecision : 6;
		int indexInList = startIndexSpinBox->value() + static_cast<int>(m_orderedLabelsContainer->getChildrenNumber()) - 1;
		clipboard->setText(QString("CC_POINT_#%0(%1;%2;%3)").arg(indexInList).arg(P.x, 0, 'f', precision).arg(P.y, 0, 'f', precision).arg(P.z, 0, 'f', precision));
		selectedPoints.push_back(PP.index);
	}

	updateList();

	if (m_associatedWin)
		m_associatedWin->redraw();
}

std::vector<size_t> ccKMeansDlg::getSelectedPoints()
{
	//��ȡ���е�
	return selectedPoints;
}

void ccKMeansDlg::clearPoints()
{
	selectedPoints.clear();
}

bool ccKMeansDlg::getState()
{
	return state;
}
