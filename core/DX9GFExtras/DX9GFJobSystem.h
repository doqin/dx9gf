#pragma once
#include <functional>
#include <atomic>
#include "DX9GFThreadPool.h"

namespace DX9GF {
	struct Job {
		std::function<void(void* data)> function;
		void* data;
	};

	struct BatchJob {
		std::function<void(void* batch, size_t index)> function;
		void* batch;
		size_t startIdx;
		size_t endIdx;
	};

	class JobSystem {
	private:
		std::atomic<int> pendingJobs{ 0 }; // lock free ring buffer
		ThreadPool pool;
	public:
		JobSystem(size_t threadPoolSize) : pool(threadPoolSize) {}
		void Dispatch(Job job);
		void DispatchBatch(BatchJob batch, size_t batchSize);
		bool IsBusy();
		void Wait();
	};
}