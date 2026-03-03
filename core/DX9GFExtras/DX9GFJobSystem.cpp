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
	auto& function = batchJob.function;
	auto endIdx = batchJob.endIdx;
	for (size_t startIdx = batchJob.startIdx; startIdx < endIdx; startIdx += batchSize) {
		// task per batch
		this->Dispatch({ [startIdx, endIdx, batchSize, function](void* data) {
			for (size_t idx = startIdx; idx < std::min(startIdx + batchSize, endIdx); ++idx) {
				function(data, idx);
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
