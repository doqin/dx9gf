#include "DX9GFJobSystem.h"

void DX9GF::JobSystem::Dispatch(Job job)
{
	pendingJobs.fetch_add(1);
	pool.Enqueue([job, this]() {
		job.function(job.data);
		pendingJobs.fetch_sub(1);
	});
}

void DX9GF::JobSystem::DispatchBatch(BatchJob batchJob, size_t batchSize)
{
	auto dataSize = batchJob.batch->size();
	auto& function = batchJob.function;
	for (size_t startIdx = 0; startIdx < dataSize; startIdx += batchSize) {
		// task per batch
		this->Dispatch({ [startIdx, dataSize, batchSize, function](void* data) {
			auto endIdx = std::min(startIdx + batchSize, dataSize);
			auto dataList = static_cast<std::vector<void*>*>(data);
			for (size_t idx = startIdx; idx < endIdx; ++idx) {
				function((*dataList)[idx]);
			}
		}, batchJob.batch });
	}
}

bool DX9GF::JobSystem::IsBusy()
{
	return pendingJobs.load() > 0;
}

void DX9GF::JobSystem::Wait()
{
	while (IsBusy()) {
		std::this_thread::yield();
	}
}
