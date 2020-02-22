#pragma once

#include <windows.h>

#include <cmath>
#include <string>
#include <iostream>
#include <type_traits>

#define KPAC kpac::
#define KPAC_NAMESPACE kpac
#define BEGIN_KPAC_NAMESPACE namespace KPAC_NAMESPACE {
#define END_KPAC_NAMESPACE }
#define USE_KPAC using namespace KPAC_NAMESPACE



BEGIN_KPAC_NAMESPACE


template<typename _Ty>
class COM_Object;

END_KPAC_NAMESPACE


template<typename _Ty>
bool operator== (const KPAC COM_Object<_Ty>& r0, const KPAC COM_Object<_Ty>& r1);
template<typename _Ty>
bool operator!= (const KPAC COM_Object<_Ty>& r0, const KPAC COM_Object<_Ty>& r1);

template<typename _Ty>
bool operator! (const KPAC COM_Object<_Ty>& r);



BEGIN_KPAC_NAMESPACE


template<typename _Ty>
class COM_Object
{
	static_assert(std::is_base_of<IUnknown, _Ty>::value);

private:
	_Ty* _obj;

public:
	COM_Object(_Ty* resource) :
		_obj{ resource }
	{}

	COM_Object(const COM_Object& obj) :
		_obj{ obj._obj }
	{
		if (_obj)
			_obj->AddRef();
	}
	COM_Object(COM_Object&& obj) noexcept :
		_obj{ std::move(obj._obj) }
	{
		obj._obj = nullptr;
	}

	~COM_Object()
	{
		if(_obj)
			_obj->Release();
	}

	COM_Object& operator= (const COM_Object& obj)
	{
		if (_obj && _obj != obj._obj)
			_obj->Release();
		_obj = obj._obj;
		_obj->AddRef();
		return *this;
	}
	COM_Object& operator= (COM_Object&& obj) noexcept
	{
		_obj = std::move(obj._obj);
		obj._obj = nullptr;
		return *this;
	}

	operator bool() const { return _obj; }

	_Ty* operator-> () { return _obj; }
	const _Ty* operator-> () const { return _obj; }

	_Ty* operator* () { return _obj; }
	const _Ty* operator* () const { return _obj; }


	template<typename _Ty>
	friend bool ::operator== (const KPAC COM_Object<_Ty>& r0, const KPAC COM_Object<_Ty>& r1);
	template<typename _Ty>
	friend bool ::operator!= (const KPAC COM_Object<_Ty>& r0, const KPAC COM_Object<_Ty>& r1);

	template<typename _Ty>
	friend bool ::operator! (const KPAC COM_Object<_Ty>& r);
};



std::wstring to_wstring(const wchar_t* wstr);

std::wstring get_process_name(DWORD pid);

END_KPAC_NAMESPACE


template<typename _Ty>
bool operator== (const KPAC COM_Object<_Ty>& obj0, const KPAC COM_Object<_Ty>& obj1) { return obj0._obj == obj1._obj; }
template<typename _Ty>
bool operator!= (const KPAC COM_Object<_Ty>& obj0, const KPAC COM_Object<_Ty>& obj1) { return obj0._obj != obj1._obj; }


template<typename _Ty>
bool operator! (const KPAC COM_Object<_Ty>& obj) { return !obj._obj; }


class Percentage;

template<typename _Ty>
_Ty operator% (const Percentage& p, const _Ty& value);

template<typename _Ty>
_Ty operator% (const _Ty& value, const Percentage& p);

template<typename _Ty>
_Ty& operator%= (_Ty& value, const Percentage& p);

bool operator! (const Percentage& p);

std::ostream& operator<< (std::ostream& os, const Percentage& p);
std::wostream& operator<< (std::wostream& os, const Percentage& p);

class Percentage
{
private:
	float _per;

public:
	inline Percentage(const float percentage, const float min = 0.f, const float max = 1.f) :
		_per{ (std::fminf(max, std::fmaxf(min, percentage)) - min) / (max - min) }
	{}
	Percentage(const Percentage&) = default;

	Percentage& operator= (const Percentage&) = default;

	inline operator bool() const { return _per; }
	inline operator float() const { return _per; }

	template<typename _Ty>
	static Percentage of(const _Ty& value, const float min = 0.f, const float max = 1.f)
	{
		return { static_cast<float>(value), min, max };
	}

	template<typename _Ty>
	friend _Ty operator% (const Percentage& p, const _Ty& value);

	template<typename _Ty>
	friend _Ty operator% (const _Ty& value, const Percentage& p);

	template<typename _Ty>
	friend _Ty& operator%= (_Ty& value, const Percentage& p);

	friend bool ::operator! (const Percentage& p);

	friend std::ostream& ::operator<< (std::ostream& os, const Percentage& p);
	friend std::wostream& ::operator<< (std::wostream& os, const Percentage& p);
};

template<typename _Ty>
_Ty operator% (const Percentage& p, const _Ty& value) { return p._per * value; }

template<typename _Ty>
_Ty operator% (const _Ty& value, const Percentage& p) { return value * p._per; }

template<typename _Ty>
_Ty& operator%= (_Ty& value, const Percentage& p) { return (value = value * p._per, value); }


