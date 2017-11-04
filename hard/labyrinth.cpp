#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

struct Square {
    int id, x, y, totalScore = 0, realScore = 0, parent = 0;
    char type = '?';
    bool visited = false, explored = false; 
    Square * parentObj = NULL;
};

bool compareByScore(const Square *a, const Square *b)
{
    return a->totalScore > b->totalScore;
}

void updateNeighbour(int x, int y, Square &current, vector<Square*> &toExplore, map<int,int> &toExploreMap, int directionToNeighbour, pair<int,int> &target, vector<vector<Square>> &map, int R, int C) {
    if(x >= 0 && x < R && y >= 0 && y < C) {
        Square &neighbour = map[x][y];
        if(neighbour.type != '#'&& neighbour.type != '?' && !neighbour.explored) {
            int newSubScore = abs(target.first - neighbour.x) + abs(target.second - neighbour.y);
            int newScore = current.realScore + 1 + newSubScore;
            if(toExploreMap.find(neighbour.id) == toExploreMap.end()) {
                neighbour.totalScore = newScore;
                neighbour.realScore = current.realScore + 1;
                neighbour.parent = directionToNeighbour;
                neighbour.parentObj = &current;
                toExplore.push_back(&neighbour);
                toExploreMap.insert(pair<int,int>(neighbour.id,1));
            } else if(newScore < neighbour.totalScore){
                neighbour.totalScore = newScore;
                neighbour.realScore = current.realScore + 1;
                neighbour.parent = directionToNeighbour;
                neighbour.parentObj = &current;
            }
        }   
    }
}

int main()
{
    int R, C, A; // number of rounds between the time the alarm countdown is activated and the time the alarm goes off.
    cin >> R >> C >> A; cin.ignore();
    vector<vector<Square>> map(R, vector<Square>(C)), map2(R, vector<Square>(C));
    vector<int> currentRoad, roadCtoT, roadtoC;
    bool endFound = false, returnFound = false, goFound = false, commandActivated = false;
    pair<int,int> entryPoint, commandPoint;

    while (1) {
        int KR, KC; // column where Kirk is located.
        cin >> KR >> KC; cin.ignore();
        int identity = 0;
        for (int i = 0; i < R; i++) {
            string ROW; // C of the characters in '#.TC?' (i.e. one line of the ASCII maze).
            cin >> ROW; cin.ignore();
            for(int j = 0; j < C; j++) {
                if(!returnFound) {
                    map[i][j].totalScore = 0;
                    map[i][j].realScore = 0;
                    map[i][j].parent = 0;
                    map[i][j].explored = false;
                    map[i][j].parentObj = NULL;         
                }

                map[i][j].x = i;
                map[i][j].y = j;
                identity++;
                map[i][j].id = identity;
                if(ROW[j] != '?' && map[i][j].type == '?') {
                    map[i][j].type = ROW[j];
                }
                if(ROW[j] == 'C') {
                    endFound = true;
                    commandPoint.first = i;
                    commandPoint.second = j;
                }
                if(ROW[j] == 'T') {
                    entryPoint.first = i;
                    entryPoint.second = j;
                }
            }
        }

        if(map[KR][KC].type == 'C') {
            commandActivated = true;
        }

        if(endFound && !returnFound) {
            map2 = map;
            vector<Square*> toExplore;
            std::map<int,int> toExploreMap;
            toExplore.push_back(&map[commandPoint.first][commandPoint.second]);
            toExploreMap.insert(pair<int,int>(map[commandPoint.first][commandPoint.second].id,1));
            while(toExplore.size()!=0) {
                sort(toExplore.begin(), toExplore.end(), compareByScore);
                Square *element = toExplore.back();
                toExplore.pop_back();
                element->explored = true;
                if(element->type == 'T' && element->realScore <= A) {
                    returnFound = true;
                    Square *endpoint = &map[entryPoint.first][entryPoint.second];
                    while(endpoint->parent != 0) {
                        roadCtoT.push_back(endpoint->parent);
                        endpoint = endpoint->parentObj;
                    }
                    break;
                } else {
                    updateNeighbour(element->x, element->y +1, *element, toExplore, toExploreMap, 1, entryPoint, map, R, C);
                    updateNeighbour(element->x+1, element->y, *element, toExplore, toExploreMap, 2, entryPoint, map, R, C);
                    updateNeighbour(element->x,element->y -1, *element, toExplore, toExploreMap, -1, entryPoint, map, R, C);
                    updateNeighbour(element->x-1, element->y, *element, toExplore, toExploreMap, -2, entryPoint, map, R, C);
                }
            }
            toExplore.clear();
            toExploreMap.clear();
        }


        map[KR][KC].visited = true;

        if(!returnFound) {
            if(map[KR][KC +1].type != '#' && map[KR][KC +1].type != 'C' && !map[KR][KC +1].visited) {
                cout << "RIGHT" << endl;
                currentRoad.push_back(1);
            } else if(map[KR+1][KC].type != '#' && map[KR+1][KC].type != 'C' && !map[KR+1][KC].visited) {
                cout << "DOWN" << endl;
                currentRoad.push_back(2);
            } else if(map[KR][KC -1].type != '#' && map[KR][KC -1].type != 'C' && !map[KR][KC -1].visited) {
                cout << "LEFT" << endl;
                currentRoad.push_back(-1);
            } else if(map[KR-1][KC].type != '#' && map[KR-1][KC].type != 'C' && !map[KR-1][KC].visited) {
                cout << "UP" << endl;
                currentRoad.push_back(-2);
            } else {
                if(currentRoad.back() == 1) {
                    cout << "LEFT" << endl;
                    currentRoad.pop_back();
                } else if(currentRoad.back() == 2) {
                    cout << "UP" << endl;
                    currentRoad.pop_back();
                } else if(currentRoad.back() == -1) {
                    cout << "RIGHT" << endl;
                    currentRoad.pop_back();
                } else if(currentRoad.back() == -2) {
                    cout << "DOWN" << endl;
                    currentRoad.pop_back();
                }
            }
            
        } else if(!goFound){
            vector<Square*> toExplore;
            std::map<int,int> toExploreMap;
            toExplore.push_back(&map2[KR][KC]);
            toExploreMap.insert(pair<int,int>(map2[KR][KC].id,1));
            while(toExplore.size()!=0) {
                sort(toExplore.begin(), toExplore.end(), compareByScore);
                Square *element = toExplore.back();
                toExplore.pop_back();
                element->explored = true;
                if(element->type == 'C') {
                    goFound = true;
                    Square *endpoint = &map2[commandPoint.first][commandPoint.second];
                    while(endpoint->parent != 0) {
                        roadtoC.push_back(endpoint->parent);
                        endpoint = endpoint->parentObj;
                    }
                    break;
                } else {
                    updateNeighbour(element->x, element->y +1, *element, toExplore, toExploreMap,1, commandPoint, map2, R, C);
                    updateNeighbour(element->x+1, element->y, *element, toExplore, toExploreMap,2, commandPoint, map2, R, C);
                    updateNeighbour(element->x, element->y -1, *element, toExplore, toExploreMap,-1, commandPoint, map2, R, C);
                    updateNeighbour(element->x-1, element->y, *element, toExplore, toExploreMap,-2, commandPoint, map2, R, C);
                }
            }
            toExplore.clear();
            toExploreMap.clear();
        }

        if(goFound && !commandActivated){
            if(roadtoC.back() == 1) {
                cout << "RIGHT" << endl;
            } else if (roadtoC.back() == 2) {
                cout << "DOWN" << endl;
            } else if (roadtoC.back() == -1) {
                cout << "LEFT" << endl;
            } else if(roadtoC.back() == -2) {
                cout << "UP" << endl;
            }
            roadtoC.pop_back();

        } else if(goFound && commandActivated){
            if(roadCtoT.back() == 1) {
                cout << "RIGHT" << endl;
            } else if (roadCtoT.back() == 2) {
                cout << "DOWN" << endl;
            } else if (roadCtoT.back() == -1) {
                cout << "LEFT" << endl;
            } else if(roadCtoT.back() == -2) {
                cout << "UP" << endl;
            }
            roadCtoT.pop_back();
        }
    }
}
