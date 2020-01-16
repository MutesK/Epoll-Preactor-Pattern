#pragma once

#ifdef WIN32
using BaseContextUnit = struct OVERLAPPED;
#else
struct BaseContextUnit : public epoll_event
{
	char*				contextBuffer; // Recv, Send  요청에 들어가야됨.
	uint32_t			bufferlength;


	int32_t				transfferredBytes;
};
#endif

using BaseContextUnitPtr = BaseContextUnit*;
using CompletionKey = void*;

class IOContextImpl;
class IOContext abstract
{
	std::unique_ptr<IOContextImpl> _ContextImpl;
public:
	IOContext(const uint32_t numOfWorker = std::thread::hardware_concurrency(), 
		const uint32_t timeout = INFINITE);
	virtual ~IOContext();

	void RegisterDescriptor(const Descriptor destrip);

	virtual void IOComplete(const uint32_t workerIndex, BaseContextUnitPtr CompletionContext,
		const uint32_t TransferredBytes) = 0;

	virtual void IOError(const uint32_t workerIndex,
		BaseContextUnitPtr ErrorContext) = 0;
};

