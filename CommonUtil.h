#pragma once

#include <random>
#include <memory>
#include <functional>

class CommonUtil
{
public:
	//随机一个整数[min, max]
	static int32_t Random(int32_t min, int32_t max)
	{
		static std::default_random_engine _generator;
		std::uniform_int_distribution<int> distribution(min, max);
		return distribution(_generator);
	}

	template<class IT, class WEIGHT>
	static int RandomByWeight(const IT& begin, const IT& end, WEIGHT w)
	{
		int sum = 0;
		for(auto it = begin; it != end; ++it) sum += w(*it);
		if(sum <= 0) return -1;
		int rnd = Random(0, sum - 1);
		size_t index = 0;
		int weight = 0;
		for(auto it = begin; it != end; ++it, ++index)
		{
			weight = w(*it);
			if(rnd < weight)
				return index;
			rnd -= weight;
		}
		return -1;
	}
	
	template<class IT, class WEIGHT>
	static IT RandomItorByWeight(const IT& begin, const IT& end, WEIGHT w)
	{
		int sum = 0;
		for(auto it = begin; it != end; ++it) sum += w(*it);
		if(sum <= 0) return end;
		int rnd = Random(0, sum - 1);
		int weight = 0;
		for(auto it = begin; it != end; ++it)
		{
			weight = w(*it);
			if(rnd < weight)
				return it;
			rnd -= weight;
		}
		return end;
	}

	template<typename T, typename ...Args>
	std::unique_ptr<T> make_unique(Args&& ...args)
	{
		return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
	}
};
