#ifndef EXAMPLE_GRIDMIN_HEADER
#define EXAMPLE_GRIDMIN_HEADER


#include "ccStdPluginInterface.h"


class GridMinimumPlugin : public QObject, public ccStdPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(ccStdPluginInterface)
	
	// info.json 文件向加载系统提供关于插件的信息，显示在插件对话框中
	Q_PLUGIN_METADATA(IID "cccorp.cloudcompare.plugin.Example" FILE "info.json")
	
public:
	explicit GridMinimumPlugin( QObject *parent = nullptr );
	~GridMinimumPlugin() override = default;
	
	// 继承自ccStdPluginInterface
	void onNewSelection( const ccHObject::Container &selectedEntities ) override;
	QList<QAction *> getActions() override;
	
private:
	/*** 在这里添加自己的自定义操作 ***/
	void doAction();
	
	//! Default action
	/** 在这里边可以添加很多个功能，只要确保每个操作功能对应专用的图标、工具栏和
	* 插件中的一个条目即可。
	**/
	QAction* m_action;
};

#endif

