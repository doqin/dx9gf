#pragma once
#include <functional>
#include <atomic>
#include "DX9GFThreadPool.h"

constexpr auto DEFAULT_BATCH_SIZE = 64;

namespace DX9GF {
	struct Job {
		std::function<void(void*)> function;
		void* data;
	};
	struct BatchJob {
		std::function<void(void*)> function;
		std::vector<void*>* batch;
	};

	class JobSystem {
	private:
		std::atomic<int> pendingJobs{ 0 }; // lock free ring buffer
		ThreadPool pool;
	public:
		JobSystem(size_t threadPoolSize) : pool(threadPoolSize) {}
		void Dispatch(Job job);
		void DispatchBatch(BatchJob batch, size_t batchSize = DEFAULT_BATCH_SIZE);
		bool IsBusy();
		void Wait();
	};
}