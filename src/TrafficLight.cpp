#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    
  std::unique_lock<std::mutex> lock(_mutex);
  _condition.wait(lock, [this]{return !_queue.empty();});
  T message = std::move(_queue.back());
  _queue.pop_back();
  return message;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    
	std::lock_guard<std::mutex> lock(_mutex);
  	_queue.emplace_back(msg);
  	_condition.notify_one();
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    
  while (true){
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  
  auto message = _messageQueue.receive();
  if (message == TrafficLightPhase::green)
  {
  	return;
  }
  }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    
  threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
     
  std::random_device rd;
  //Mersenne twister
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> distrib(4000, 6000);
  int cycle_duration = distrib(gen);
  auto  last_update = std::chrono:: system_clock::now();
   
while (true)
	{
			  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  long delta = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - last_update).count();
if (delta >= cycle_duration)
{
	if (_currentPhase == red)
      _currentPhase = green;
  	else if (_currentPhase == green)
      _currentPhase = red;
  	_messageQueue.send(std::move(TrafficLightPhase(_currentPhase)));
  last_update = std::chrono::system_clock::now();
  cycle_duration = distrib(gen);
  
}  
  
	} 
  
}

