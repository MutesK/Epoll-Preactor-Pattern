#pragma once

class IOCompleteionCallBack;
using IOCompleteCallbackPtr = std::shared_ptr<IOCompleteionCallBack>;

class IOEtcErrorCallBack;
using IOEtcErrorCallbackPtr = std::shared_ptr<IOEtcErrorCallBack>;

class IOContextImpl abstract
{
	friend class IOContext;
public:
	virtual ~IOContextImpl();
protected:
	Descriptor		_contextHandle;
	uint32_t		_timeout;

	IOContextImpl();


	IOCompleteCallbackPtr _CompletionCallbackPtr;
	IOEtcErrorCallbackPtr _ErrorCallbackPtr;

	virtual void Initialize(const uint32_t timeout, 
		IOCompleteCallbackPtr CompletionPtr, 
		IOEtcErrorCallbackPtr ErrorPtr);

	virtual void RegisterDescriptor(const Descriptor descriptor) = 0;

	virtual void Stop() = 0;
	virtual void Post(const BaseContextUnitPtr pContext) = 0;
};

