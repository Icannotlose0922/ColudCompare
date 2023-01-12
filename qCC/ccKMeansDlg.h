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

	//��һ��������Ի������������
	void linkWithEntity(ccHObject* entity);

	//��ȡѡ�еĵ�
	std::vector<size_t> getSelectedPoints();

	//���֮ǰ��ѡ�ĵ�
	void clearPoints();

	//��ȡӦ��״̬
	bool getState();

protected slots:

	void applyAndExit();

	void cancelAndExit();

	//���б���ɾ����һ����
	void removeLastEntry();

	//����Ǵ�С�ı�ʱ���»��ƴ���
	void markerSizeChanged(int);
	//��ʼ���������ı�
	void startIndexChanged(int);
	//���µ��б�
	void updateList();

protected:

	//�ú������ȡ��귢�͹�������Ϣ
	void processPickedPoint(const PickedItem& picked) override;

	//��ȡ��ǰѡ�еĵ�
	unsigned getPickedPoints(std::vector<cc2DLabel*>& pickedPoints);

	ccHObject* m_associatedEntity;

	unsigned m_lastPreviousID;

	ccHObject* m_orderedLabelsContainer;
	//ɾ���ĵ�
	ccHObject::Container m_toBeDeleted;
	//��ӵĵ�
	ccHObject::Container m_toBeAdded;

	//ѡ�еĵ�
	std::vector<size_t> selectedPoints;

	//����һ����������ʾ״̬
	bool state;
};

#endif
