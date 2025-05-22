#pragma once
#include "SharedResource.h"

template<typename T>
class SharedPtr: public AC::SharedResource< SharedPtr<T> >
{
protected:
	T* m_pT;

	void Dispose()
	{
		delete m_pT;
	}

public:
	SharedPtr(T* pT = NULL): m_pT(pT) 
	{
	}

	SharedPtr(const SharedPtr& sp) 
		:SharedResource<SharedPtr>(sp), m_pT(sp.m_pT)
	{
	}

	bool operator==(const SharedPtr& sp) 
	{
		return m_pT == sp.m_pT;
	}

	bool operator!=(const SharedPtr& sp) 
	{
		return m_pT != sp.m_pT;
	}

	SharedPtr& operator=(const SharedPtr& sp) 
	{
		if (this != &sp) 
		{
			SharedResource<SharedPtr>::operator =(sp);
			m_pT = sp.m_pT;
		}
		return *this;
	}
	T* operator->() { return m_pT; }
	T& operator*() { return *m_pT; }
};