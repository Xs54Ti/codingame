#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

struct Square {         // struct representing each square of the map
    int id, x, y;
    char type;
    int count = 0;
    bool visited = false;
};

int main()
{
    int L, H, N, x, y, id = 0;
    cin >> L >> H; cin.ignore();
    vector<vector<Square>> plan(L, vector<Square>(H));          // whole map in Squares
    for (int i = 0; i < H; i++) {
        string row;
        getline(cin, row);
        for(int j = 0; j < L; j++) {
            plan[j][i].type = row[j];
            plan[j][i].id = id;
            id++;
            plan[j][i].x = j;
            plan[j][i].y = i;
        }
    }
    cin >> N; cin.ignore();
    Square * currentSquare;
    map<int, Square*> listVisited;              // list of already visited Square
    for (int i = 0; i < N; i++) {
        int X, Y, count = 0;
        cin >> X >> Y; cin.ignore();

        map<int, Square*> lakeList;              // current list of Square which are part of the lake
        vector<Square*> currentList;             // list of square we currently need to explore
        if(plan[X][Y].visited == false) {
            currentList.push_back(&plan[X][Y]);
            while(currentList.size() != 0) {        // loop while we have something to explore
                currentSquare = currentList.back();
                currentList.pop_back();
                x = currentSquare->x;
                y = currentSquare->y;
                if(currentSquare->visited == false) {   // we do something only if we didn't visit the Square already
                    currentSquare->visited = true;
                    if(currentSquare->type == 'O') {
                        count++;
                        lakeList.emplace(currentSquare->id, currentSquare);

                        if(0 <= x+1 && x+1 < L) {                   // we push neighbour of the currentSquare on the list to explore
                            currentList.push_back(&plan[x+1][y]);
                        }  
                        if(0 <= y+1 && y+1 < H) {
                            currentList.push_back(&plan[x][y+1]);
                        }
                        if(0 <= x-1 && x-1 < L) {
                            currentList.push_back(&plan[x-1][y]);
                        }
                        if(0 <= y-1 && y-1 < H) {
                            currentList.push_back(&plan[x][y-1]);
                        }                     
                    }
                } 
            }
            for(auto element : lakeList) {      // mémoïsation of the lake's size on each of the Squares representing the lake
                element.second->count = count;
            }
        } else {
            count = plan[X][Y].count;           // we already calculate the lake size at this point so we can directly print it
        }
        cout << count << endl;
    }
}
