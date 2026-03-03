#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>

namespace DX9GF {
	class ThreadPool {
	private:
		std::vector<std::thread> workers;
		std::queue<std::function<void(void)>> tasks;
		std::mutex queueMutex;
		std::condition_variable condition;
		bool stop = false;
	protected:
	public:
		ThreadPool(size_t threadSize);
		~ThreadPool();
		void Enqueue(std::function<void(void)> task);
	};
}