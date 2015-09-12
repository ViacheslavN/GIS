#pragma once

#define ARCGIS_VERSION 1000

#pragma warning(push)
#pragma warning(disable : 4049)
#pragma warning(disable : 4192)
#pragma warning(disable : 4278)
#pragma warning(disable : 4336)

#import "esriSystem.olb" named_guids no_function_mapping
namespace esriSystem
{
	using ::IStream;
	using ::IStreamPtr;
	using ::IPersistStream;
	using ::IErrorInfo;
	using ::IErrorInfoPtr;

#if ARCGIS_VERSION >= 1000
	using ::tagRECT;
#endif
}


#import "esriSystemUI.olb" named_guids no_function_mapping rename("ICommand", "IESRICommand")
#import "esriGeometry.olb" named_guids no_function_mapping

#import "esriDisplay.olb" named_guids no_function_mapping

namespace esriDisplay
{
	using ::tagRECT;
}

#import "esriGeoDatabase.olb" named_guids no_function_mapping rename("IRow", "IESRIRow")
#import "esriDataSourcesFile.olb" named_guids no_function_mapping

#import "esriServer.olb" named_guids no_function_mapping

#if ARCGIS_VERSION >= 1000
namespace esriDataSourcesRaster
{
	using esriServer::IServerContext;
	using esriServer::IServerContextPtr;
}
#import "esriDataSourcesRaster.olb" named_guids no_function_mapping rename("IRow", "IESRIRow")
#else
#import "esriDataSourcesRaster.olb" named_guids no_function_mapping
#endif
#import "esriDataSourcesGDB.olb" named_guids no_function_mapping

namespace esriGISClient
{
	using esriServer::IServerObjectManager;
	using esriServer::IServerObjectManagerPtr;
	using esriServer::IServerObjectAdmin;
	using esriServer::IServerObjectAdminPtr;
	using esriServer::IServerObjectConfiguration;
	using esriServer::IServerObjectConfigurationPtr;
}
#import "esriGISClient.olb" named_guids no_function_mapping

#import "esriOutput.olb" named_guids no_function_mapping

#if ARCGIS_VERSION > 910
#import "esriGeoDatabaseExtensions.olb" named_guids no_function_mapping
#endif

#if ARCGIS_VERSION >= 910
#import "esriGeoDatabaseDistributed.olb" named_guids no_function_mapping
#endif

namespace esriCarto
{
	using esriDataSourcesFile::ICadDrawingDatasetPtr;
	using esriDataSourcesRaster::IUniqueValuesPtr;
	using esriSystemUI::IOperation;
	using esriGISClient::IIMSServiceDescriptionPtr;
	using esriOutput::IPrinterPtr;

#if ARCGIS_VERSION > 910
	using esriGeoDatabaseExtensions::ITerrain;
	using esriGeoDatabaseExtensions::ITerrainPtr;
	using esriGeoDatabaseExtensions::ICadastralFabric;
	using esriGeoDatabaseExtensions::ICadastralFabricPtr;
	using ::UINT_PTR;
#endif

#if ARCGIS_VERSION >= 1000
	using esriGeoDatabaseDistributed::IGDSData;
	using esriGeoDatabaseDistributed::IGDSDataPtr;
#endif
}

#if ARCGIS_VERSION <= 910
#import "esriCarto.olb" named_guids no_function_mapping
#else
#import "esriCarto.olb" named_guids no_function_mapping exclude("UINT_PTR") rename("ITableDefinition", "IESRITableDefinition") rename("IRow", "IESRIRow")
#endif

#if ARCGIS_VERSION > 910
#import "esriGeoDatabaseDistributed.olb" named_guids no_function_mapping
#endif
#if ARCGIS_VERSION >= 1000
#import "esriGeoAnalyst.olb" named_guids no_function_mapping
#endif

#if ARCGIS_VERSION > 910
namespace esriGeoprocessing
{
	using esriGeoDatabaseDistributed::IGDSData;
	using esriGeoDatabaseDistributed::IGDSDataPtr;
}
#endif
#import "esriGeoProcessing.olb" named_guids no_function_mapping

#if ARCGIS_VERSION >= 1000
namespace esriCatalog
{
	using esriServer::IServerObjectConfiguration;
	using esriServer::IServerObjectConfigurationPtr;
	//using esriGeoProcessing::IServerObjectConfiguration;
	//using esriGeoProcessing::IServerObjectConfigurationPtr;
}
#endif
#import "esriCatalog.olb" named_guids no_function_mapping

namespace esriFramework
{
	using esriSystemUI::IESRICommand;
	using esriSystemUI::IESRICommandPtr;

#if ARCGIS_VERSION > 910
	using ::UINT_PTR;
#endif
}

#if ARCGIS_VERSION <= 910
#import "esriFramework.olb" named_guids no_function_mapping rename("ICommand", "IESRICommand")
#else
#import "esriFramework.olb" named_guids no_function_mapping exclude("UINT_PTR") rename("ICommand", "IESRICommand")
#endif

namespace esriCartoUI
{
	using esriFramework::IApplication;
	using esriFramework::IApplicationPtr;

#if ARCGIS_VERSION <= 910
	using esriCatalog::IDataGraph;
	using esriCatalog::IDataGraphPtr;
#else
	using esriGeoDatabaseExtensions::ICadastralFabric;
	using esriGeoDatabaseExtensions::ICadastralFabricPtr;
#endif
}
#import "esriCartoUI.olb" named_guids no_function_mapping

#import "esriGeoAnalyst.olb" named_guids no_function_mapping

#if ARCGIS_VERSION >= 1000
#import "esriSearch.olb" named_guids no_function_mapping
#endif

#if ARCGIS_VERSION >= 1000
namespace esriCatalogUI
{
	using esriSearch::IItemInfo;
	using esriSearch::IItemInfoPtr;
}
#endif
#import "esriCatalogUI.olb" named_guids no_function_mapping
#import "esriGeoDatabaseUI.olb" named_guids no_function_mapping rename("IRow", "IESRIRow")
#import "esriArcMapUI.olb" named_guids no_function_mapping rename("ICommand", "IESRICommand")
#import "esriNetworkAnalysis.olb" named_guids no_function_mapping
#import "esriEditor.olb" named_guids no_function_mapping rename("IRow", "IESRIRow")

namespace esriEditorExt
{
	using esriEditor::IEditTask;
	using esriNetworkAnalysis::IJunctionFlagDisplay;
	using esriNetworkAnalysis::IJunctionFlagDisplayPtr;
	using esriNetworkAnalysis::IEdgeFlagDisplay;
	using esriNetworkAnalysis::IEdgeFlagDisplayPtr;
}
#import "esriEditorExt.olb" named_guids no_function_mapping

#import "esriDataSourcesNetCDF.olb" named_guids no_function_mapping
namespace esriAnimation
{
	using esriDataSourcesNetCDF::IMDDatasetView;
	using esriDataSourcesNetCDF::IMDDatasetViewPtr;
}
#import "esriAnimation.olb" named_guids no_function_mapping
namespace esri3DAnalyst
{
	using esriAnimation::IAnimationExtension;
	using esriAnimation::IAnimationExtensionPtr;
}
#import "esri3DAnalyst.olb" named_guids no_function_mapping

#import "esriGlobeCore.olb" named_guids no_function_mapping

#import "esriNetworkAnalyst.olb" named_guids no_function_mapping rename("IRow", "IESRIRow")

namespace esriControls
{
	using esri3DAnalyst::IScene;
	using esri3DAnalyst::IScenePtr;
	using esri3DAnalyst::ISceneGraph;
	using esri3DAnalyst::ISceneGraphPtr;
	using esri3DAnalyst::ISceneViewer;
	using esri3DAnalyst::ISceneViewerPtr;
	using esri3DAnalyst::ICamera;
	using esri3DAnalyst::ICameraPtr;
	using esriGlobeCore::IGlobePtr;
	using esriGlobeCore::IGlobeDisplayPtr;
	using esriGlobeCore::IGlobeViewerPtr;
	using esriGlobeCore::IGlobeCameraPtr;
	using esriNetworkAnalyst::INAClass;
	using esriNetworkAnalyst::INAClassPtr;
	using esriNetworkAnalyst::INALayer;
	using esriNetworkAnalyst::INALayerPtr;
	using esriNetworkAnalyst::INAStreetDirectionsContainer;
	using esriNetworkAnalyst::INAStreetDirectionsContainerPtr;
}

#import "esriControls.olb" named_guids no_function_mapping rename("ICommand", "IESRICommand") rename("IRow", "IESRIRow")
#import  "esriMaplex.olb" named_guids
namespace esriCore
{
	using namespace esriSystem;
	using namespace esriGeometry;
	using namespace esriDisplay;
	using namespace esriGeoDatabase;
	using namespace esriDataSourcesFile;
	using namespace esriDataSourcesRaster;
	using namespace esriDataSourcesGDB;
	using namespace esriSystemUI;
	using namespace esriServer;
	using namespace esriGISClient;
	using namespace esriOutput;
	using namespace esriCarto;
	using namespace esriCatalog;
	using namespace esriCartoUI;
	using namespace esriFramework;
	using namespace esriGeoAnalyst;
	using namespace esriGeoprocessing;
	using namespace esriCatalogUI;
	using namespace esriGeoDatabaseUI;
	using namespace esriArcMapUI;
	using namespace esriNetworkAnalysis;
	using namespace esriEditor;
	using namespace esriEditorExt;
	using namespace esriControls;
}

#pragma warning(pop)


