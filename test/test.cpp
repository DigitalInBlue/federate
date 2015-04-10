#include <Federate/Federate.h>
#include <gtest/gtest.h>

template<typename F> void CallCommonAPIFunctions(F&& f)
{
	f.size();
	f.clear();
	f.empty();
	f.garbageSize();
	f.clean();
}

TEST(Federate, BuildTemplates_IntInt)
{
	auto a = Federate<int(int), false, false>();
	auto b = Federate<int(int), false, true>();
	auto c = Federate<int(int), true, false>();
	auto d = Federate<int(int), true, true>();

	EXPECT_NO_THROW(CallCommonAPIFunctions(a));
	EXPECT_NO_THROW(CallCommonAPIFunctions(b));
	EXPECT_NO_THROW(CallCommonAPIFunctions(c));
	EXPECT_NO_THROW(CallCommonAPIFunctions(d));
}

TEST(Federate, BuildTemplates_IntVoid)
{
	auto a = Federate<int(void), false, false>();
	auto b = Federate<int(void), false, true>();
	auto c = Federate<int(void), true, false>();
	auto d = Federate<int(void), true, true>();

	EXPECT_NO_THROW(CallCommonAPIFunctions(a));
	EXPECT_NO_THROW(CallCommonAPIFunctions(b));
	EXPECT_NO_THROW(CallCommonAPIFunctions(c));
	EXPECT_NO_THROW(CallCommonAPIFunctions(d));
}

TEST(Federate, BuildTemplates_VoidInt)
{
	auto a = Federate<void(int), false, false>();
	auto b = Federate<void(int), false, true>();
	auto c = Federate<void(int), true, false>();
	auto d = Federate<void(int), true, true>();

	EXPECT_NO_THROW(CallCommonAPIFunctions(a));
	EXPECT_NO_THROW(CallCommonAPIFunctions(b));
	EXPECT_NO_THROW(CallCommonAPIFunctions(c));
	EXPECT_NO_THROW(CallCommonAPIFunctions(d));
}

TEST(Federate, BuildTemplates_VoidVoid)
{
	auto a = Federate<void(void), false, false>();
	auto b = Federate<void(void), false, true>();
	auto c = Federate<void(void), true, false>();
	auto d = Federate<void(void), true, true>();

	EXPECT_NO_THROW(CallCommonAPIFunctions(a));
	EXPECT_NO_THROW(CallCommonAPIFunctions(b));
	EXPECT_NO_THROW(CallCommonAPIFunctions(c));
	EXPECT_NO_THROW(CallCommonAPIFunctions(d));
}

TEST(Federate, IntInt)
{
	const bool tracked = false;
	auto fed = Federate<int(int), tracked>();

	auto multiplyBy2 = 
		[](int x)->int
		{
			return x * 2;
		};

	auto powerOf2 =
		[](int x)->int
		{
			return static_cast<int>(std::pow(2, x));
		};

	fed.push_back(multiplyBy2);
	fed.push_back(powerOf2);
	fed.push_back([](int x)->int
		{
			return static_cast<int>(std::pow(x, 2));
		});

	auto answers = fed.invoke(8);

	EXPECT_EQ(3, fed.size());
	ASSERT_EQ(3, answers.size());
	
	EXPECT_EQ(16, answers[0]);
	EXPECT_EQ(256, answers[1]);
	EXPECT_EQ(64, answers[2]);

	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_FALSE(fed.empty());

	EXPECT_NO_THROW(fed.clean());
	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_FALSE(fed.empty());
	EXPECT_EQ(3, fed.size());

	EXPECT_NO_THROW(fed.clear());
	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_TRUE(fed.empty());
	EXPECT_EQ(0, fed.size());
}

TEST(Federate, VoidInt)
{
	const bool tracked = false;
	auto fed = Federate<void(int), tracked>();

	int y = 0;

	auto multiplyBy2 =
		[&y](int x)
	{
		y = x * 2;
	};

	auto powerOf2 =
		[&y](int x)
	{
		y = static_cast<int>(std::pow(2, x));
	};

	fed.push_back(multiplyBy2);
	fed.push_back(powerOf2);
	fed.push_back([&y](int x)
	{
		y = static_cast<int>(std::pow(x, 2));
	});

	EXPECT_NO_THROW(fed.invoke(8));
	EXPECT_EQ(3, fed.size());

	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_FALSE(fed.empty());

	EXPECT_NO_THROW(fed.clean());
	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_FALSE(fed.empty());
	EXPECT_EQ(3, fed.size());

	EXPECT_NO_THROW(fed.clear());
	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_TRUE(fed.empty());
	EXPECT_EQ(0, fed.size());
}

TEST(Federate, IntVoid)
{
	const bool tracked = false;
	auto fed = Federate<int(void), tracked>();

	int x = 2;

	auto multiplyBy2 =
		[&x]()
	{
		return x * 2;
	};

	auto powerOf2 =
		[&x]()->int
	{
		return static_cast<int>(std::pow(2, x));
	};

	fed.push_back(multiplyBy2);
	fed.push_back(powerOf2);
	fed.push_back([&x]()->int
	{
		return static_cast<int>(std::pow(x, 2));
	});

	auto answers = fed.invoke();

	EXPECT_EQ(3, fed.size());
	ASSERT_EQ(3, answers.size());

	EXPECT_EQ(4, answers[0]);
	EXPECT_EQ(4, answers[1]);
	EXPECT_EQ(4, answers[2]);

	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_FALSE(fed.empty());

	EXPECT_NO_THROW(fed.clean());
	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_FALSE(fed.empty());
	EXPECT_EQ(3, fed.size());

	EXPECT_NO_THROW(fed.clear());
	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_TRUE(fed.empty());
	EXPECT_EQ(0, fed.size());
}

TEST(Federate, VoidVoid)
{
	const bool tracked = false;
	auto fed = Federate<void(void), tracked>();

	int x = 2;

	auto multiplyBy2 =
		[&x]()
	{
		x = x * 2;
	};

	auto powerOf2 =
		[&x]()
	{
		x = static_cast<int>(std::pow(2, x));
	};

	fed.push_back(multiplyBy2);
	fed.push_back(powerOf2);
	fed.push_back([&x]()
	{
		x = static_cast<int>(std::pow(x, 2));
	});

	fed.invoke();

	EXPECT_EQ(3, fed.size());

	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_FALSE(fed.empty());

	EXPECT_NO_THROW(fed.clean());
	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_FALSE(fed.empty());
	EXPECT_EQ(3, fed.size());

	EXPECT_NO_THROW(fed.clear());
	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_TRUE(fed.empty());
	EXPECT_EQ(0, fed.size());
}

TEST(Federate, IntInt_Tracked)
{
	const bool tracked = true;
	auto fed = Federate<int(int), tracked>();
	std::vector<Federate<int(int), tracked>::Tracker> tracker;

	auto multiplyBy2 =
		[](int x)
	{
		return x * 2;
	};

	auto powerOf2 =
		[](int x)->int
	{
		return static_cast<int>(std::pow(2, x));
	};

	tracker.push_back(fed.push_back(multiplyBy2));
	tracker.push_back(fed.push_back(powerOf2));
	tracker.push_back(fed.push_back([](int x)->int
	{
		return static_cast<int>(std::pow(x, 2));
	}));

	auto answers = fed.invoke(8);

	EXPECT_EQ(3, fed.size());
	ASSERT_EQ(3, answers.size());

	EXPECT_EQ(16, answers[0]);
	EXPECT_EQ(256, answers[1]);
	EXPECT_EQ(64, answers[2]);

	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_FALSE(fed.empty());

	EXPECT_NO_THROW(fed.clean());
	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_FALSE(fed.empty());
	EXPECT_EQ(3, fed.size());

	EXPECT_NO_THROW(fed.clear());
	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_TRUE(fed.empty());
	EXPECT_EQ(0, fed.size());
}

TEST(Federate, VoidInt_Tracked)
{
	const bool tracked = true;
	auto fed = Federate<void(int), tracked>();
	std::vector<Federate<void(int), tracked>::Tracker> tracker;
	
	int y = 0;

	auto multiplyBy2 =
		[&y](int x)
		{
			y = x * 2;
		};

	auto powerOf2 =
		[&y](int x)
		{
			y = static_cast<int>(std::pow(2, x));
		};

	tracker.emplace_back(fed.push_back(multiplyBy2));
	tracker.emplace_back(fed.push_back(powerOf2));
	tracker.emplace_back(fed.push_back([&y](int x)
		{
			y = static_cast<int>(std::pow(x, 2));
		}));

	EXPECT_NO_THROW(fed.invoke(8));
	EXPECT_EQ(3, fed.size());

	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_FALSE(fed.empty());

	EXPECT_NO_THROW(fed.clean());
	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_FALSE(fed.empty());
	EXPECT_EQ(3, fed.size());

	EXPECT_NO_THROW(fed.clear());
	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_TRUE(fed.empty());
	EXPECT_EQ(0, fed.size());
}

TEST(Federate, IntVoid_Tracked)
{
	const bool tracked = true;
	auto fed = Federate<int(void), tracked>();
	std::vector<Federate<int(void), tracked>::Tracker> tracker;

	int x = 2;

	auto multiplyBy2 =
		[&x]()
	{
		return x * 2;
	};

	auto powerOf2 =
		[&x]()->int
	{
		return static_cast<int>(std::pow(2, x));
	};

	tracker.emplace_back(fed.push_back(multiplyBy2));
	tracker.emplace_back(fed.push_back(powerOf2));
	tracker.emplace_back(fed.push_back([&x]()->int
	{
		return static_cast<int>(std::pow(x, 2));
	}));

	auto answers = fed.invoke();

	EXPECT_EQ(3, fed.size());
	ASSERT_EQ(3, answers.size());

	EXPECT_EQ(4, answers[0]);
	EXPECT_EQ(4, answers[1]);
	EXPECT_EQ(4, answers[2]);

	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_FALSE(fed.empty());

	EXPECT_NO_THROW(fed.clean());
	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_FALSE(fed.empty());
	EXPECT_EQ(3, fed.size());

	EXPECT_NO_THROW(fed.clear());
	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_TRUE(fed.empty());
	EXPECT_EQ(0, fed.size());
}

TEST(Federate, VoidVoid_Tracked)
{
	const bool tracked = true;
	auto fed = Federate<void(void), tracked>();
	std::vector<Federate<void(void), tracked>::Tracker> tracker;

	int x = 2;

	auto multiplyBy2 =
		[&x]()
	{
		x = x * 2;
	};

	auto powerOf2 =
		[&x]()
	{
		x = static_cast<int>(std::pow(2, x));
	};

	tracker.emplace_back(fed.push_back(multiplyBy2));
	tracker.emplace_back(fed.push_back(powerOf2));
	tracker.emplace_back(fed.push_back([&x]()
	{
		x = static_cast<int>(std::pow(x, 2));
	}));

	fed.invoke();

	EXPECT_EQ(3, fed.size());

	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_FALSE(fed.empty());

	EXPECT_NO_THROW(fed.clean());
	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_FALSE(fed.empty());
	EXPECT_EQ(3, fed.size());

	EXPECT_NO_THROW(fed.clear());
	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_TRUE(fed.empty());
	EXPECT_EQ(0, fed.size());
}

TEST(Federate, IntInt_Tracked_ThreadSafe)
{
	const bool tracked = true;
	const bool threadSafe = true;
	auto fed = Federate<int(int), tracked, threadSafe>();
	std::vector<Federate<int(int), tracked, threadSafe>::Tracker> tracker;

	auto multiplyBy2 =
		[](int x)
	{
		return x * 2;
	};

	auto powerOf2 =
		[](int x)->int
	{
		return static_cast<int>(std::pow(2, x));
	};

	tracker.push_back(fed.push_back(multiplyBy2));
	tracker.push_back(fed.push_back(powerOf2));
	tracker.push_back(fed.push_back([](int x)->int
	{
		return static_cast<int>(std::pow(x, 2));
	}));

	auto answers = fed.invoke(8);

	EXPECT_EQ(3, fed.size());
	ASSERT_EQ(3, answers.size());

	EXPECT_EQ(16, answers[0]);
	EXPECT_EQ(256, answers[1]);
	EXPECT_EQ(64, answers[2]);

	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_FALSE(fed.empty());

	EXPECT_NO_THROW(fed.clean());
	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_FALSE(fed.empty());
	EXPECT_EQ(3, fed.size());

	EXPECT_NO_THROW(fed.clear());
	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_TRUE(fed.empty());
	EXPECT_EQ(0, fed.size());
}

TEST(Federate, VoidInt_Tracked_ThreadSafe)
{
	const bool tracked = true;
	const bool threadSafe = true; 
	auto fed = Federate<void(int), tracked, threadSafe>();
	std::vector<Federate<void(int), tracked, threadSafe>::Tracker> tracker;

	int y = 0;

	auto multiplyBy2 =
		[&y](int x)
	{
		y = x * 2;
	};

	auto powerOf2 =
		[&y](int x)
	{
		y = static_cast<int>(std::pow(2, x));
	};

	tracker.emplace_back(fed.push_back(multiplyBy2));
	tracker.emplace_back(fed.push_back(powerOf2));
	tracker.emplace_back(fed.push_back([&y](int x)
	{
		y = static_cast<int>(std::pow(x, 2));
	}));

	EXPECT_NO_THROW(fed.invoke(8));
	EXPECT_EQ(3, fed.size());

	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_FALSE(fed.empty());

	EXPECT_NO_THROW(fed.clean());
	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_FALSE(fed.empty());
	EXPECT_EQ(3, fed.size());

	EXPECT_NO_THROW(fed.clear());
	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_TRUE(fed.empty());
	EXPECT_EQ(0, fed.size());
}

TEST(Federate, IntVoid_Tracked_ThreadSafe)
{
	const bool tracked = true;
	const bool threadSafe = true;
	auto fed = Federate<int(void), tracked, threadSafe>();
	std::vector<Federate<int(void), tracked, threadSafe>::Tracker> tracker;

	int x = 2;

	auto multiplyBy2 =
		[&x]()
	{
		return x * 2;
	};

	auto powerOf2 =
		[&x]()->int
	{
		return static_cast<int>(std::pow(2, x));
	};

	tracker.emplace_back(fed.push_back(multiplyBy2));
	tracker.emplace_back(fed.push_back(powerOf2));
	tracker.emplace_back(fed.push_back([&x]()->int
	{
		return static_cast<int>(std::pow(x, 2));
	}));

	auto answers = fed.invoke();

	EXPECT_EQ(3, fed.size());
	ASSERT_EQ(3, answers.size());

	EXPECT_EQ(4, answers[0]);
	EXPECT_EQ(4, answers[1]);
	EXPECT_EQ(4, answers[2]);

	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_FALSE(fed.empty());

	EXPECT_NO_THROW(fed.clean());
	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_FALSE(fed.empty());
	EXPECT_EQ(3, fed.size());

	EXPECT_NO_THROW(fed.clear());
	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_TRUE(fed.empty());
	EXPECT_EQ(0, fed.size());
}

TEST(Federate, VoidVoid_Tracked_ThreadSafe)
{
	const bool tracked = true;
	const bool threadSafe = true;
	auto fed = Federate<void(void), tracked, threadSafe>();
	std::vector<Federate<void(void), tracked, threadSafe>::Tracker> tracker;

	int x = 2;

	auto multiplyBy2 =
		[&x]()
	{
		x = x * 2;
	};

	auto powerOf2 =
		[&x]()
	{
		x = static_cast<int>(std::pow(2, x));
	};

	tracker.emplace_back(fed.push_back(multiplyBy2));
	tracker.emplace_back(fed.push_back(powerOf2));
	tracker.emplace_back(fed.push_back([&x]()
	{
		x = static_cast<int>(std::pow(x, 2));
	}));

	fed.invoke();

	EXPECT_EQ(3, fed.size());

	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_FALSE(fed.empty());

	EXPECT_NO_THROW(fed.clean());
	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_FALSE(fed.empty());
	EXPECT_EQ(3, fed.size());

	EXPECT_NO_THROW(fed.clear());
	EXPECT_EQ(0, fed.garbageSize());
	EXPECT_TRUE(fed.empty());
	EXPECT_EQ(0, fed.size());
}

TEST(Federate, BasicDemo)
{
	// You can fully define the federate.  
	// First is the function signature, 
	// then if you want to enable tracking, 
	// then if you want to enable thread safety.
	const bool EnableTracking = false;
	const bool EnableThreadSafety = false;

	// Both Tracking and Thread Safety default to "false", so they can be omittied.
	// i.e. "auto collectResults = Federate<float(float)>();"
	auto collectResults = Federate<float(float), EnableTracking, EnableThreadSafety>();

	// Now we can add functions into the federate.
	auto degreesToRadians = [](float degrees)
	{
		return degrees * 0.0174532925f;
	};

	collectResults.push_back(degreesToRadians);

	auto fahrenheitToCelsius = [](float degrees)
	{
		return (degrees - 32.0f) * (5.0f / 9.0f);
	};

	collectResults.push_back(fahrenheitToCelsius);

	// Now we have two functions inside our federate.
	// Call both of them and get the results.
	auto results = collectResults.invoke(90.0f);

	std::cerr << "90 degrees to radians: " << results[0] << std::endl;
	std::cerr << "90 degrees F to C: " << results[1] << std::endl;
}

TEST(Federate, TrackedDemo)
{
	// You can fully define the federate.  
	// First is the function signature, 
	// then if you want to enable tracking, 
	// then if you want to enable thread safety.
	const bool EnableTracking = true;
	const bool EnableThreadSafety = false;

	// Both Tracking and Thread Safety default to "false", so they can be omittied.
	// i.e. "auto collectResults = Federate<float(float), EnableTracking>();"
	auto collectResults = Federate<float(float), EnableTracking, EnableThreadSafety>();

	// Create something to manage the tracking.
	// Tracking is managed via scope.
	std::vector<Federate<float(float), EnableTracking>::Tracker> tracker;

	// Now we can add functions into the federate.
	auto degreesToRadians = [](float degrees)
	{
		return degrees * 0.0174532925f;
	};

	// push and save the scope of the returned tracking object.
	tracker.emplace_back(
		collectResults.push_back(degreesToRadians));

	auto fahrenheitToCelsius = [](float degrees)
	{
		return (degrees - 32.0f) * (5.0f / 9.0f);
	};

	// Here, we "forget" to keep the tracking object returned by "push_back".
	// As a result, this will not ever get called and will be considered "garbage"
	// as it has expired immediately.  This is not a useful thing to do.
	collectResults.push_back(fahrenheitToCelsius);

	// push and save the scope of the returned tracking object.
	tracker.emplace_back(
		collectResults.push_back(fahrenheitToCelsius));

	// Set up a narrower scope.
	{
		// Now, let's add a third function with a narrower scope.
		auto degreeProofToAlchoholByVolume = [](float degrees)
		{
			return degrees / (7.0f / 4.0f);
		};

		auto locallyTracked = collectResults.push_back(degreeProofToAlchoholByVolume);

		// Now we have two functions inside our federate.
		// Call both of them and get the results.
		auto results = collectResults.invoke(90.0f);

		// 4 functions in the federate, but 1 is garbage from our "mistake" earlier.
		std::cerr << "Federation Size: " << collectResults.size() << ", Garbage Size: " << collectResults.garbageSize() << std::endl;
		std::cerr << "Result size: " << results.size() << std::endl;
		std::cerr << "    90 degrees to radians: " << results[0] << std::endl;
		std::cerr << "    90 degrees F to C: " << results[1] << std::endl;
		std::cerr << "    90 degrees proof to ABV: " << results[2] << std::endl << std::endl;
	}

	// "locallyTracked" is now out of scope.
	// "degreeProofToAlchoholByVolume" is now expired / garbage.  
	auto results = collectResults.invoke(180.0f);
	std::cerr << "Federation Size: " << collectResults.size() << ", Garbage Size: " << collectResults.garbageSize() << std::endl;
	std::cerr << "Result size: " << results.size() << std::endl;
	std::cerr << "    90 degrees to radians: " << results[0] << std::endl;
	std::cerr << "    90 degrees F to C: " << results[1] << std::endl;
}

TEST(Federate, SlotDemo)
{
	class Foo
	{
		public:
		Federate<void(const std::string&), true>::Tracker registerFunction(std::function<void(const std::string&)> f)
		{
			return slot.push_back(f);
		}

		void callFunctions(const std::string& x)
		{
			this->slot.invoke(x);
		}

		void callFunctionsAsync(const std::string& x)
		{
			this->slot.invokeAsync(x);
		}

		private:
		Federate<void(const std::string&), true> slot;
	};

	Foo foo;

	auto keepAlive1 = foo.registerFunction([](const std::string& x)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		std::cerr << " hello " << x << " ";
	});

	// Change Scope.
	{
		auto keepAlive2 = foo.registerFunction([](const std::string& x)
		{

			std::cerr << " world " << x << " ";
		});

		foo.callFunctions("!!!");
		std::cerr << std::endl;
		foo.callFunctionsAsync("***");
		std::cerr << std::endl;
	}

	foo.callFunctions("!!!");
	std::cerr << std::endl;
	foo.callFunctionsAsync("***");
}
