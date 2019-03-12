// SearchFactory.cpp
// =================
//Ivaylo Ivanov
//
// Factory function implementation to create CSearchXXX objects 
//


/* TODO - include each implemented search class here */
#include "AStarAlgorithm.h"
#include "DijkstraAlgorithm.h"
#include "DepthFirstAlgorithm.h"
#include "SearchFactory.h" // Factory declarations

// Create new search object of the given type and return a pointer to it.
// Note the returned pointer type is the base class. This is how we implement polymorphism.
ISearch* NewSearch(ESearchType search)
{
  switch (search)
  {
    
	case AStar:
	 {
		return new CAStarSearch();
	 }

	case Dijkstra:
	{
		return new CDijkstraSearch();
	}
	case DepthFirst:
	{
		return new CDepthFirstSearch();
	}
  }
}
