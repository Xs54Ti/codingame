#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

struct Factory {
    int id;
    int nbCyb;
    int production;
    int belonging = 0;
    vector<int> fightScore;
};

Factory * nearestFactory(int source, map<int, Factory*> &list, vector<vector<int>> &distanceTable) {
    Factory *result = new Factory;
    result = list.begin()->second;
    for (auto & factory : list) {
         if(distanceTable[source][factory.second->id] < distanceTable[source][result->id]) {
            result = factory.second;
        }
    }
    return result;
}

int main()
{
    int factoryCount, linkCount;
    cin >> factoryCount; cin.ignore();
    cin >> linkCount; cin.ignore();
    vector<vector<int>> distanceTable(factoryCount, vector<int>(factoryCount));
    for (int i = 0; i < linkCount; i++) {
        int factory1, factory2, distance;
        cin >> factory1 >> factory2 >> distance; cin.ignore();
        distanceTable[factory1][factory2] = distance;
        distanceTable[factory2][factory1] = distance;
    }
    
    map<int, Factory*> ourFactories, targetFactories, enemyFactories, neutralFactories, allFactories, neutralProductiveFactories;

    int source, destination, nbTroupToSend, bomb = 2;
    string output;
    // game loop
    while (1) {
        int entityCount; // the number of entities (e.g. factories and troops)
        cin >> entityCount; cin.ignore();
        bool ourBombRunning = false;
        bool ennemyBombRunning = false;
        for (int i = 0; i < entityCount; i++) {
            int entityId, arg1, arg2, arg3, arg4, arg5;
            string entityType;
            cin >> entityId >> entityType >> arg1 >> arg2 >> arg3 >> arg4 >> arg5; cin.ignore();
            if(i < factoryCount) {
                Factory *newFactory = new Factory();
                newFactory->id = entityId;
                newFactory->nbCyb = arg2;
                newFactory->production = arg3;
                newFactory->belonging = arg1;
                
                if(arg1 == 1) {
                    newFactory->fightScore.push_back(arg2);
                    ourFactories.emplace(entityId, newFactory);
                } else if (arg1 == -1) {
                    newFactory->fightScore.push_back(-arg2);
                    enemyFactories.emplace(entityId, newFactory);
                } else if (arg1 == 0) {
                    newFactory->fightScore.push_back(-arg2);
                    neutralFactories.emplace(entityId, newFactory);
                    if(arg3 != 0) {
                        neutralProductiveFactories.emplace(entityId, newFactory);
                    }
                }

                if(arg3 != 0 || arg1 == -1) { 
                    allFactories.emplace(entityId, newFactory);    
                }

            }

            if(entityType == "TROOP" && allFactories.find(arg3) != allFactories.end()) {
                if(allFactories[arg3]->fightScore.size() >= arg5 + 1) {
                    for(int k = arg5; k < allFactories[arg3]->fightScore.size(); k++) {
                        allFactories[arg3]->fightScore[k] += (arg4 * arg1);
                    }
                } else {
                    int currentLastScore = allFactories[arg3]->fightScore.back();
                    while(allFactories[arg3]->fightScore.size() != arg5 + 1) {
                        allFactories[arg3]->fightScore.push_back(currentLastScore);
                    } 
                    allFactories[arg3]->fightScore[arg5] += (arg4 * arg1);
                }
            }

            if (entityType == "BOMB" && arg1 == 1) {
                ourBombRunning = true;
            } else if (entityType == "BOMB" && arg1 == -1) {
                ennemyBombRunning = true;
            }
        }

        for (auto & factory : allFactories) { 
            if(factory.second->belonging != 0) {
                if(factory.second->fightScore[0] > 0 || (factory.second->fightScore[0] == 0 && factory.second->belonging == 1)) { //premier next tour en fonction de belonging
                    for(int j = 1; j < factory.second->fightScore.size(); j++) { 
                        factory.second->fightScore[j] += factory.second->production;
                    }
                } else {
                    for(int j = 1; j < factory.second->fightScore.size(); j++) { 
                        factory.second->fightScore[j] -= factory.second->production;
                    }
                }

                for(int i = 2; i < factory.second->fightScore.size(); i++) {
                    if(factory.second->fightScore[i-1] > 0 || (factory.second->fightScore[i-1] == 0 && factory.second->fightScore[i-2] > 0)) {
                        for(int j = i; j < factory.second->fightScore.size(); j++) { 
                            factory.second->fightScore[j] += factory.second->production;
                        }
                    } else {
                        for(int j = i; j < factory.second->fightScore.size(); j++) { 
                            factory.second->fightScore[j] -= factory.second->production;
                        }
                    }
                }

                if(factory.second->fightScore.size() > 1) {
                    if(factory.second->fightScore.back() < 0 || (factory.second->fightScore.back() == 0 && factory.second->fightScore[factory.second->fightScore.size()-2] < 0)) {
                        targetFactories.emplace(factory.second->id, factory.second);
                        if(ourFactories.find(factory.second->id) != ourFactories.end()) {
                            ourFactories.erase(factory.second->id);
                        }
                    }
                }else {
                    if(factory.second->fightScore.back() < 0 || (factory.second->fightScore.back() == 0 && factory.second->belonging == -1)) {
                        targetFactories.emplace(factory.second->id, factory.second);
                        if(ourFactories.find(factory.second->id) != ourFactories.end()) {
                            ourFactories.erase(factory.second->id);
                        }
                    }
                }
            } else {
                targetFactories.emplace(factory.second->id, factory.second);
            }
        }

        output = "WAIT";
        cout << output;
 
        if(targetFactories.size() != 0) {

            for (auto & factory : ourFactories) {
                Factory *nearestEnnemy = nearestFactory(factory.second->id, enemyFactories, distanceTable);
                bool incWait = false, neutralBetterOption = false;
                if(neutralProductiveFactories.size() !=0) {
                    Factory *nearestNeutral = nearestFactory(factory.second->id, neutralProductiveFactories, distanceTable);
                    if(distanceTable[factory.second->id][nearestNeutral->id] < 10 ) {
                        neutralBetterOption = true;
                    }
                }


                if(!neutralBetterOption && factory.second->production < 3 && distanceTable[factory.second->id][nearestEnnemy->id] > 10 && factory.second->fightScore.back() > 0){
                    if(factory.second->nbCyb >= 10) {
                        cout << ";INC " << factory.second->id;
                        factory.second->nbCyb -= 10;
                    } else {
                        incWait = true;
                    }
                }

                if (!incWait) {
                    int cybs = factory.second->fightScore.back();
                    map<int, Factory*> copyTargets = targetFactories;
                    while(cybs > 0 && copyTargets.size() != 0) {
                        Factory *target = nearestFactory(factory.second->id, copyTargets, distanceTable);
                        destination = target->id;
                        if(target->fightScore.back()  <= 0) {
                            source = factory.second->id;
                            if(target->nbCyb + 1 < cybs) {
                                nbTroupToSend = target->nbCyb + 1;
                            } else {
                                nbTroupToSend = cybs;
                            }
                            cybs -= nbTroupToSend;
                            

                            // faire une étape
                            int stop = target->id;
                            map<int, Factory*> copySubTargets = ourFactories;
                            for (auto & element : neutralFactories) {
                                if(element.second->nbCyb == 0) {
                                    copySubTargets.insert(element);
                                }
                            }

                            int minDistance = distanceTable[factory.first][target->id];
                            for (auto & element : copySubTargets) {
                                if(element.first != factory.first && (distanceTable[factory.first][element.first] + distanceTable[element.first][target->id]) < minDistance) {
                                    stop = element.first;
                                    minDistance = distanceTable[factory.first][element.first] + distanceTable[element.first][target->id];
                                }   
                            }
                            destination = stop;
                            if(destination == target->id) {
                                target->fightScore.back()  += nbTroupToSend;
                            }

                            cout << ";MOVE " << source << " " << destination << " " << nbTroupToSend;
                            
                        } else {
                            copyTargets.erase(destination);
                        }
                    }
                }
            }
        }

        if(bomb != 0 && !ourBombRunning) {
            for (auto & factory : enemyFactories) {
                if(factory.second->production == 3) {
                    Factory *nearestAlly = nearestFactory(factory.second->id, ourFactories, distanceTable);
                    cout << ";BOMB " << nearestAlly->id << " " << factory.second->id;
                    bomb--;
                    break;
                }
            }
        }

        cout << endl;
        for (auto & factory : allFactories) {
            delete factory.second;    
        }
        ourFactories.clear();
        targetFactories.clear();
        enemyFactories.clear();
        neutralFactories.clear();
        neutralProductiveFactories.clear();
        allFactories.clear();
    }
}
