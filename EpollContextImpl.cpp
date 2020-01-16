#include "Precompile.h"
#include "IOContext.h"
#include "EpollContextImpl.h"


class ContextProcessor final : public EpollContextImpl::BaseContextProcessor
{
private:
	int _WorkerIndex;
public:
	ContextProcessor(int WorkerIndex, EpollContextImpl& Context)
		:BaseContextProcessor(Context), _WorkerIndex(WorkerIndex)
	{
	}
	virtual ~ContextProcessor()
	{
	}

	void EnqueueCompletedContext(const BaseContextUnitPtr Ptr)
	{
		{
			std::lock_guard<std::mutex> lock(_mutex);

			_ProcesWaitQueue.push(Ptr);
		}
	}

private:
	virtual void Process() override
	{
		BaseContextUnitPtr Ptr = nullptr;
		std::size_t QueueSize = _ProcesWaitQueue.size();

		while (true)
		{
			Ptr = nullptr;

			while (QueueSize > 0)
			{
				{
					std::lock_guard<std::mutex> lock(_mutex);

					Ptr = _ProcesWaitQueue.front();
					_ProcesWaitQueue.pop();

					QueueSize = _ProcesWaitQueue.size();
				}

				if (Ptr == nullptr)
					continue;

				_Context._CompletionCallbackPtr(_WorkerIndex, Ptr->transfferredBytes, Ptr);
				delete Ptr;
			}
		}
	}


};

class MainContextProcessor final : public EpollContextImpl::BaseContextProcessor
{
public:
	MainContextProcessor(EpollContextImpl& Context)
		:BaseContextProcessor(Context)
	{
	}

	virtual ~MainContextProcessor()
	{
	}

	void RegisterDescriptor(const Descriptor descriptor)
	{
		BaseContextUnitPtr Ptr = new BaseContextUnit;

		memset(Ptr, 0, sizeof(BaseContextUnit));

		Ptr->data.fd = descriptor;
		Ptr->events = EPOLLIN | EPOLLET;

		::epoll_ctl(_Context._contextHandle, EPOLL_CTL_ADD, descriptor, Ptr);

		{
			std::lock_guard<std::mutex> lock(_mutex);

			_ProcesWaitQueue.push(Ptr);
		}
	}
private:
	virtual void Process() override
	{
		BaseContextUnitPtr Ptr = nullptr;
		std::size_t QueueSize = _ProcesWaitQueue.size();

		while (true)
		{
			Ptr = nullptr;

			while (QueueSize > 0)
			{
				{
					std::lock_guard<std::mutex> lock(_mutex);

					Ptr = _ProcesWaitQueue.front();
					_ProcesWaitQueue.pop();

					QueueSize = _ProcesWaitQueue.size();
				}

				if (Ptr == nullptr)
					continue;


				int process = ::epoll_wait(_Context._contextHandle, Ptr, 1, -1);

				if (1 == process)
				{
					Descriptor desript = Ptr->data.fd;

					int32_t transfferedBytes = read(desript, Ptr->contextBuffer, Ptr->bufferlength);

					Ptr->transfferredBytes = transfferedBytes;

					::epoll_ctl(_Context._contextHandle, EPOLL_CTL_DEL, desript, Ptr);
					_Context.Post(Ptr);
				}
			}
		}
	}

};

EpollContextImpl::EpollContextImpl(const uint32_t numOfWorker)
{
	_WorkerProcessor.resize(numOfWorker);
}

EpollContextImpl::~EpollContextImpl()
{
	Stop();
}

void EpollContextImpl::Initialize(const uint32_t timeout, IOCompleteCallbackPtr CompletionPtr, IOEtcErrorCallbackPtr ErrorPtr)
{
	super::Initialize(timeout, CompletionPtr, ErrorPtr);

	_MainProcessor = std::make_shared<MainContextProcessor>(this);

	int index = 0;
	for (auto Processor : _WorkerProcessor)
	{
		Processor = std::make_shared<ContextProcessor>(index++, this);
	}
}

void EpollContextImpl::RegisterDescriptor(const Descriptor descriptor)
{
	_MainProcessor->RegisterDescriptor(descriptor);
}

void EpollContextImpl::Post(const BaseContextUnitPtr pContext)
{
	uint32_t MaxIndex = _WorkerProcessor.size();
	static std::atomic_uint32_t RoundRobinIndex;

	_WorkerProcessor[RoundRobinIndex]->EnqueueCompletedContext(pContext);

	++RoundRobinIndex;
	RoundRobinIndex = RoundRobinIndex % MaxIndex;
}
