#include "Precompile.h"
#include "IOContext.h"
#include "EpollContextImpl.h"




IOContext::IOContext(const uint32_t numOfWorker, const uint32_t timeout)
{
#ifdef WIN32
#else
	_ContextImpl = std::unique_ptr<EpollContextImpl>(new EpollContextImpl(numOfWorker));

	_ContextImpl->Initialize(timeout,
		std::make_shared<IOCompleteionCallBack>(this),
		std::make_shared<IOEtcErrorCallBack>(this));
#endif
}

IOContext::~IOContext()
{
	_ContextImpl->Stop();
}

void IOContext::RegisterDescriptor(const Descriptor destrip)
{
	_ContextImpl->RegisterDescriptor(destrip);
}


IOCompleteionCallBack::IOCompleteionCallBack(IOContext* Context)
	:_Context(Context)
{
}

void IOCompleteionCallBack::CallBack(const uint32_t WorkerIndex, const std::size_t& TransfferedBytes, BaseContextUnitPtr Ptr)
{
	_Context->IOComplete(WorkerIndex, Ptr, TransfferedBytes);
}

IOEtcErrorCallBack::IOEtcErrorCallBack(IOContext* Context)
	:_Context(Context)
{
}

void IOEtcErrorCallBack::Callback(const uint32_t WorkerIndex, BaseContextUnitPtr Ptr)
{
	_Context->IOError(WorkerIndex, Ptr);
}
