#include "DX9GFThreadPool.h"

DX9GF::ThreadPool::ThreadPool(size_t threadSize)
{
	for (size_t i = 0; i < threadSize; i++) {
		workers.emplace_back([this]() {
			while (true) {
				std::function<void(void)> task;
				{
					std::unique_lock<std::mutex> lock(this->queueMutex);
					this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); }); // sleep until the condition_variable wakes you up
					if (this->stop && this->tasks.empty()) return;
					task = std::move(this->tasks.front());
					this->tasks.pop();
				}
				task();
			}
		});
	}
}

DX9GF::ThreadPool::~ThreadPool()
{
	{
		std::unique_lock<std::mutex> lock(queueMutex);
		stop = true;
	}
	condition.notify_all();
	for (auto& worker : workers) {
		worker.join();
	}
}

void DX9GF::ThreadPool::Enqueue(std::function<void(void)> task)
{
	{
		std::unique_lock<std::mutex> lock(this->queueMutex);
		tasks.emplace(std::move(task));
	}
	condition.notify_one(); // wake up one waiter
}
