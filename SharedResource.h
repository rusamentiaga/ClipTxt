#pragma once
#include "Windows.h"

namespace AC
{
    template <class T>
    class SharedResource
    {
    private:
		struct DerivedResource: public T
		{
			DerivedResource() {}
			static void DoDispose(T& t)
			{
				void (T::*fn)() = &DerivedResource::Dispose;
				(t.*fn)();
			}
		};

		LONG* m_pCount;

		void Release()
        {
			if (!InterlockedDecrement(m_pCount))
			{
				delete m_pCount;
				DerivedResource::DoDispose(static_cast<T&>(*this));
			}
		}

		void Acquire(const SharedResource& rhs)
		{
			m_pCount = rhs.m_pCount;
			InterlockedIncrement(m_pCount);
		}

    public:
		SharedResource() 
			: m_pCount(new LONG(1)) {}

        SharedResource(const SharedResource<T>& rhs) 
        {
			Acquire(rhs);
		}

        SharedResource& operator=(const SharedResource<T>& rhs)
        {
			if (this != &rhs) 
			{
				Release();
				Acquire(rhs);
			}
            return *this;
        }

		~SharedResource()
        {
			Release();
		}

#ifdef TESTING_AC
		int GetRefCount() { return *m_pCount; }
#endif
    };
}