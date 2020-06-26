#include <SFML/Graphics.hpp>
#include "Source.h"
#include "Tetris.h"
using namespace std;
using namespace sf;

int main(void)
{
	Tetris* t = new Tetris();
	t->Game();
	delete t;

	return 0;
}