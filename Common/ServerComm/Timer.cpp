#include "Timer.h"

struct event_base* LibEventTimer::base = nullptr;

LibEventTimer::LibEventTimer()
{
	m_nBaseTimerID = 0;
	m_TimerParsMap.clear();
}

LibEventTimer::~LibEventTimer()
{
	TimerParMap::iterator it = m_TimerParsMap.begin();
	for (; it != m_TimerParsMap.end();)
	{
		TimerPar& timerPar = it->second;
		if (timerPar.pEvent)
		{
			evtimer_del(timerPar.pEvent);
			delete timerPar.pEvent;
			it = m_TimerParsMap.erase(it);
		}
		else
			++it;
	}
	m_TimerParsMap.clear();
	m_nBaseTimerID = 0;
}

bool LibEventTimer::Init()
{
	base = event_base_new();

	return true;
}

void LibEventTimer::UnInit()
{
	if (base)
	{
		event_base_free(base);
		base = nullptr;
	}
}

void OnTimer(evutil_socket_t sock, short event, void *arg)
{
	LibEventTimer::TimerPar* pPar = (LibEventTimer::TimerPar*)arg;
	if (pPar->bLoop)
	{
		float fIntervalTime = pPar->fIntervalTime;
		time_t nScend = (time_t)fIntervalTime;
		time_t nUsec = (fIntervalTime - nScend) * 1000000;
		struct timeval tv;
		tv.tv_sec = nScend;
		tv.tv_usec = nUsec;
		evtimer_add(pPar->pEvent, &tv);
	}
	if (pPar->callBack)
		pPar->callBack(pPar->nTimerID);
}


int LibEventTimer::Run_EveryTimer(float fIntervalTime, Callback callBack)
{
	return CreateTimer(fIntervalTime, callBack, true);
}
int LibEventTimer::Run_AfterTimer(float fIntervalTime, Callback callBack)
{
	return CreateTimer(fIntervalTime, callBack, false);
}

int LibEventTimer::CreateTimer(float fIntervalTime, Callback callBack, bool bLoop)
{
	time_t nScend = (time_t)fIntervalTime;
	time_t nUsec = (fIntervalTime - nScend) * 1000000;

	int nNewTimerID = m_nBaseTimerID++;

	TimerPar timerParm;
	timerParm.fIntervalTime = fIntervalTime;
	timerParm.callBack = callBack;
	timerParm.bLoop = bLoop;
	timerParm.nTimerID = nNewTimerID;

	struct timeval tv;
	tv.tv_sec = nScend;
	tv.tv_usec = nUsec;
	struct event* timer_ev = evtimer_new(base, OnTimer, &m_TimerParsMap[nNewTimerID]);
	timerParm.pEvent = timer_ev;
	m_TimerParsMap[nNewTimerID] = timerParm;
	evtimer_add(timer_ev, &tv);
	return nNewTimerID;
}

void LibEventTimer::CancelTimer(int nTimerID)
{
	TimerParMap::iterator it = m_TimerParsMap.find(nTimerID);
	if (it == m_TimerParsMap.end())
	{
		return;
	}
	if (it->second.pEvent)
	{
		evtimer_del(it->second.pEvent);
		delete it->second.pEvent;
		it->second.pEvent = nullptr;
	}
	m_TimerParsMap.erase(it);
}
void LibEventTimer::CancelAllTimer()
{
	auto it = m_TimerParsMap.begin();
	while (it != m_TimerParsMap.end())
	{
		int nTimerID = it->first;
		CancelTimer(nTimerID);
		it = m_TimerParsMap.begin();
	}
}


bool LibEventTimer::HeartBeat()
{
	event_base_loop(base, EVLOOP_NONBLOCK);
	return true;
}

