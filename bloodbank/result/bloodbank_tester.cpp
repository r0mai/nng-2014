#include <vector>
#include <set>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <queue>
#include <functional>
#include <iostream>
#include <random>
#include <chrono>

#include "bloodbank_api.h"

typedef std::size_t U;
typedef unsigned long long ULL;
typedef std::vector< U > UVec;
typedef UVec::iterator UVec_iter;
UVec transl;
UVec b_transl;

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
        return
            transl[
            getValue_( elem , getSetIndex( elem ) )
            ]
            ;
    }

    inline U getValue_( const U& elem , const U& setIndex )
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
        if( setEnds.size() &&
            transl[
            begins.back() + sizes.back()
            ]
             == elem )
        {
            ++sizes.back();
            ++setEnds.back();
        }
        else
        {
            begins.push_back(
            b_transl[
            elem
            ]
            );
            sizes.push_back( 1 );
            setEnds.push_back( size() + 1 );
        }
    }
};

double maxi = 0;
void runtests(BloodBank& bank)
{
    const U N = bank.getNumberOfSamples();
    //std::cerr << "N:" << N << "\t" << N*0.4 << "\t" << (maxi+=N*0.4) << "\t";
    transl.resize( N );
    b_transl.resize( N );
    for( U i = 0; i < N ; ++i )
    {
        transl[i] = i;
    }
    std::default_random_engine rd(std::chrono::system_clock::now().time_since_epoch().count());
    std::shuffle( transl.begin() , transl.end() , rd );

    for( U i = 0; i < N ; ++i )
    {
        b_transl[ transl[i] ] = i;
    }


    U M = std::ceil( N / 10. ); // max!!

    if( !M ) M = 1;

    U need = std::ceil( N * 0.8 );

    double K = 2. - (N>99 ? N / 3333333. : 0);

    //if( N <= 100 ) K = 5.;

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
        //if( db - ( db / K ) > need )
        {   //*
            std::priority_queue<U , std::vector<U> , std::greater<U> > que;
            ULL tol_tmp = 0;
            ULL ig_tmp = 0;
            ULL sum = 0;
            U i = 1;
            for(  ; i < number ; ++i )
            {
                ig_tmp = db * i / testnum;

                ULL halm_db = ig_tmp-tol_tmp ;

                que.push( halm_db );
                if( que.size() > M )
                {
                    //std::cout << que.top() << " ";
                    sum += que.top();
                    que.pop();
                }
                if( sum >= need ) break;
                tol_tmp = ig_tmp;

            }//*/
            //std::cout << std::endl;
            //number = M + std::ceil( need * M * K / db ) + 1; // need / ( db / testnum ); //
            // ki kéne számolni, hogy várhatóan mennyit teszünk bele, és annyi "halmazt" beletenni, amennyiben ha a max-ok rosszak, akkor is jók legyünk.
            number = i;
            //std::cerr << testnum << "simplY!!! to " << number << std::endl;
        }

        for( U i = 1 ; i <= number ; ++i )
        {
            ig = db * i / testnum;
            //std::cerr << (int)c << "max" << db << " need" << need  << " " << tol << " " << ig << std::endl;

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
        /*
        if( c == 0 && M / static_cast<double>(need) > 2 )
            M += 1;
        if( c == 1 && M / static_cast<double>(need) > 4/3. )
            K = 5.;*/
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
