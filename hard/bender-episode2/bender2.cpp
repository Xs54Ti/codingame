#include <iostream>
#include <string>
#include <vector>

using namespace std;

void recursion (int index, int money, vector<vector<int>> &rooms, vector<vector<int>> &rooms2, 
        int &finalMoney) { // each recursion have its own index and money, but all recursion use same tables and finalMoney
    money += rooms[index][1]; 
    if (money < rooms2[index][1]) { // check if we don't already have a better road to get to this room
        return; // if it's the case we don't need to continue checking this road
    }
    rooms2[index][1] = money; // it's the best road to this room we found for now, save the money we got
    
    if (rooms[index][2] == -1) { // if the road is over with first door, check if it's better than the best road (=max money) we got
        if( finalMoney < money) {
            finalMoney = money;
        } 
    } else {
        recursion (rooms[index][2], money, rooms, rooms2, finalMoney); // continue the road through first door
    }
    
    if (rooms[index][3] == -1) { // same as above but for second door
        if (finalMoney < money) {
            finalMoney = money;
        } 
    } else {
        recursion (rooms[index][3], money, rooms, rooms2, finalMoney);
    }
}

int main()
{
    int N;
    cin >> N; cin.ignore();
    vector<vector<int>> rooms; // table for room number, money and doors numbers 
    vector<vector<int>> rooms2; // same table but used for saving max money you can have when you get to a room
    int finalMoney = 0;
    vector<int> v;
    for (string s; cin >> s; ) { // fill the table
        if (s == "E") {
            v.push_back(-1);
        } else {
            v.push_back(stoi(s));
        }
        
        if(v.size() == 4) {
            rooms.push_back(v);
            v.clear();
        }
    }
    rooms2 = rooms;
    recursion (0, 0, rooms, rooms2, finalMoney); // recursive function
    cout << finalMoney << endl;
}
