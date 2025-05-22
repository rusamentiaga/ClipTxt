#pragma once

#include <iostream>
#include <sstream>
#include <memory>
#include <stdexcept>
#include "tstring.h"


namespace AC
{
	class Exception : public std::runtime_error
	{
	private:
		mutable std::auto_ptr<AC::tostringstream> ss_;
		mutable std::string s_;

	public:
		Exception() :
			std::runtime_error(""),
			ss_(std::auto_ptr<AC::tostringstream>(new AC::tostringstream()))
		{
		}

		virtual ~Exception() throw()
		{
		}

		template <typename T>
		Exception & operator << (const T & t)
		{
			(*ss_) << t;
			return *this;
		}

		virtual const char * what() const throw()
		{
			try
			{
				tstring s = ss_->str();
				s_ = StrUtil::WideToAnsiString(s.c_str(), s.length());
				return s_.c_str();
			}
			catch (...)
			{
				return "Exception::what() threw an exception";
			}
		}
	};
}
