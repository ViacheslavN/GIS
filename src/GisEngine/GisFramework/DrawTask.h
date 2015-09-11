#ifndef GIS_ENGINE_GIS_FRAEMWORK_DRAW_TASK_
#define GIS_ENGINE_GIS_FRAEMWORK_DRAW_TASK_
namespace GisEngine
{
	namespace GisFramework
	{
		struct IDrawTask
		{
		public: 
			virtual ~IDrawTask(){}
			virtual void Draw() = 0;
			virtual void StopDraw(bool bWait = true) = 0;
			virtual void SetTrackCancel(bool bSet) = 0;
		};
	}
}
#endif