#pragma once
#include <functional>
#include <event2/event_compat.h>
#include <event2/event_struct.h>
#include <event2/event.h>
#include <map>

class LibEventTimer
{
public:
	typedef std::function<void(int)> Callback;
	struct TimerPar
	{
		TimerPar()
		{
			pEvent = NULL;
			fIntervalTime = 0;
			bLoop = false;
			nTimerID = -1;
		}
		struct event* pEvent;
		float fIntervalTime;
		Callback callBack;
		bool bLoop;
		int nTimerID;
	};
public:
	LibEventTimer();
	virtual~LibEventTimer();
	int Run_EveryTimer(float fIntervalTime, Callback callBack);
	int Run_AfterTimer(float fIntervalTime, Callback callBack);
	void CancelTimer(int nTimerID);
	void CancelAllTimer();

	static bool Init();
	static void UnInit();

	static bool HeartBeat();

private:
	int CreateTimer(float fIntervalTime, Callback callBack, bool bLoop);
private:

	static struct event_base* base;

	int m_nBaseTimerID{0};
	typedef std::map<int, TimerPar> TimerParMap;
	TimerParMap m_TimerParsMap;
};

