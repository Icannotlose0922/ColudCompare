//##########################################################################
//#                                                                        #
//#                      CLOUDCOMPARE PLUGIN: qSRA                         #
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
//#                           COPYRIGHT: EDF                               #
//#                                                                        #
//##########################################################################

#ifndef QSRA_DISTANCE_MAP_GENERATION_DIALOG
#define QSRA_DISTANCE_MAP_GENERATION_DIALOG

//Local
#include "distanceMapGenerationTool.h"

//qCC_db
#include <ccBBox.h>

//Qt
#include <QSharedPointer>
#include <QColor>

//CCCoreLib
#include <CCTypes.h>

//GUIs generated by Qt Designer
#include <ui_distanceMapGenerationDlg.h>

class ccMainAppInterface;
class ccPointCloud;
class ccPolyline;
class ccScalarField;
class ccGLWindow;
class ccColorScaleSelector;
class ccSymbolCloud;
class ccMapWindow;

//! Dialog for generating a distance map (surface of revolution)
class DistanceMapGenerationDlg : public QDialog, public Ui::DistanceMapGenerationDlg
{
	Q_OBJECT

public:

	//! Default constructor
	DistanceMapGenerationDlg(ccPointCloud* cloud, ccScalarField* sf, ccPolyline* polyline, ccMainAppInterface* app = nullptr);

	//! Default destructor
	virtual ~DistanceMapGenerationDlg() = default;

	//! Angular units
	enum ANGULAR_UNIT { ANG_DEG, ANG_RAD, ANG_GRAD };

	//! Returns selected angular unit
	ANGULAR_UNIT getAngularUnit() const;
	//! Returns selected angular unit as a string
	QString getAngularUnitString() const;
	//! Returns selected angular unit as a condensed string
	QString getCondensedAngularUnitString() const;

	//! Returns height unit
	QString getHeightUnitString() const;

	//! Returns the grid filling strategy
	DistanceMapGenerationTool::FillStrategyType getFillingStrategy() const;

	//! Returns the empty cells filling option
	DistanceMapGenerationTool::EmptyCellFillOption getEmptyCellFillingOption() const;

	//! Projection mode
	enum ProjectionMode {	PROJ_CYLINDRICAL,
							PROJ_CONICAL,
	};

	//! Returns current projection mode
	ProjectionMode getProjectionMode() const;

	//! Returns the current 'X' parameters
	/** Always correspond to angular values
	**/
	void getGridXValues(double& minX, double& maxX, double& step, ANGULAR_UNIT unit = ANG_RAD) const;

	//! Returns the current 'Y' parameters
	/** Depends on the current projection mode (i.e. height or latitude).
		The 'unit' parameter is only required for Conical mode.
	**/
	void getGridYValues(double& minY, double& maxY, double& step, ANGULAR_UNIT unit = ANG_RAD) const;

	//! Returns scale 'Y' step (current unit)
	/** The 'unit' parameter is only required for Conical mode.
	**/
	double getScaleYStep(ANGULAR_UNIT unit = ANG_RAD) const;

protected:

	void projectionModeChanged(int);
	void angularUnitChanged(int);
	void updateGridSteps();
	void updateProfileRevolDim(int);
	void updateProfileOrigin();
	void colorScaleChanged(int);
	void spawnColorScaleEditor();
	void updateHeightUnits();
	void exportMapAsCloud();
	void exportMapAsMesh();
	void exportMapAsGrid();
	void exportMapAsImage();
	void exportProfilesAsDXF();
	void loadOverlaySymbols();
	void clearOverlaySymbols();
	void overlaySymbolsSizeChanged(int);
	void overlaySymbolsColorChanged();
	void overlayGridColorChanged();
	void labelFontSizeChanged(int);
	void labelPrecisionChanged(int);
	void baseRadiusChanged(double);
	void colorRampStepsChanged(int);
	void toggleOverlayGrid(bool);
	void changeGridColor();
	void changeSymbolColor();
	void toggleColorScaleDisplay(bool);
	void updateVolumes();

	//! Updates overlat grid (if any)
	void updateOverlayGrid();

	//! Updates map (and display)
	void update();

	void closeEvent(QCloseEvent* e) override;

protected:

	//! Updates internal map
	QSharedPointer<DistanceMapGenerationTool::Map> updateMap();

	//! Updates internal map texture (if any)
	void updateMapTexture();
	
	//! Updates 2D view zoom
	void updateZoom(ccBBox& box);

	//! Clears the 3D view
	void clearView();

	//! Returns currently applicable base radius
	/** Warning: return always 1 for concial projection mode!
	**/
	double getBaseRadius() const;

	//! Returns the angle in a particular spinbox in the specified units
	double getSpinboxAngularValue(QDoubleSpinBox* spinBox, DistanceMapGenerationDlg::ANGULAR_UNIT unit) const;

	//! Loads parameters from persistent settings
	void initFromPersistentSettings();

	//! Saves parameters to persistent settings
	void saveToPersistentSettings();

	//! Updates the min and max limits (fields)
	void updateMinAndMaxLimits();

	//! Application interface
	ccMainAppInterface* m_app;

	//! Associated cloud
	ccPointCloud* m_cloud;

	//! Associated profile
	ccPolyline* m_profile;

	//! Associated scalar field
	ccScalarField* m_sf;

	//! Internal map structure
	QSharedPointer<DistanceMapGenerationTool::Map> m_map;

	//! Current angular units
	ANGULAR_UNIT m_angularUnits;

	//! 2D display
	ccMapWindow* m_window;

	//! Color scale selector
	ccColorScaleSelector* m_colorScaleSelector;

	//! X labels
	ccSymbolCloud* m_xLabels;
	//! Y labels
	ccSymbolCloud* m_yLabels;

	//! Grid color
	QColor m_gridColor;

	//! Symbols color
	QColor m_symbolColor;
};

#endif //QSRA_DISTANCE_MAP_GENERATION_DIALOG
