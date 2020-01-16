#pragma once

using IOCompleteCallbackPtr = void (*)(const uint32_t, const uint32_t,
	BaseContextUnitPtr);

using IOEtcErrorCallbackPtr = void (*)(const uint32_t,
	BaseContextUnitPtr);

class IOContextImpl abstract
{
	friend class IOContext;
protected:
	Descriptor		_contextHandle;
	uint32_t		_timeout;

	IOContextImpl();
	virtual ~IOContextImpl();

	IOCompleteCallbackPtr _CompletionCallbackPtr;
	IOEtcErrorCallbackPtr _ErrorCallbackPtr;

	virtual void Initialize(const uint32_t timeout, 
		IOCompleteCallbackPtr CompletionPtr, 
		IOEtcErrorCallbackPtr ErrorPtr);

	virtual void RegisterDescriptor(const Descriptor descriptor) = 0;

	virtual void Stop() = 0;
	virtual void Post(const BaseContextUnitPtr pContext) = 0;
};

