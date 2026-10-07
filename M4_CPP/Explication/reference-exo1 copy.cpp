#include <iostream>

int main()
{
	int		numberOfBalls = 42;

	int*	ballsPtr = &numberOfBalls;
	int&	ballsRef = numberOfBalls;

	// int&	ballsRef2; FAUX Obligatoire d'initialiser une reference

	std::cout << numberOfBalls << " " << *ballsPtr << " " << ballsRef << std::endl;

	*ballsPtr = 21;
	std::cout << numberOfBalls << std::endl;

	ballsRef = 84;
	std::cout << numberOfBalls << std::endl;

	numberOfBalls = 100;
	std::cout << ballsRef << std::endl;

	return(0);
}