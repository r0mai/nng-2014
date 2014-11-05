#include <vector>
#include <set>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <iostream>

#include "bloodbank_api.h"

typedef std::size_t U;
typedef unsigned long long ULL;
typedef std::vector< U > UVec;
typedef UVec::iterator UVec_iter;

// halmaz struktúra, amin keresünk
struct IVSet
{
    UVec begins; // a halmazok kezdőértékei
    UVec sizes; // a halmazok méretei
    UVec setEnds;

    IVSet(){}

    IVSet(  const UVec & b,
            const UVec & s,
            const UVec & e):
            begins(b),
            sizes(s),
            setEnds(e)
    {}

    inline U getSetIndex( const U& index )
    {
        return std::upper_bound( setEnds.begin(), setEnds.end(), index ) - setEnds.begin();
    }

    inline U getValue( const U& elem )
    {
        return getValue( elem , getSetIndex( elem ) );
    }

    inline U getValue( const U& elem , const U& setIndex )
    {
        return ( setIndex == setEnds.size() ) ?
                    begins.back() + sizes.back() :
                    begins[ setIndex ] + ( setIndex ? elem - setEnds[ setIndex - 1 ] : elem );
    }

    inline U size() const
    {
        return setEnds.size() ? setEnds.back() : 0;
    }

    inline void add( const U& elem )
    {
        if( setEnds.size() && begins.back() + sizes.back() == elem )
        {
            ++sizes.back();
            ++setEnds.back();
        }
        else
        {
            begins.push_back( elem );
            sizes.push_back( 1 );
            setEnds.push_back( size() + 1 );
        }
    }
};


void runtests(BloodBank& bank)
{
    const U N = bank.getNumberOfSamples();
    U M = N / 10; // max!!

    if( !M ) M = 1;

    U need = std::ceil( N * 0.8 );

    double K = 2.;

    IVSet ivset(std::vector< U >( 1 ),
                std::vector< U >( 1 , N ),
                std::vector< U >( 1 , N ));

    for( char c = 0 ; c < 3 ; ++c )
    {
        ULL tol = 0; // halmazindexek
        ULL ig = 0;

        const U db = ivset.size();
        const U testnum = M*K;
        U number = testnum;

        Batch* batch = bank.createBatch();
        /*
        std::cerr << "::batch:" << (int)c
                  << "::  MBB:" << M
                  << "::    K:" << K
                  << "::   TN:" << testnum
                  << "::   db:" << db
                  << ":: dbps:" << db / ( M * K )
                  << "::biSet:" << db - ( db / K )
                  << ":: need:" << need
                  << std::endl;*/
        if( db - ( db / K ) > need )
        {
            number = M + std::ceil( need * M * K / db );
            /*std::cerr << "simplY!!! to " << number << std::endl;*/
        }

        for( U i = 1 ; i <= number ; ++i )
        {
            ig = db * i / testnum;

            if(ig > tol)
            {
                U testId = batch->addTest();

                for( ULL j = tol ; j != ig ; ++j )
                {
                    batch->addSample( testId , ivset.getValue( j ) );
                }
                tol = ig;
            }
        }
        bank.evaluateBatch( batch );

        bool b = false;

        IVSet newSet;

        for( U i = 0; i < batch->getNumberOfTests() ; ++i )
        {
            std::vector< U > samples;
            batch->getSamples( i , samples );
            if( batch->getTestResult( i ) )
            {
                for( UVec_iter it = samples.begin() ; it != samples.end() ; ++it )
                {
                    newSet.add( *it );
                }
            }
            else
            {
                for( UVec_iter it = samples.begin() ; it != samples.end() ; ++it )
                {
                    --need;

                    bank.markSafe( *it );

                    if( !need )
                        b = true;
                }
            }
        }
        delete batch;

        if( b ) return;

        ivset = newSet;
    }

    std::cerr << "HELP ME GOD" << std::endl;
    // WE ARE SOO FUCKED!!!
    srand(time(NULL));

    std::set< U > randoms;

    while( randoms.size() != need )
        randoms.insert( rand() % ivset.size() );

    for( std::set< U >::iterator it = randoms.begin() ; it != randoms.end() ; ++it )
        bank.markSafe( ivset.getValue( *it ) );


}
