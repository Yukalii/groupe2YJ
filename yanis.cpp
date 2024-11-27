#include <iostream>
#include <string>
#include <vector>

using namespace std;

class test
{
public:
    test()
    {
        cout << "Constructor called" << endl;
    }
    ~test()
    {
        cout << "Destructor called" << endl;
    }
};

int main()
{
    
    return 0;
}