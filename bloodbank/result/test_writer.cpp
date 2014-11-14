#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <cmath>

int main(int argc , char** argv)
{
    srand(time(NULL));
    for(int i = 4; i <= 100; ++i)
    {
        char filename[14];
        sprintf(filename, "blood_%03d.txt", i);
        
        std::ofstream f(filename);
        std::size_t c; 
        do
        {
            c = 100000;//rand() % static_cast<std::size_t>(std::pow(10. , i % 5 + 2));
        }
        while( c < 20 );	
        std::cout << i << " " << c << std::endl; 
        f << c << std::endl;
        std::size_t max_bad = c / 10;
        for( std::size_t i = 0; i < c ; ++i )
            f << !(rand() % (c/10 - 1) || !max_bad || !--max_bad) << std::endl;
    }
    
}
