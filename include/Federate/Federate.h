#ifndef H_HELLEBORECONSULTING_FEDERATE_H
#define H_HELLEBORECONSULTING_FEDERATE_H

// www.helleboreconsulting.com

///
///	\author	John Farrier
///

#include <functional>
#include <vector>
#include <future>
#include <memory>

///
/// A dummy template class which will be used to supporess warnings from unused variables. 
/// The "unused variables" actually scope mutex locks on resources, so they ARE used.
///
template<typename T> inline void SuppressWarningUnusedVariable(T const&)
{
}

///
/// Mixin with conditional template parameter.
///
template<bool, typename T> struct VectorMember
{
	std::vector<T> vec;
};

///
/// Mixin with conditional template parameter.
///
template<typename T> struct VectorMember<true, T>
{
	std::vector<std::weak_ptr<T>> vec;
};

///
/// Mixin with conditional template parameter.
///
template<bool> struct MutexMember
{
	int acquire() const
	{
		return 0;
	}
};

///
/// Mixin with conditional template parameter.
///
template<> struct MutexMember<true>
{
	std::lock_guard<std::mutex> acquire() const
	{
		return std::lock_guard<std::mutex>(this->access);
	}

	mutable std::mutex access;
};

///
/// Base class for all Federate classes.
/// This consolodates some of the copy-paste implementation that would otherwise be required.
///
template<typename FederateFunction, bool Tracked, bool ThreadSafe> class FederateBase
{
	public:
		typedef std::shared_ptr<FederateFunction> Tracker;
		typedef std::weak_ptr<FederateFunction> WeakTracker;

		///
		/// Adds a new function to the end of the Federate.
		/// Non-Tracked Version.
		///
		template<typename = typename std::enable_if<!Tracked>::type> 
		void push_back(FederateFunction f)
		{
			auto scopedLock = this->lock.acquire();
			SuppressWarningUnusedVariable(scopedLock);
			this->functions.vec.emplace_back(std::move(f));
		}

		///
		/// Adds a new function to the end of the Federate.
		/// Tracked Version.
		///
		template<typename = typename std::enable_if<Tracked>::type>
		Tracker push_back(FederateFunction f)
		{
			auto tracker = std::make_shared<FederateFunction>(std::move(f));
			auto scopedLock = this->lock.acquire();
			SuppressWarningUnusedVariable(scopedLock);
			this->functions.vec.push_back(tracker);
			return tracker;
		}

		///
		/// Returns the number of functions in the Federate.
		///
		size_t size() const
		{
			auto scopedLock = this->lock.acquire();
			SuppressWarningUnusedVariable(scopedLock);
			return this->functions.vec.size();
		}

		///
		/// Clears the functions in the Federate.
		///
		void clear()
		{
			auto scopedLock = this->lock.acquire();
			SuppressWarningUnusedVariable(scopedLock);
			this->functions.vec.clear();
		}

		///
		/// Returns true if the Federate is empty.
		///
		bool empty() const
		{
			auto scopedLock = this->lock.acquire();
			SuppressWarningUnusedVariable(scopedLock);
			return this->functions.vec.empty();
		}

		///
		/// Determines how many tracked objects are expired.
		/// The non-tracked version will always return 0.
		///
		size_t garbageSize() const
		{
			return this->garbageSizeTracked(std::integral_constant<bool, Tracked>());
		}

		///
		/// Removes all tracked objects that are expired.
		///
		void clean()
		{
			this->cleanTracked(std::integral_constant<bool, Tracked>());
		}

	protected:
		void cleanTracked(std::true_type)
		{
			auto scopedLock = this->lock.acquire();
			SuppressWarningUnusedVariable(scopedLock);
			this->functions.vec.erase(std::remove_if(std::begin(this->functions.vec), std::end(this->functions.vec),
				[](WeakTracker& f)->bool
			{
				return f.lock() == nullptr;
			}), std::end(this->functions.vec));
		}

		void cleanTracked(std::false_type)
		{
		}

		size_t garbageSizeTracked(std::true_type) const
		{
			auto scopedLock = this->lock.acquire();
			SuppressWarningUnusedVariable(scopedLock);

			return std::count_if(std::begin(this->functions.vec), std::end(this->functions.vec),
				[](const WeakTracker& f)->bool
			{
				return f.lock() == nullptr;
			});
		}

		size_t garbageSizeTracked(std::false_type) const
		{
			return 0;
		}

		VectorMember<Tracked, FederateFunction> functions;
		MutexMember<ThreadSafe> lock;
};

///
///
///
template<typename T, bool Tracked = false, bool ThreadSafe = false> class Federate
{
};

///
/// A non-thread safe Federate of non-tracked function objects.
/// For functions with the signature "R (Args...)"
///
template<typename R, typename... Args, bool Tracked, bool ThreadSafe> class Federate<R(Args...), Tracked, ThreadSafe> : public FederateBase<std::function<R(Args...)>, Tracked, ThreadSafe>
{
	public:
		/// To clean up the spelling throughout the function, define a type for our Federate functions.
		typedef std::function<R(Args...)> FederateFunction;

		///
		/// Invokes each of the functions in the Federate serially.
		/// Invokes each of the functions in the Federate serially with tracking.
		///
		std::vector<R> invoke(Args... args)
		{
			auto scopedLock = this->lock.acquire();
			SuppressWarningUnusedVariable(scopedLock);

			return this->invokeTracked(args..., std::integral_constant<bool, Tracked>());
		}

		///
		/// Invokes each of the functions in the Federate asynchronously.  
		/// Invokes each of the functions in the Federate asynchronously with tracking.
		/// Returns a vector of futures for the functions.
		///
		std::vector<std::future<R>> invokeAsync(Args... args)
		{
			auto scopedLock = this->lock.acquire();
			SuppressWarningUnusedVariable(scopedLock);

			return this->invokeAsyncTracked(args..., std::integral_constant<bool, Tracked>());
		}

	protected:
		std::vector<R> invokeTracked(Args... args, std::true_type)
		{
			std::vector<R> results;

			for(auto& f : this->functions.vec)
			{
				auto func = f.lock();

				if(func != nullptr)
				{
					results.push_back(func->operator()(args...));
				}
			}

			return results;
		}

		std::vector<R> invokeTracked(Args... args, std::false_type)
		{
			std::vector<R> results;

			for(auto& f : this->functions.vec)
			{
				results.push_back(f(args...));
			}

			return results;
		}

		std::vector<std::future<R>> invokeAsyncTracked(Args... args, std::true_type)
		{
			std::vector<std::future<R>> futures;

			for(auto& f : this->functions.vec)
			{
				auto func = f.lock();

				if(func != nullptr)
				{
					futures.emplace_back(std::async(std::launch::async,
						[func, args]()->R
					{
						return func(args...);
					}));
				}
			}

			return futures;
		}

		std::vector<std::future<R>> invokeAsyncTracked(Args... args, std::false_type)
		{
			std::vector<std::future<R>> futures;

			for(auto& f : this->functions.vec)
			{
				futures.emplace_back(std::async(std::launch::async,
					[f, args]()->R
				{
					return f(args...);
				}));
			}

			return futures;
		}
};

///
/// A non-thread safe Federate of non-tracked function objects.
/// For functions with the signature "void (Args...)"
///
template<typename... Args, bool Tracked, bool ThreadSafe> class Federate<void(Args...), Tracked, ThreadSafe> : public FederateBase<std::function<void(Args...)>, Tracked, ThreadSafe>
{
	public:
		/// To clean up the spelling throughout the function, define a type for our Federate functions.
		typedef std::function<void (Args...)> FederateFunction;

		///
		/// Invokes each of the functions in the Federate serially.
		///
		void invoke(Args... args)
		{
			auto scopedLock = this->lock.acquire();
			SuppressWarningUnusedVariable(scopedLock);

			this->invokeTracked(args..., std::integral_constant<bool, Tracked>());
		}

		///
		/// Invokes each of the functions in the Federate asynchronously.  
		/// Returns a vector of futures for the functions.vec.
		///
		std::vector<std::future<void>> invokeAsync(Args... args)
		{
			auto scopedLock = this->lock.acquire();
			SuppressWarningUnusedVariable(scopedLock);

			return this->invokeAsyncTracked(args..., std::integral_constant<bool, Tracked>());
		}

	protected:
		void invokeTracked(Args... args, std::true_type)
		{
			for(auto& f : this->functions.vec)
			{
				auto func = f.lock();

				if(func != nullptr)
				{
					func->operator()(args...);
				}
			}
		}

		void invokeTracked(Args... args, std::false_type)
		{
			for(auto& f : this->functions.vec)
			{
				f(args...);
			}
		}

		std::vector<std::future<void>> invokeAsyncTracked(Args... args, std::true_type)
		{
			std::vector<std::future<void>> futures;

			for(auto& f : this->functions.vec)
			{
				auto func = f.lock();

				if(func != nullptr)
				{
					futures.emplace_back(std::async(std::launch::async,
						[func, args...]()
					{
						return func->operator()(args...);
					}));
				}
			}

			return futures;
		}

		std::vector<std::future<void>> invokeAsyncTracked(Args... args, std::false_type)
		{
			std::vector<std::future<void>> futures;

			for(auto& f : this->functions.vec)
			{
				futures.emplace_back(std::async(std::launch::async,
					[f, args]()
				{
					return f(args...);
				}));
			}

			return futures;
		}
};

///
/// A non-thread safe Federate of non-tracked function objects.
/// For functions with the signature "R (void)"
///
template<typename R, bool Tracked, bool ThreadSafe> class Federate<R(void), Tracked, ThreadSafe> : public FederateBase<std::function<R(void)>, Tracked, ThreadSafe>
{
	public:
		/// To clean up the spelling throughout the function, define a type for our Federate functions.
		typedef std::function<R(void)> FederateFunction;

		///
		/// Invokes each of the functions in the Federate serially.
		///
		std::vector<R> invoke()
		{
			auto scopedLock = this->lock.acquire();
			SuppressWarningUnusedVariable(scopedLock);

			return this->invokeTracked(std::integral_constant<bool, Tracked>());
		}

		///
		/// Invokes each of the functions in the Federate asynchronously.  
		/// Returns a vector of futures for the functions.vec.
		///
		std::vector<std::future<R>> invokeAsync()
		{
			auto scopedLock = this->lock.acquire();
			SuppressWarningUnusedVariable(scopedLock);

			return this->invokeAsyncTracked(std::integral_constant<bool, Tracked>());
		}

	protected:
		std::vector<R> invokeTracked(std::true_type)
		{
			std::vector<R> results;

			for(auto& f : this->functions.vec)
			{
				auto func = f.lock();

				if(func != nullptr)
				{
					results.push_back(func->operator()());
				}
			}

			return results;
		}

		std::vector<R> invokeTracked(std::false_type)
		{
			std::vector<R> results;

			for(auto& f : this->functions.vec)
			{
				results.push_back(f());
			}

			return results;
		}

		std::vector<std::future<R>> invokeAsyncTracked(std::true_type)
		{
			std::vector<std::future<R>> futures;

			for(auto& f : this->functions.vec)
			{
				auto func = f.lock();

				if(func != nullptr)
				{
					futures.emplace_back(std::async(std::launch::async,
						[func]()->R
					{
						return func();
					}));
				}
			}

			return futures;
		}

		std::vector<std::future<R>> invokeAsyncTracked(std::false_type)
		{
			std::vector<std::future<R>> futures;

			for(auto& f : this->functions.vec)
			{
				futures.emplace_back(std::async(std::launch::async,
					[f]()->R
				{
					return f();
				}));
			}

			return futures;
		}
};

///
/// A non-thread safe Federate of non-tracked function objects.
/// For functions with the signature "void(void)"
///
template<> class Federate<void(void), false, false> : public FederateBase<std::function<void(void)>, false, false>
{
	public:
		/// To clean up the spelling throughout the function, define a type for our Federate functions.
		typedef std::function<void(void)> FederateFunction;

		///
		/// Invokes each of the functions in the Federate serially.
		///
		void invoke()
		{
			auto scopedLock = this->lock.acquire();
			SuppressWarningUnusedVariable(scopedLock);
			this->invokeTrackedFalse();
		}

		///
		/// Invokes each of the functions in the Federate asynchronously.  
		/// Returns a vector of futures for the functions.vec.
		///
		std::vector<std::future<void>> invokeAsync()
		{
			auto scopedLock = this->lock.acquire();
			SuppressWarningUnusedVariable(scopedLock);
			return this->invokeAsyncTrackedFalse();
		}

	protected:
		void invokeTrackedFalse()
		{
			for(auto& f : this->functions.vec)
			{
				f();
			}
		}

		std::vector<std::future<void>> invokeAsyncTrackedFalse()
		{
			std::vector<std::future<void>> futures;

			for(auto& f : this->functions.vec)
			{
				futures.emplace_back(std::async(std::launch::async,
					[f]()
				{
					f();
				}));
			}

			return futures;
		}
};

///
/// A non-thread safe Federate of non-tracked function objects.
/// For functions with the signature "void(void)"
///
template<> class Federate<void(void), false, true> : public FederateBase<std::function<void(void)>, false, true>
{
	public:
		/// To clean up the spelling throughout the function, define a type for our Federate functions.
		typedef std::function<void(void)> FederateFunction;

		///
		/// Invokes each of the functions in the Federate serially.
		///
		void invoke()
		{
			auto scopedLock = this->lock.acquire();
			SuppressWarningUnusedVariable(scopedLock);
			this->invokeTrackedFalse();
		}

		///
		/// Invokes each of the functions in the Federate asynchronously.  
		/// Returns a vector of futures for the functions.vec.
		///
		std::vector<std::future<void>> invokeAsync()
		{
			auto scopedLock = this->lock.acquire();
			SuppressWarningUnusedVariable(scopedLock);
			return this->invokeAsyncTrackedFalse();
		}

	protected:
		void invokeTrackedFalse()
		{
			for(auto& f : this->functions.vec)
			{
				f();
			}
		}

		std::vector<std::future<void>> invokeAsyncTrackedFalse()
		{
			std::vector<std::future<void>> futures;

			for(auto& f : this->functions.vec)
			{
				futures.emplace_back(std::async(std::launch::async,
					[f]()
				{
					f();
				}));
			}

			return futures;
		}
};

///
/// A non-thread safe Federate of non-tracked function objects.
/// For functions with the signature "void(void)"
///
template<> class Federate<void(void), true, false> : public FederateBase<std::function<void(void)>, true, false>
{
	public:
		/// To clean up the spelling throughout the function, define a type for our Federate functions.
		typedef std::function<void(void)> FederateFunction;

		///
		/// Invokes each of the functions in the Federate serially.
		///
		void invoke()
		{
			auto scopedLock = this->lock.acquire();
			SuppressWarningUnusedVariable(scopedLock);
			this->invokeTrackedTrue();
		}

		///
		/// Invokes each of the functions in the Federate asynchronously.  
		/// Returns a vector of futures for the functions.vec.
		///
		std::vector<std::future<void>> invokeAsync()
		{
			auto scopedLock = this->lock.acquire();
			SuppressWarningUnusedVariable(scopedLock);
			return this->invokeAsyncTrackedTrue();
		}

	protected:
		void invokeTrackedTrue()
		{
			for(auto& f : this->functions.vec)
			{
				auto func = f.lock();

				if(func != nullptr)
				{
					func.get()->operator()();
				}
			}
		}

		std::vector<std::future<void>> invokeAsyncTrackedTrue()
		{
			std::vector<std::future<void>> futures;

			for(auto& f : this->functions.vec)
			{
				auto func = f.lock();

				if(func != nullptr)
				{
					futures.emplace_back(std::async(std::launch::async,
						[func]()
					{
						func->operator()();
					}));
				}
			}

			return futures;
		}
};


///
/// A non-thread safe Federate of non-tracked function objects.
/// For functions with the signature "void(void)"
///
template<> class Federate<void(void), true, true> : public FederateBase<std::function<void(void)>, true, true>
{
	public:
		/// To clean up the spelling throughout the function, define a type for our Federate functions.
		typedef std::function<void(void)> FederateFunction;

		///
		/// Invokes each of the functions in the Federate serially.
		///
		void invoke()
		{
			auto scopedLock = this->lock.acquire();
			SuppressWarningUnusedVariable(scopedLock);
			this->invokeTrackedTrue();
		}

		///
		/// Invokes each of the functions in the Federate asynchronously.  
		/// Returns a vector of futures for the functions.vec.
		///
		std::vector<std::future<void>> invokeAsync()
		{
			auto scopedLock = this->lock.acquire();
			SuppressWarningUnusedVariable(scopedLock);
			return this->invokeAsyncTrackedTrue();
		}

	protected:
		void invokeTrackedTrue()
		{
			for(auto& f : this->functions.vec)
			{
				auto func = f.lock();

				if(func != nullptr)
				{
					func->operator()();
				}
			}
		}

		std::vector<std::future<void>> invokeAsyncTrackedTrue()
		{
			std::vector<std::future<void>> futures;

			for(auto& f : this->functions.vec)
			{
				auto func = f.lock();

				if(func != nullptr)
				{
					futures.emplace_back(std::async(std::launch::async,
						[func]()
					{
						func->operator()();
					}));
				}
			}

			return futures;
		}
};

#endif
