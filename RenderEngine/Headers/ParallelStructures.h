#pragma once
#include <map>
#include <mutex>

template<class Key,class T,class Compare = std::less<Key>,class Allocator = std::allocator<std::pair<const Key,T>>>
class ParallelMap:public std::map<Key, T, Compare, Allocator>
{
public:
    using iterator = typename std::map<Key, T, Compare, Allocator>::iterator;
    using const_iterator = typename std::map<Key, T, Compare, Allocator>::const_iterator;

	T& operator[](const Key& key)
	{
		std::lock_guard<std::mutex> guard(mut);
		return std::map<Key, T, Compare, Allocator>::operator[](key);
	}
	T& operator[](Key&& key)
	{
		std::lock_guard<std::mutex> guard(mut);
		return std::map<Key, T, Compare, Allocator>::operator[](key);
	}
	iterator begin() noexcept
	{
		std::lock_guard<std::mutex> guard(mut);
		return std::map<Key, T, Compare, Allocator>::begin();
	}
	iterator end() noexcept
	{
		std::lock_guard<std::mutex> guard(mut);
		return std::map<Key, T, Compare, Allocator>::end();
	}
	const_iterator begin() const noexcept
	{
		std::lock_guard<std::mutex> guard(mut);
		return std::map<Key, T, Compare, Allocator>::begin();
	}

	const_iterator end() const noexcept
	{
		std::lock_guard<std::mutex> guard(mut);
		return std::map<Key, T, Compare, Allocator>::end();
	}
	const_iterator cbegin() const noexcept
	{
		std::lock_guard<std::mutex> guard(mut);
		return std::map<Key, T, Compare, Allocator>::cbegin();
	}
	const_iterator cend() const noexcept
	{
		std::lock_guard<std::mutex> guard(mut);
		return std::map<Key, T, Compare, Allocator>::cend();
	}

	iterator find(const Key& key)
	{
		std::lock_guard<std::mutex> guard(mut);
		return std::map<Key, T, Compare, Allocator>::find(key);
	}
	const_iterator find(const Key& key) const
	{
		std::lock_guard<std::mutex> guard(mut);
		return std::map<Key, T, Compare, Allocator>::find(key);
	}

	iterator erase(const_iterator pos)
	{
		std::lock_guard<std::mutex> guard(mut);
		return std::map<Key, T, Compare, Allocator>::erase(pos);
	}
	iterator erase(iterator pos)
	{
		std::lock_guard<std::mutex> guard(mut);
		return std::map<Key, T, Compare, Allocator>::erase(pos);
	}
	iterator erase(const_iterator first, const_iterator last)
	{
		std::lock_guard<std::mutex> guard(mut);
		return std::map<Key, T, Compare, Allocator>::erase(first, last);
	}

	void clear() noexcept
	{
		std::lock_guard<std::mutex> guard(mut);
		std::map<Key, T, Compare, Allocator>::clear();
	}
private:
	std::mutex mut;
};
