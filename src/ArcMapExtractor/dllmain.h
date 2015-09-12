// dllmain.h : Declaration of module class.

class CArcMapExtractorModule : public ATL::CAtlDllModuleT< CArcMapExtractorModule >
{
public :
	DECLARE_LIBID(LIBID_ArcMapExtractorLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ARCMAPEXTRACTOR, "{BEDD38DA-686D-4C27-9D71-437C49B824E3}")
};

extern class CArcMapExtractorModule _AtlModule;
