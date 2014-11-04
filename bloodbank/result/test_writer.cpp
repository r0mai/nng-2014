#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>

int main(int argc , char** argv)
{
    srand(time(NULL));
    for(int i = 4; i <= 100; ++i)
	{
        std::cout << i << std::endl;
		char filename[14];
		sprintf(filename, "blood_%03d.txt", i);
		
	    std::ofstream f(filename);
        std::size_t c = rand() % 1000000UL;

        f << c << std::endl;
        for( std::size_t i = 0; i < c ; ++i )
            f << !(rand() % 12) << " ";
	}
	
}
