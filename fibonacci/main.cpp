
#include <string>
#include <iostream>
#include <vector>

typedef unsigned long long UL;

std::string fs;
std::string ss;
UL dik;
UL count = 0;
struct node
{
    node* f;
    node* s;
    UL length;
    UL end;

    node( node* s = NULL, node* f = NULL ):
        f(f),s(s),
        length( s ?
                    f ?
                        s->length + f->length
                    :
                        s->length + ss.length()
                :
                    fs.length() + ss.length() ),
        end( s? s->end + length : 2*length )

        { ++count; }
    ~node()
    {
        --count;
        if( f ) delete f;
    }
    void benne()
    {
        if( end >= dik )
        {
            //std::cout << "e/d/l " << end << " " << dik << " " << length << " " <<  length - (end - dik) << std::endl;
            std::cout << keres( length - (end - dik) ) << std::endl;
            throw "";
        }

    }
    char keres( UL n )
    {
        std::cout << "KERES : " << n << " - " << length << " - " << end << std::endl;
        return  s ?
                    f ?
                        n > f->length ?
                            s->keres( n - f->length )
                        :
                            f->keres( n )
                    :
                        n > ss.length() ?
                            s->keres( n - ss.length() )
                        :
                            ss[ n - 1 ]
                :
                    n > fs.length() ?
                        ss[ n - fs.length() - 1 ]
                    :
                        fs[ n - 1 ];
    }
private:
    node(const node&){}
};



int main()
{
    std::cin >> fs >> ss >> dik;

    if( dik <= fs.size() )
    {
        std::cout << fs[ dik - 1 ] << std::endl;
        return 0;
    }
    if( dik <= fs.size() + ss.size() )
    {
        std::cout << ss[ dik - fs.length() - 1 ] << std::endl;
        return 0;
    }

    node* f = new node();
    node* s = new node( f );
    try
    {
        f->benne();
        s->benne();
        while(true)
        {
            node* n = new node( s , f );
            f = s;
            s = n;
            n->benne();
        }
    }
    catch(...)
    {

    }
    delete f;
    delete s;
    std::cout << "memszemet: " << count <<std::endl;
}
