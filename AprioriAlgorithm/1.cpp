#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;


int main(){
    set<int> set1;
    set<int> set2;
    set1.insert(1);
    set1.insert(2);
    set1.insert(3);
    set1.insert(4);
    set1.insert(5);
    set1.insert(6);
    set1.insert(7);
    
    
    set2.insert(2);
    set2.insert(3);
    set2.insert(4);
    
    
    set<int> result;
    set_difference(set1.begin(), set1.end(), set2.begin(), set2.end(),inserter(result, result.end()));
    
    for (set<int>::iterator it = result.begin(); it != result.end(); it++) {
        cout << *it << " ";
    }
    cout << endl;
    
    
    return 0;
}