#include "Precompile.h"
#include "IOContext.h"
#include "IOContextImpl.h"


IOContextImpl::IOContextImpl()
	:_CompletionCallbackPtr(nullptr), _ErrorCallbackPtr(nullptr)
{
}

IOContextImpl::~IOContextImpl()
{
}

void IOContextImpl::Initialize(const uint32_t timeout,
	IOCompleteCallbackPtr CompletionPtr, IOEtcErrorCallbackPtr ErrorPtr)
{
	_timeout = timeout;

	_CompletionCallbackPtr = CompletionPtr;
	_ErrorCallbackPtr = ErrorPtr;
}
