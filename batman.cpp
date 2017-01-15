#define _USE_MATH_DEFINES
#include "SFML/Graphics.hpp"
#include <iostream>
#include <cstdint>
#include <cmath>
#include <random>
#include <iomanip>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>

#include "ThreadSafeQueue.hpp"
#include "CommonStructs.h"

const double HASH_1 = (6 * sqrt(10)) / 7;
const double HASH_2 = HASH_1 / 2;
const double HASH_3 = ((3 * sqrt(33)) - 7) / 112;

bool initGraphics(std::vector<sf::Text>& texts);

void pointGenerator(const uint32_t& windowWidth, const uint32_t& windowHeight);

void monteCarlo(const MonteCarloArgs& args, uint32_t& totalPoints, uint32_t& pointsIn);

bool isBatman(const Point& point, const Point& origin, const double& scale);

bool inOval(const Point& point, const Point& origin, const OvalRadius& ovalRadius);

double ovalArea(const OvalRadius& ovalRadius);

double calculateError(double allPoints, double pointsIn, const MonteCarloArgs& args);

void finalDraw(sf::RenderWindow& mainWindow, std::vector<sf::Text>& texts);

void waitForExit(sf::RenderWindow& mainWindow);

/* std::vector is used for Image container, because of the
   contiguous placement of his elements in  the memory ->
   there will be a lot of cache hits when drawing */
std::vector<sf::CircleShape> gDrawContainer;
ThreadSafeQueue<Point> gSupplyContainer;
std::mutex gDrawMutex;
std::mutex gPointMutex;
std::mutex gIOMutex;
uint32_t MAX_POINTS = 0;

int main()
{
	std::cin >> MAX_POINTS;
	using Time = std::chrono::high_resolution_clock;
	std::chrono::high_resolution_clock::time_point start = Time::now();
	std::chrono::high_resolution_clock::time_point end;
	
	const uint32_t WINDOW_WIDTH = 1000;
	const uint32_t WINDOW_HEIGHT = 600;
	sf::RenderWindow mainWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Batman");

	const double X_RADIUS = WINDOW_WIDTH / 2;
	const double Y_RADIUS = X_RADIUS / 2;
	uint32_t totalPoints = 0;
	uint32_t pointsIn = 0;

	MonteCarloArgs args;
	args.animationCenter = Point(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
	args.animationScale = 60.0;
	args.ovalRadius = OvalRadius(X_RADIUS, Y_RADIUS);

	std::vector<sf::Text> texts;
	if (initGraphics(texts))
	{
		std::cerr << "Error. Could not load graphics. Terminating..." << std::endl;
		return EXIT_FAILURE;
	}

	std::thread generatorThread(pointGenerator, std::ref(WINDOW_WIDTH), std::ref(WINDOW_HEIGHT));
	generatorThread.detach();

	gDrawContainer.reserve(MAX_POINTS);

	/* hardware_concurrency may return 0 if its not supported */
	const uint32_t THREAD_NUM = std::thread::hardware_concurrency();
	std::vector<std::thread> monteCarloThreadPool;

	volatile bool isMultiThreading = false;
	/* if threads are <= 2 no need to run Monte Carlo on 
	   different thread, because there will be a performance
	   lost from the constant threads context switches */
	if (THREAD_NUM > 2)
	{
		isMultiThreading = true;

		/* -2 because:
		*  1 thread for random point generation,
		*  1 thread(main) for drawing */
		const uint32_t MONTE_CARLO_THREAD_NUM = THREAD_NUM - 2;
		monteCarloThreadPool.resize(MONTE_CARLO_THREAD_NUM);

		for (uint32_t i = 0; i < MONTE_CARLO_THREAD_NUM; ++i)
		{
			monteCarloThreadPool[i] = std::thread(monteCarlo,
				std::ref(args), std::ref(totalPoints), std::ref(pointsIn));
			monteCarloThreadPool[i].detach();
		}
	}

	volatile bool isAlgoFinished = false;
	volatile bool isWindowOpen = mainWindow.isOpen();

	/* clear the window with black color */
	mainWindow.clear(sf::Color::Black);

	sf::Event event;
	while (isWindowOpen)
	{
		if (mainWindow.pollEvent(event) != 0)
		{
			if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape)
			{
				/* reset MAX_POINTS to 0 so all threads can 
				   release their mutex locks (if any) */
				MAX_POINTS = 0;

				/* give time to other threads to release their locks */
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
				isWindowOpen = false;
			}
		}

		/* if the algorithm has finished -> sleep for some time
		   to let other threads release their locks (if any) and
		   stop calculations and drawing */
		if (isAlgoFinished)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			break;
		}

		/* if you do not have enought hardware resources for 
		   threads -> none shall be used for Monte Carlo */
		if (!isMultiThreading)
		{
			monteCarlo(args, totalPoints, pointsIn);
		}

		gPointMutex.lock();
		/* maximum density point density reached -> stop the calculations */
		if (totalPoints > MAX_POINTS)
		{
			gPointMutex.unlock();
			isAlgoFinished = true;
			continue;
		}
		gPointMutex.unlock();

		mainWindow.clear(sf::Color::Black);

		gDrawMutex.lock();
		const size_t CIRCLES_SIZE = gDrawContainer.size();

		for (size_t i = 0; i < CIRCLES_SIZE; ++i)
		{
			/* since drawing is taking enourmous amounts of time
			   compared to the actual Monte Carlo calculations -> 
			   release the mutex lock every 10 cycles so 
			   calculating threads can take the lock */
			if (i % 10 == 0)
			{
				gDrawMutex.unlock();
				gDrawMutex.lock();
			}
			mainWindow.draw(gDrawContainer[i]);
		}
		gDrawMutex.unlock();

		end = Time::now();
		texts[TIME].setString("Time spent: " + std::to_string(
			std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()) + " ms");
		texts[ALL_POINTS].setString("Points: " + std::to_string(totalPoints));
		texts[ERROR].setString("Error: " + std::to_string(calculateError(
															totalPoints, pointsIn, args)) + "%");
		for (int32_t i = 0; i < TEXTS_COUNT; ++i)
		{
			mainWindow.draw(texts[i]);
		}

		mainWindow.display();
	}

	if (isWindowOpen)
	{
		texts[TIME].setString("Time spent: " + std::to_string(
			std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()) + " ms");
		texts[ALL_POINTS].setString("Points: " + std::to_string(totalPoints));
		texts[ERROR].setString("Error: " + std::to_string(calculateError(
			totalPoints, pointsIn, args)) + "%");
		finalDraw(mainWindow, texts);
		/* release resources and stop drawing */
		gDrawContainer.~vector();
		gSupplyContainer.~ThreadSafeQueue();

		waitForExit(mainWindow);
	}
	else
	{
		mainWindow.close();
	}
	
	return EXIT_SUCCESS;
}

bool initGraphics(std::vector<sf::Text>& texts)
{
	static sf::Font font;
	if (!font.loadFromFile("orbitron-light.otf"))
	{
		std::cerr << "Error. Could not load font!" << std::endl;
		return EXIT_FAILURE;
	}

	texts.resize(TEXTS_COUNT);
	for (int32_t i = 0; i < TEXTS_COUNT; ++i)
	{
		texts[i].setFont(font);
		texts[i].setCharacterSize(24);
		texts[i].setColor(sf::Color::Green);
		texts[i].setStyle(sf::Text::Bold);
	}

	texts[TIME].setPosition(20, 10);
	texts[ALL_POINTS].setPosition(750, 10);
	texts[ERROR].setPosition(20, 560);

	return EXIT_SUCCESS;
}

bool inOval(const Point& point, const Point& origin, const OvalRadius& ovalRadius)
{
	const double posX = point.x - origin.x;
	const double posY = point.y - origin.y;

	return (posX / ovalRadius.x) * (posX / ovalRadius.x) +
		(posY / ovalRadius.y) * (posY / ovalRadius.y) <= 1 ? true : false;
}

double ovalArea(const OvalRadius& ovalRadius)
{
	return M_PI * ovalRadius.x * ovalRadius.y;
}

double calculateError(double allPoints, double pointsIn, const MonteCarloArgs& args)
{
	static const double REAL_AREA = 48.4243597 * args.animationScale * args.animationScale;
	static const double OVAL_AREA = ovalArea(args.ovalRadius);

	const double AREA_DIFF = abs(((pointsIn / allPoints) * OVAL_AREA) - REAL_AREA);
	/* *100 to get the error in percents */
	return ((AREA_DIFF / REAL_AREA) * 100);
}

bool isBatman(const Point& point, const Point& origin, const double& scale)
{
	const double POS_X = (point.x - origin.x) / scale;
	const double POS_Y = (point.y - origin.y) / scale;
	double tempX = 0.0;
	double tempY = 0.0;

	if (POS_Y < 0)
	{
		/* left upper wing */
		if (POS_X <= -3)
		{
			tempX = (-7 * sqrt(1 - ((POS_Y * POS_Y) / 9.0)));
			return POS_X >= tempX ? true : false;
		}

		/* left shoulder */
		if (POS_X > -3 && POS_X <= -1)
		{
			tempX = -POS_X;
			const double LOC_HASH = abs(tempX) - 1;
			tempY = -(HASH_1 + (1.5 - 0.5 * tempX)) +
				HASH_2 * sqrt(4.0 - (LOC_HASH * LOC_HASH));
			return POS_Y > tempY ? true : false;
		}

		/* exterior left ear */
		if (POS_X > -1 && POS_X <= -0.75)
		{
			tempY = 9.0 + 8 * POS_X;
			return POS_Y > -tempY ? true : false;
		}

		/* interior left ear */
		if (POS_X > -0.75 && POS_X <= -0.5)
		{
			tempY = -3 * POS_X + 0.75;
			return POS_Y > -tempY ? true : false;
		}

		/* top of head */
		if (POS_X > -.5 && POS_X <= 0.5)
		{
			tempY = 2.25;
			return POS_Y > -tempY ? true : false;
		}

		/* interior right ear */
		if (POS_X > 0.5 && POS_X <= 0.75)
		{
			tempY = 3 * POS_X + 0.75;
			return POS_Y > -tempY ? true : false;
		}

		/* exterior right ear */
		if (POS_X > 0.75 && POS_X <= 1)
		{
			tempY = 9.0 - 8 * POS_X;
			return POS_Y > -tempY ? true : false;
		}


		/* right shoulder */
		if (POS_X <= 3 && POS_X > 1){
			const double LOC_HASH = abs(POS_X) - 1;
			tempY = -(HASH_1 + (1.5 - 0.5 * POS_X)) +
				HASH_2 * sqrt(4.0 - (LOC_HASH * LOC_HASH));
			return POS_Y > tempY ? true : false;
		}

		/* right upper wing */
		if (POS_X > 3)
		{
			tempX = (7 * sqrt(1 - ((POS_Y * POS_Y) / 9.0)));
			return POS_X <= tempX ? true : false;
		}
	}
	if (POS_Y >= 0)
	{
		/* bottom left wing */
		if (POS_X <= -4.0)
		{
			tempX = (-7 * sqrt(1 - ((POS_Y * POS_Y) / 9.0)));
			return POS_X >= tempX ? true : false;
		}

		/* bottom wing */
		if (POS_X > -4.0 && POS_X <= 4)
		{
			const double LOC_HASH = abs(abs(POS_X) - 2) - 1;
			tempY = (abs(POS_X / 2) - (HASH_3 * POS_X * POS_X) - 3.0) +
				sqrt(1 - (LOC_HASH * LOC_HASH));
			tempY *= -1;
			return POS_Y < tempY ? true : false;
		}

		/* bottom right wing */
		if (POS_X >= 4.0)
		{
			tempX = (7 * sqrt(1 - ((POS_Y * POS_Y) / 9.0)));
			return POS_X <= tempX ? true : false;
		}
	}

	return false;
}

void pointGenerator(const uint32_t& windowWidth, const uint32_t& windowHeight)
{
	Point currPoint;
	uint32_t totalPointsGenerated = 0;

	std::random_device rd; /* seed for the pseudo random engine */
	std::mt19937 range(rd()); /* mersenne_twister engine */
	std::uniform_real_distribution<> distr(0, 1.0); /* we NEED uniform distribution for Monte Carlo */

	/* MAX_POINTS * 2, because not all points will be the oval,
	   therefore not all will be used for the Monte Carlo ->
	   generate twice as much */
	while (totalPointsGenerated < MAX_POINTS * 2)
	{
		++totalPointsGenerated;
		currPoint.x = distr(range) * windowWidth;
		currPoint.y = distr(range) * windowHeight;
		gSupplyContainer.push(currPoint);
	}

	gIOMutex.lock();
	std::cout << "Exiting from pointGenerator with threadID: " << std::this_thread::get_id() << std::endl;
	gIOMutex.unlock();

	return;
}

void monteCarlo(const MonteCarloArgs& args, uint32_t& totalPoints, uint32_t& pointsIn)
{
	Point currPoint;
	sf::CircleShape currCircle(2);
	currCircle.setFillColor(sf::Color::Yellow);

	while (true)
	{
		if (gSupplyContainer.empty())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			return;
		}
		else
		{
			if (!gSupplyContainer.try_pop(currPoint))
			{
				continue;
			}
		}

		if (inOval(currPoint, args.animationCenter, args.ovalRadius))
		{
			if (isBatman(currPoint, args.animationCenter, args.animationScale))
			{
				gPointMutex.lock();
				++pointsIn;
				gPointMutex.unlock();
			}
			else
			{
				/* we are not going to lose any significant
				bits from double to float conversations */
				currCircle.setPosition(static_cast<float>(currPoint.x),
					static_cast<float>(currPoint.y));

				gDrawMutex.lock();

				gDrawContainer.emplace_back(currCircle);

				gDrawMutex.unlock();
			}

			gPointMutex.lock();
			/* maximum density point density reached -> stop the calculations */
			if (++totalPoints > MAX_POINTS)
			{
				gPointMutex.unlock();

				gIOMutex.lock();
				std::cout << "Exiting from MonteCarlo with threadID: " << std::this_thread::get_id() << std::endl;
				gIOMutex.unlock();
				return;
			}
			gPointMutex.unlock();
		}
	}
}

void waitForExit(sf::RenderWindow& mainWindow)
{
	sf::Event event;
	while (mainWindow.isOpen())
	{
		if (mainWindow.pollEvent(event) != 0)
		{
			if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape)
			{
				mainWindow.close();
				return;
			}
		}
		else
		{
			/* if not input -> sleep for some time. No need to take all the CPU time */
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
}

void finalDraw(sf::RenderWindow& mainWindow, std::vector<sf::Text>& texts)
{
	mainWindow.clear(sf::Color::Black);

	const size_t CIRCLES_SIZE = gDrawContainer.size();
	for (size_t i = 0; i < CIRCLES_SIZE; ++i)
	{
		mainWindow.draw(gDrawContainer[i]);
	}

	for (int32_t i = 0; i < TEXTS_COUNT; ++i)
	{
		mainWindow.draw(texts[i]);
	}

	mainWindow.display();
}
