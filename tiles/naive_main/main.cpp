#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>

typedef unsigned long UL;
typedef std::pair<UL,UL> PUL;
typedef std::vector<UL> VUL;
typedef std::vector<VUL> MUL;


UL x,y;

UL count = 0;
std::stringstream ss;

void csere(UL e , UL k)
{
    //std::cout << e <<" " << k << std::endl;
    ++count;
    ss << e/y << " " << e%y << " " << k/y << " " << k%y << std::endl;
}

inline PUL tri_ind( UL w , UL mx , UL my )
{
    UL max_v = mx*my/2;
    if( w > max_v ) return {-1,-1};

    UL min_dim = std::min( mx , my );
    UL tria = min_dim * (min_dim + 1) / 2;

    if ( w < tria )
    {
        UL n = (std::sqrt(8*w+1)-1)/2;
        UL in = w - n*(n+1)/2;
        if( n%2 )
            return { in , n-in };
        else
            return { n-in , in };
    }
    else
    {
        w-=tria;

        if( mx < my )
        {
            return { w%min_dim - 1 , w/min_dim + 1 + w%min_dim };
        }
        else
        {
            return { min_dim - 1 + w/min_dim , w%min_dim - 1 };
        }
    }
}

struct matrix: private MUL
{
    matrix( const MUL& z ):MUL(z){}
    matrix( MUL&& z ):MUL(std::move(z)){}

    inline UL size_x() const { return this->size(); }
    inline UL size_y() const { return this->begin()->size(); }
    inline UL size_max() const { return size_x() * size_y(); }

    inline UL& get_x_y( UL x , UL y )
    {
        return (*this)[x][y];
    }
    inline UL& get_x_y( PUL p )
    {
        return get_x_y( p.first , p.second );
    }

    inline UL& get_x( UL x )
    {
        return (*this)[x/size_y()][x%size_y()];
    }

    inline UL& get_y( UL y )
    {
        return (*this)[y%size_x()][y/size_x()];
    }



    inline UL& get_diag( UL x ) // upper - lower
    {
        if( x <size_max()/2 )
        {
            PUL ind = tri_ind(x , size_x() , size_y() );
            return get_x_y( ind );
        }
        else
        {
            UL xm = size_max() - x -1;
            PUL ind = tri_ind(xm , size_x() , size_y() );
            return get_x_y( size_x() - ind.first - 1 , size_y() - ind.second - 1 );
        }
    }
    inline UL& get_rev_diag( UL x ) // upper - lower
    {
        if( x <size_max()/2 )
        {
            PUL ind = tri_ind(x , size_x() , size_y() );
            return get_x_y( ind.first , size_y() - ind.second - 1 );
        }
        else
        {
            UL xm = size_max() - x -1;
            PUL ind = tri_ind(xm , size_x() , size_y() );
            return get_x_y( size_x() - ind.first - 1 , ind.second );
        }
    }

    template <typename T>
    inline void csere( UL f , UL s , T fun )
    {
        ++count;

        UL& firref = (this->*fun)(f);
        UL& secref = (this->*fun)(s);

        std::swap( firref, secref );

        for( UL i = 0 ; i < size_x() ; ++i )
        {
            for( UL j = 0 ; j < size_y() ; ++j )
            {
                if( &this->get_x_y(i,j) == &firref )
                {
                    ss << i << " " << j << " ";
                }
                if( &this->get_x_y(i,j) == &secref )
                {
                    ss << i << " " << j << " ";
                }
            }
        }
        ss << std::endl;
    }
private:

};

MUL pe()
{
    MUL res;
    VUL x{0,1,2};
    do
    {
        res.push_back( x );
    }
    while ( std::next_permutation(x.begin(),x.end()) );
    return res;
}


int main()
{

    //VUL permback{ p1,p2,p3 };

    std::cin >> x >> y;
    matrix m(MUL(x,VUL(y,x*y)));

    if(0)
    {
        std::cout << "TEST" <<std::endl;
        for( UL i = 0; i <m.size_max() ; ++i )
            m.get_rev_diag( i ) = i;

        for( UL i = 0; i < m.size_x() ; ++i )
        {
            for( UL j = 0; j < m.size_y() ; ++j )
                std::cout << m.get_x_y( i , j ) << "\t";
            std::cout << std::endl;
        }
        return 0;
    }

    VUL db(3);

    MUL perms( pe() );

    for( UL i = 0 ; i < m.size_max() ; ++i )
    {
        std::cin >> m.get_x(i);
        ++db[m.get_x(i)];
    }

    UL best = -1;
    VUL permofthree;
    UL bestfunct;
    auto functions = {&matrix::get_x , &matrix::get_y , &matrix::get_diag , &matrix::get_rev_diag};

    for( UL j = 0; j < perms.size() ; ++j )
    {
        std::vector<UL> counters( functions.size() );
        for( UL i = 0 ; i < m.size_max() ; ++i )
        {
            if( i < db[ perms[j][0] ] )
            {
                for( auto it = functions.begin() ; it != functions.end() ; ++it )
                {
                    if( (m.**it)(i) != perms[j][0] )
                        ++counters[it-functions.begin()];
                }
            }
            else if( i < db[ perms[j][0] ] + db[ perms[j][1] ] )
            {
                for( auto it = functions.begin() ; it != functions.end() ; ++it )
                {
                    if( (m.**it)(i) != perms[j][1] )
                        ++counters[it-functions.begin()];
                }
            }
            else
            {
                for( auto it = functions.begin() ; it != functions.end() ; ++it )
                {
                    if( (m.**it)(i) != perms[j][2] )
                        ++counters[it-functions.begin()];
                }
            }
        }
        std::cerr << "perm of " << perms[j][0] << " "
                                << perms[j][1] << " "
                                << perms[j][2] << std::endl;


        for( UL k = 0 ; k < counters.size() ; ++k )
        {
            if( best > counters[k] )
            {
                best = counters[k];
                permofthree = perms[j];
                bestfunct = k;
            }
            std::cerr << k  << ". :" << counters[k] << " aka ++" << (counters[k]+1)/2 << std::endl;
        }
    }
    db = VUL({  db[permofthree[0]],
                db[permofthree[0]] + db[permofthree[1]],
                db[permofthree[0]] + db[permofthree[1]] + db[permofthree[2]] });

    auto funct = *(std::begin( functions ) + bestfunct);



    // calculate best permutation!!!



    //*

    UL i = 0;
    UL j = db[0];
    UL k = db[2] - 1;

    while( i < db[0] || j < db[1] || k >= db[1] )
    {
        //std::cout << "ittene: " << i <<" " << j <<" " << k << std::endl;
        while( i < db[0] && (m.*funct)(i)==permofthree[0] ) ++i;
        while( j < db[1] && (m.*funct)(j)==permofthree[1] ) ++j;
        while( k >= db[1] && (m.*funct)(k)==permofthree[2] ) --k;
        //std::cout << "ittenv: " << i <<" " << j <<" " << k << std::endl;

        if( i < db[0] && (m.*funct)(i) == permofthree[1] )
        {
            UL l = j;
            while( (m.*funct)(l) != permofthree[0] ) ++l;

            m.csere(i,l, funct);

            if( j == l )
                ++j;
            ++i;
        }
        else if( i < db[0] && (m.*funct)(i) == permofthree[2] )
        {
            UL l = k;
            while( (m.*funct)(l) != permofthree[0] ) --l;

            m.csere(i,l, funct);

            if( k == l )
                --k;
            ++i;
        }
        else if( j < db[1] )
        {
            m.csere(j,k, funct);

            --k;
            ++j;
        }
    }
    std::ofstream ofx("test/tiles_out_checker_" + std::to_string( count ) + ".out");
    //std::ostream& ofx(std::cout);

    UL u = 0;
    for( UL i = 0; i < m.size_max() ; ++i )
    {
        ofx << m.get_x(i) << " ";
        if( ++u % y == 0 ) ofx << std::endl;
    }
    std::cout << count << std::endl << ss.str();
    //*/
    return 0;
}
