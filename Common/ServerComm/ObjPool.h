#pragma once

#include <map>
#include "Lock.h"
#include <iostream>
#include "CommonDef.h"

template<class T>
class ObjPool
{
	typedef std::map<int32_t, T*> ObjPoolType;
public:
	ObjPool():m_Mutex(){}
	~ObjPool()
	{
		for(auto iter = m_mUsedPool.begin(); iter!= m_mUsedPool.end(); iter++)
		{
			DELETE_PTR(iter->second);
		}
		for(auto iter = m_mUnUsedPool.begin(); iter!= m_mUnUsedPool.end(); iter++)
		{
			DELETE_PTR(iter->second);
		}
		m_mUnUsedPool.clear();
		m_mUsedPool.clear();
		m_nIndex = -1;
	}
	static ObjPool<T>& Instance()
	{
		static ObjPool objPool;
		return objPool;
	}

	void InitObjPool(int nCount)
	{
		m_nIndex = nCount;
		for(int i=0;i<m_nIndex;++i)
		{
			T* pObj = new T;
			if(pObj == nullptr)
			{
				std::cout << "new error" << std::endl;
				return;
			}
			m_mUnUsedPool.emplace(i, pObj);
		}

	}

	void Expand()
	{
		if(m_mUnUsedPool.empty())
		{
			int32_t nOldIndex = m_nIndex;
			m_nIndex += 100;
			for(int i=nOldIndex;i<m_nIndex;++i)
			{
				T* pObj = new T;
				if(pObj == nullptr)
				{
					std::cout << "new error" << std::endl;
					return;
				}
				m_mUnUsedPool.emplace(i, pObj);
			}
		}
	}

	T* NewObj()
	{
		AutoLock lock(m_Mutex);
		Expand();
		if(!m_mUnUsedPool.empty())
		{
			auto iter = m_mUnUsedPool.begin();
			if(iter != m_mUnUsedPool.end())
			{
				int32_t nIndex = iter->first;
				T* pObj = iter->second;
				m_mUsedPool.emplace(nIndex, pObj);
				m_mUnUsedPool.erase(iter);
				pObj->SetObjPoolId(nIndex);
				return pObj;
			}
		}
		return nullptr;
	}

	void ReleaseObj(T* pObj)
	{
		AutoLock Lock(m_Mutex);
		if(pObj == nullptr)
			return;
		int32_t nPoolId = pObj->GetObjPoolId();
		if(m_mUsedPool.find(nPoolId) == m_mUsedPool.end())
		{
			return;
		}
		pObj->Clear();
		pObj->SetObjPoolId(-1);
		m_mUnUsedPool.emplace(nPoolId, pObj);
		m_mUsedPool.erase(nPoolId);
	}

	int32_t GetObjNum()
	{
		AutoLock Lock(m_Mutex);
		return m_mUsedPool.size();
	}

	int32_t GetFreeObjNum()
	{
		AutoLock Lock(m_Mutex);
		return m_mUnUsedPool.size();
	}
private:
	Mutex m_Mutex;
	int32_t m_nIndex{-1};
	ObjPoolType m_mUnUsedPool;
	ObjPoolType m_mUsedPool;
};