#pragma once

#include <string>

#define KPAC kpac::
#define KPAC_NAMESPACE kpac
#define BEGIN_KPAC_NAMESPACE namespace KPAC_NAMESPACE {
#define END_KPAC_NAMESPACE }
#define USE_KPAC using namespace KPAC_NAMESPACE



BEGIN_KPAC_NAMESPACE


template<typename _Ty>
class Resource;

template<typename _Ty>
class ResourceReference;

END_KPAC_NAMESPACE


template<typename _Ty>
bool operator== (const KPAC Resource<_Ty>& r0, const KPAC Resource<_Ty>& r1);
template<typename _Ty>
bool operator!= (const KPAC Resource<_Ty>& r0, const KPAC Resource<_Ty>& r1);

template<typename _Ty>
bool operator== (const KPAC ResourceReference<_Ty>& r0, const KPAC ResourceReference<_Ty>& r1);
template<typename _Ty>
bool operator!= (const KPAC ResourceReference<_Ty>& r0, const KPAC ResourceReference<_Ty>& r1);

template<typename _Ty>
bool operator== (const KPAC Resource<_Ty>& r0, const KPAC ResourceReference<_Ty>& r1);
template<typename _Ty>
bool operator!= (const KPAC Resource<_Ty>& r0, const KPAC ResourceReference<_Ty>& r1);

template<typename _Ty>
bool operator== (const KPAC ResourceReference<_Ty>& r0, const KPAC Resource<_Ty>& r1);
template<typename _Ty>
bool operator!= (const KPAC ResourceReference<_Ty>& r0, const KPAC Resource<_Ty>& r1);

template<typename _Ty>
bool operator! (const KPAC Resource<_Ty>& r);

template<typename _Ty>
bool operator! (const KPAC ResourceReference<_Ty>& r);



BEGIN_KPAC_NAMESPACE


template<typename _Ty>
class ResourceReference
{
private:
	_Ty* _res;

public:
	ResourceReference(_Ty* resource) :
		_res{ resource }
	{}
	ResourceReference(const ResourceReference&) = default;
	ResourceReference(ResourceReference&&) = default;

	ResourceReference& operator= (const ResourceReference&) = default;
	ResourceReference& operator= (ResourceReference&&) = default;

	operator bool() const { return _res; }

	_Ty* operator-> () { return _res; }
	const _Ty* operator-> () const { return _res; }

	_Ty* operator* () { return _res; }
	const _Ty* operator* () const { return _res; }

	template<typename _Ty>
	friend bool ::operator== (const KPAC ResourceReference<_Ty>& r0, const KPAC ResourceReference<_Ty>& r1);
	template<typename _Ty>
	friend bool ::operator!= (const KPAC ResourceReference<_Ty>& r0, const KPAC ResourceReference<_Ty>& r1);

	template<typename _Ty>
	friend bool ::operator== (const KPAC Resource<_Ty>& r0, const KPAC ResourceReference<_Ty>& r1);
	template<typename _Ty>
	friend bool ::operator!= (const KPAC Resource<_Ty>& r0, const KPAC ResourceReference<_Ty>& r1);

	template<typename _Ty>
	friend bool ::operator== (const KPAC ResourceReference<_Ty>& r0, const KPAC Resource<_Ty>& r1);
	template<typename _Ty>
	friend bool ::operator!= (const KPAC ResourceReference<_Ty>& r0, const KPAC Resource<_Ty>& r1);

	template<typename _Ty>
	friend bool ::operator! (const KPAC ResourceReference<_Ty>& r);
};

template<typename _Ty>
class Resource
{
private:
	_Ty* _res;

public:
	Resource(_Ty* resource) :
		_res{ resource }
	{}

	Resource(const Resource&) = delete;
	Resource(Resource&& r) noexcept :
		_res{ std::move(r._res) }
	{
		r._res = nullptr;
	}

	~Resource()
	{
		if(_res)
			_res->Release();
	}

	Resource& operator= (const Resource&) = delete;
	Resource& operator= (Resource&& r)
	{
		_res = std::move(r._res);
		r._res = nullptr;
		return *this;
	}

	operator bool() const { return _res; }

	_Ty* operator-> () { return _res; }
	const _Ty* operator-> () const { return _res; }

	_Ty* operator* () { return _res; }
	const _Ty* operator* () const { return _res; }

	ResourceReference<_Ty> operator& () { return { _res }; }
	const ResourceReference<_Ty> operator& () const { return { _res }; }


	template<typename _Ty>
	friend bool ::operator== (const KPAC Resource<_Ty>& r0, const KPAC Resource<_Ty>& r1);
	template<typename _Ty>
	friend bool ::operator!= (const KPAC Resource<_Ty>& r0, const KPAC Resource<_Ty>& r1);

	template<typename _Ty>
	friend bool ::operator== (const KPAC Resource<_Ty>& r0, const KPAC ResourceReference<_Ty>& r1);
	template<typename _Ty>
	friend bool ::operator!= (const KPAC Resource<_Ty>& r0, const KPAC ResourceReference<_Ty>& r1);

	template<typename _Ty>
	friend bool ::operator== (const KPAC ResourceReference<_Ty>& r0, const KPAC Resource<_Ty>& r1);
	template<typename _Ty>
	friend bool ::operator!= (const KPAC ResourceReference<_Ty>& r0, const KPAC Resource<_Ty>& r1);

	template<typename _Ty>
	friend bool ::operator! (const KPAC Resource<_Ty>& r);
};



std::wstring to_wstring(const wchar_t* wstr);

END_KPAC_NAMESPACE


template<typename _Ty>
bool operator== (const KPAC Resource<_Ty>& r0, const KPAC Resource<_Ty>& r1) { return r0._res == r1._res; }
template<typename _Ty>
bool operator!= (const KPAC Resource<_Ty>& r0, const KPAC Resource<_Ty>& r1) { return r0._res != r1._res; }

template<typename _Ty>
bool operator== (const KPAC ResourceReference<_Ty>& r0, const KPAC ResourceReference<_Ty>& r1) { return r0._res == r1._res; }
template<typename _Ty>
bool operator!= (const KPAC ResourceReference<_Ty>& r0, const KPAC ResourceReference<_Ty>& r1) { return r0._res != r1._res; }

template<typename _Ty>
bool operator== (const KPAC Resource<_Ty>& r0, const KPAC ResourceReference<_Ty>& r1) { return r0._res == r1._res; }
template<typename _Ty>
bool operator!= (const KPAC Resource<_Ty>& r0, const KPAC ResourceReference<_Ty>& r1) { return r0._res != r1._res; }

template<typename _Ty>
bool operator== (const KPAC ResourceReference<_Ty>& r0, const KPAC Resource<_Ty>& r1) { return r0._res == r1._res; }
template<typename _Ty>
bool operator!= (const KPAC ResourceReference<_Ty>& r0, const KPAC Resource<_Ty>& r1) { return r0._res != r1._res; }


template<typename _Ty>
bool operator! (const KPAC Resource<_Ty>& r) { return !r._res; }

template<typename _Ty>
bool operator! (const KPAC ResourceReference<_Ty>& r) { return !r._res; }
