#pragma once

#include "IOContextImpl.h"

class EpollContextImpl : public IOContextImpl
{
	typedef IOContextImpl super;

	class BaseContextProcessor abstract
	{
	protected:
		std::thread				_processor;
		std::mutex				_mutex;
		EpollContextImpl& _Context;
		std::queue<BaseContextUnitPtr>  _ProcesWaitQueue;

		BaseContextProcessor(EpollContextImpl* Context)
			:_Context(*Context)
		{
			_processor = std::thread(std::mem_fn(&BaseContextProcessor::Process), this);
		}

	public:
		virtual ~BaseContextProcessor()
		{
			_processor.join();
		}
	
		virtual void Process() = 0;

		virtual void RegisterDescriptor(const Descriptor descriptor)
		{
			throw;
		}

		virtual void EnqueueCompletedContext(const BaseContextUnitPtr pContext)
		{
			throw;
		}
	};

	std::shared_ptr<BaseContextProcessor>		   _MainProcessor;
	std::vector<std::shared_ptr<BaseContextProcessor>> _WorkerProcessor;
private:
	EpollContextImpl(const uint32_t numOfWorker);

	friend class IOContext;
	friend class BaseContextProcessor;
	friend class ContextProcessor;
	friend class MainContextProcessor;


public:
	virtual ~EpollContextImpl();

	virtual void Initialize(const uint32_t timeout,
		IOCompleteCallbackPtr CompletionPtr,
		IOEtcErrorCallbackPtr ErrorPtr) override;

	virtual void RegisterDescriptor(const Descriptor descriptor) override;

	virtual void Stop() override;
	virtual void Post(const BaseContextUnitPtr pContext) override;

};

