//============================================================================
// Name        : Portfolio_Milestone.cpp
// Author      : Nolan Hill
// Version     :
// Copyright   : 
// Description : Program that uses one thread to counts up to 20, then triggers a second thread that counts back down to 0.
//============================================================================

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

std::condition_variable cv;
std::mutex mtx;
int shared_integer = 0;
bool thread1_completed = false;

/*
 * Method used by thread1 to count up to 'max'
 */
void countUp(int max) {
    std::cout << "Thread 1:" << std::endl;

    // Loop through to print value of shared_integer and increment it
    for (int i = 0; i <= max; ++i) {
    	// Lock the mutex to prevent race
    	std::lock_guard<std::mutex> lock(mtx);

    	// Output shared_integer (the current count value)
        std::cout << shared_integer << std::endl;

        // Ensure last iteration does not increment shared_integer
        if (i != max) {shared_integer++;}

        // If last iteration, update thread1_completed to true
        else {thread1_completed = true;}
    }

    // Notify thread2 that thread1 is finished
    cv.notify_one();
}

/*
 * Method used by thread2 to count down from 'shared_integer' to 'min'
 */
void countDown(int min) {
	// Create unique_lock
    std::unique_lock<std::mutex> lock(mtx);

    // Ensure that thread2 waits for thread1 to complete
    cv.wait(lock, [] { return thread1_completed; });

    // Unlock the mutex
    lock.unlock();

    std::cout << "Thread 2:" << std::endl;

    // Loop through to print value of shared_integer and decrement it
    for (int i = shared_integer; i >= min; --i) {
    	// Lock the mutex
    	std::lock_guard<std::mutex> guard(mtx);
        std::cout << shared_integer << std::endl;
        shared_integer--;
    }
}

/*
 * Main program to have one thread count up to 20 and then have a second thread
 * wait for first thread to finish and then count down to 0
 */
int main() {
	// Create first thread to count up to 20
    std::thread thread1(countUp, 20);

    // Create second thread to count down to 0
    std::thread thread2(countDown, 0);

    // Ensure thread1 finishes before proceeding
    thread1.join();

    // Ensure thread2 finishes before proceeding
    thread2.join();

    return 0;
}
