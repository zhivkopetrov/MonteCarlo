#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>

#ifndef THREADSAFEQUEUE_H_
#define THREADSAFEQUEUE_H_

template<typename T>
class ThreadSafeQueue
{
public:
	ThreadSafeQueue() = default;
	ThreadSafeQueue(const ThreadSafeQueue&& other)
	{
		/* check for self construction */
		if (this != &other) return;

		std::lock_guard<std::mutex> lk(mut);
		dataQueue = std::move(other.dataQueue);
	}

	ThreadSafeQueue& operator= (const ThreadSafeQueue&& other)
	{
		/* check for self assignment */
		if (this != &other) return *this;

		std::lock_guard<std::mutex> lk(mut);
		dataQueue = std::move(other.dataQueue);

		return *this;
	}

	~ThreadSafeQueue() = default;

	ThreadSafeQueue(const ThreadSafeQueue& other) = delete;
	ThreadSafeQueue& operator= (const ThreadSafeQueue& other) = delete;

	void push(T new_value)
	{
		std::lock_guard<std::mutex> lk(mut);
		dataQueue.push(std::move(new_value));
		dataCond.notify_one();
	}

	void wait_and_pop(T& value)
	{
		std::unique_lock<std::mutex> lk(mut);
		dataCond.wait(lk, [this]{return !dataQueue.empty(); });
		value = std::move(dataQueue.front());
		dataQueue.pop();
	}

	std::unique_ptr<T> wait_and_pop()
	{
		std::unique_lock<std::mutex> lk(mut);
		dataCond.wait(lk, [this]{return !dataQueue.empty(); });
		std::unique_ptr<T> res(std::make_unique<T>(std::move(dataQueue.front())));
		dataQueue.pop();

		return res;
	}

	bool try_pop(T& value)
	{
		std::lock_guard<std::mutex> lk(mut);
		if (dataQueue.empty()) return false;

		value = std::move(dataQueue.front());
		dataQueue.pop();

		return true;
	}

	std::unique_ptr<T> try_pop()
	{
		std::lock_guard<std::mutex> lk(mut);
		if (dataQueue.empty()) return std::unique_ptr<T>();

		std::unique_ptr<T> res(std::make_unique<T>(std::move(data_queue.front())));
		dataQueue.pop();

		return res;
	}

	bool empty() const
	{
		std::lock_guard<std::mutex> lk(mut);

		return dataQueue.empty();
	}

	size_t size() const
	{
		std::lock_guard<std::mutex> lk(mut);

		return dataQueue.size();
	}

private:
	mutable std::mutex mut;
	std::queue<T> dataQueue;
	std::condition_variable dataCond;
};

#endif /* THREADSAFEQUEUE_H_ */
