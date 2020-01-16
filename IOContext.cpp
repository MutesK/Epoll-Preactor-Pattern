#include "Precompile.h"
#include "IOContextImpl.h"
#include "IOContext.h"
#include "EpollContextImpl.h"

IOContext::IOContext(const uint32_t numOfWorker, const uint32_t timeout)
{
	auto IOCompletionCallback = [&](const uint32_t workerIndex,
		BaseContextUnitPtr Ptr, const uint32_t TransfferedBytes)
	{
		this->IOComplete(workerIndex, Ptr, TransfferedBytes);
	};

	auto IOErrorCallback = [&](const uint32_t workerIndex,
		BaseContextUnitPtr Ptr)
	{
		this->IOError(workerIndex, Ptr);
	};

#ifdef WIN32
#else
	_ContextImpl = make_unique<EpollContextImpl>(numOfWorker,
		timeout, IOCompletionCallback, IOErrorCallback);
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
