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

class IOContext;
class IOCompleteionCallBack
{
private:
	IOContext* _Context;
public:
	IOCompleteionCallBack(IOContext* Context);

	void CallBack(const uint32_t WorkerIndex, const std::size_t& TransfferedBytes,
		BaseContextUnitPtr Ptr);
};
using IOCompleteCallbackPtr = std::shared_ptr<IOCompleteionCallBack>;

class IOEtcErrorCallBack
{
private:
	IOContext* _Context;
public:
	IOEtcErrorCallBack(IOContext* Context);

	void Callback(const uint32_t WorkerIndex, BaseContextUnitPtr Ptr);
};
using IOEtcErrorCallbackPtr = std::shared_ptr<IOEtcErrorCallBack>;


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

