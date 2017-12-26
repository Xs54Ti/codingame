#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>

using namespace std;

double distance(double x, double y, double x2, double y2) {
    return sqrt((x - x2) * (x - x2) + (y - y2) * (y - y2));
}

struct Player;
struct Entity;
struct Wreck;
struct Skill;
struct Vehicle; 
struct Tanker;
struct Looters;
struct Reaper;
struct Destroyer;
struct Doof;

template<class T>
T* nearest(vector<T*> table, Vehicle &vehicle) {
    int minDist = 0;
    T* newEntity;
    for(auto element:table) {
        int dist = distance(vehicle.x, vehicle.y, element->x, element->y);
        if(minDist == 0 || dist < minDist) {
            minDist = dist;
            newEntity = element;
        }
    }
    return newEntity;
}

struct Player {
    int score, rage;
    Reaper* reaper;
    Destroyer* destroyer;
    Doof* doof;
};

struct Entity {
    double unitId, unitType, radius, x, y;
    Entity(){}
    Entity(double unitId, double unitType, double radius, double x, double y) 
            : unitId(unitId), unitType(unitType), radius(radius), x(x), y(y){}
};

double distance2(Entity *first, Entity *second) {
    return sqrt((first->x - second->x) * (first->x - second->x) + (first->y - second->y) * (first->y - second->y));
}

template<class T>
pair<double, double> eval(vector<T*> table, vector<vector<Wreck*>> overlappingWrecks, Vehicle &vehicle, Player &opponent1, Player &opponent2) {
    double score = 0;
    double targetX, targetY;
    bool alreadyOnAWreck = false;
    for(auto element:table) {
        double dist = distance2(element, &vehicle);
        double scoreDistance = (-dist/1000) + 7;
 
        if(dist < element->radius + vehicle.radius) {
            score = element->water + scoreDistance;
            targetX = element->x;
            targetY = element->y;
            alreadyOnAWreck = true;
            break;
        } else if(score < element->water + scoreDistance) {
            score = element->water + scoreDistance;
            targetX = element->x;
            targetY = element->y;
        }
    }

    for(auto element:overlappingWrecks) {
        double dist = distance(vehicle.x, vehicle.y, (element[0]->x + element[1]->x)/2, (element[0]->y + element[1]->y)/2);
        double scoreDistance = (-dist/1000) + 7;
        if(score < element[0]->water + element[1]->water + scoreDistance) {
            if(!alreadyOnAWreck || (alreadyOnAWreck && dist < 2000)) {
                score = element[0]->water + element[1]->water + scoreDistance;
                targetX = (element[0]->x + element[1]->x)/2;
                targetY = (element[0]->y + element[1]->y)/2;
            }
        }
    }
    
    return pair<double, double>(targetX, targetY);
}


struct Wreck:Entity {
    double water;
    Wreck(){}
    Wreck(double unitId, double unitType, double radius, double x, double y, double water) 
            : Entity(unitId, unitType, radius, x, y), water(water){}
};

struct Skill:Entity {
    double duration;
    Skill(){}
    Skill(double unitId, double unitType, double radius, double x, double y, double duration) 
            : Entity(unitId, unitType, radius, x, y), duration(duration){}
};

struct Vehicle:Entity {
    double vx, vy, mass;
    Vehicle(){}
    Vehicle(double unitId, double unitType, double radius, double x, double y, double vx, double vy, double mass) 
            : Entity(unitId, unitType, radius, x, y), vx(vx), vy(vy), mass(mass){}
};

struct Tanker:Vehicle {
    double capacity, water;
    Tanker(){}
    Tanker(double unitId, double unitType, double radius, double x, double y, double vx, double vy, double mass, double capacity, double water) 
            : Vehicle(unitId, unitType, radius, x, y, vx, vy, mass), capacity(capacity), water(water){}
};

struct Looters:Vehicle {
    double playerId;
    Entity* target;
    int targetX;
    int targetY;

    void update(double unitId, double unitType, double radius, double x, double y, double vx, double vy, double mass, double playerId) {
        this->unitId = unitId;
        this->unitType = unitType;
        this->radius = radius;
        this->x = x;
        this->y = y;
        this->vx =vx;
        this->vy = vy;
        this->mass = mass;
        this->playerId = playerId;
    }
};

struct Reaper:Looters {
    int tarCounter = 0;

    void action(vector<Wreck*> wrecks, vector<Tanker*> tankersIn, vector<Tanker*> tankersOut, Player &player, Player &opponent1, Player &opponent2, vector<vector<Wreck*>> overlappingWrecks);
};

struct Destroyer:Looters {
    void action(vector<Wreck*> wrecks, vector<Tanker*> tankersIn, Reaper &bestOReaper, Player &player, Player &opponent1, Player &opponent2);
};

struct Doof:Looters {
    int oilCounter = 0;

    void action(vector<Wreck*> wrecks, Reaper &bestOReaper, Player &player) {
        if(oilCounter > 0) {
            oilCounter--;
        }
        targetX = bestOReaper.x - 1.5 * vx;
        targetY = bestOReaper.y - 1.5 * vy;
        target = &bestOReaper;
        string doofOrder = to_string(targetX) + " " + to_string(targetY) + " 300";
        if(oilCounter == 0 && player.rage >= 30 && wrecks.size() != 0) {
            Wreck* nearWreck = nearest(wrecks, bestOReaper);
            if(distance(player.reaper->x, player.reaper->y, nearWreck->x, nearWreck->y) > 1800
                        && distance(nearWreck->x, nearWreck->y, bestOReaper.x, bestOReaper.y) < bestOReaper.radius + nearWreck->radius + 200
                        && distance(x, y, nearWreck->x, nearWreck->y) <= 2000) {
                doofOrder = "SKILL " + to_string((int)nearWreck->x) + " " + to_string((int)nearWreck->y);
                player.rage -= 30;
                oilCounter = 3;
            }
        }
        cout << doofOrder << " DOOF"<< endl;
    }
};

void Reaper::action(vector<Wreck*> wrecks, vector<Tanker*> tankersIn, vector<Tanker*> tankersOut, Player &player, Player &opponent1, 
            Player &opponent2, vector<vector<Wreck*>> overlappingWrecks) {
    if(tarCounter > 0) {
        tarCounter--;
    }

    if(wrecks.size() != 0) {
        pair<double, double> nearWreck = eval(wrecks, overlappingWrecks, *this, opponent1, opponent2);
        targetX = nearWreck.first - 1.5 * vx;
        targetY = nearWreck.second - 1.5 * vy;
    } else if(tankersIn.size() != 0) {
        pair<double, double> nearTanker = eval(tankersIn, overlappingWrecks, *this, opponent1, opponent2);
        targetX = nearTanker.first - 1.5 * vx;
        targetY = nearTanker.second - 1.5 * vy;
    } else {
        pair<double, double> nearTanker = eval(tankersOut, overlappingWrecks, *this, opponent1, opponent2);
        targetX = nearTanker.first - 1.5 * vx;
        targetY = nearTanker.second - 1.5 * vy;
    }
    string reaperOrder = to_string(targetX) + " " + to_string(targetY) + " 300";
    cout << reaperOrder << " REAPER" << endl;
}

void Destroyer::action(vector<Wreck*> wrecks, vector<Tanker*> tankersIn, Reaper &bestOReaper, Player &player, Player &opponent1, Player &opponent2) {
    if(tankersIn.size() != 0) {
        target = nearest(tankersIn, *player.reaper);  
        targetX = target->x - 1.5 * vx;
        targetY = target->y - 1.5 * vy;
    } else {
        targetX = 0;
        targetY = 0;
    }
    string destroyerOrder = to_string(targetX) + " " + to_string(targetY) + " 300";
    if(player.rage >= 60 && distance2(this, player.reaper) <= 2000) {
        int trafficJam = 0;
        if(distance2(player.reaper,opponent1.reaper) < 1000){
            trafficJam++;
        }
        if(distance2(player.reaper,opponent1.destroyer) < 1000){
            trafficJam++;
        }
        if(distance2(player.reaper,opponent1.doof) < 1000){
            trafficJam++;
        }
        if(distance2(player.reaper,opponent2.reaper) < 1000){
            trafficJam++;
        }
        if(distance2(player.reaper,opponent2.destroyer) < 1000){
            trafficJam++;
        }
        if(distance2(player.reaper,opponent2.doof) < 1000){
            trafficJam++;
        }

        if(trafficJam >= 2) {
            destroyerOrder = "SKILL " + to_string((int)player.reaper->x) + " " + to_string((int)player.reaper->y);
            player.rage -= 60;
        }
    }
    cout << destroyerOrder << " DESTROYER"<< endl;
}


int main()
{
    Reaper reaper, oReaper1, oReaper2;
    Destroyer destroyer, oDestroyer1, oDestroyer2;
    Doof doof, oDoof1, oDoof2;
    Player player, opponent1, opponent2;

    player.reaper = &reaper;
    player.destroyer = &destroyer;
    player.doof = &doof;

    opponent1.reaper = &oReaper1;
    opponent1.destroyer = &oDestroyer1;
    opponent1.doof = &oDoof1;

    opponent2.reaper = &oReaper2;
    opponent2.destroyer = &oDestroyer2;
    opponent2.doof = &oDoof2;

    vector<Tanker*> tankersIn, tankersOut;
    vector<Wreck*> wrecks;
    vector<Skill*> tarPools, oilPools;

    while (1) {
        int unitCount;
        cin >> player.score >> opponent1.score >> opponent2.score >> player.rage >> opponent1.rage >> opponent2.rage >> unitCount; cin.ignore();
        for (int i = 0; i < unitCount; i++) {
            int unitId, unitType, playerId, radius, x, y, vx, vy, extra, extra2;
            float mass;
            cin >> unitId >> unitType >> playerId >> mass >> radius >> x >> y >> vx >> vy >> extra >> extra2; cin.ignore();

            if(unitType == 4) {
                Wreck* newWreck = new Wreck(unitId, unitType, radius, x, y, extra);
                wrecks.push_back(newWreck);
            } else if(unitType == 3 && extra != 0) {
                Tanker* newTanker = new Tanker(unitId, unitType, radius, x, y, vx, vy, mass, extra2, extra);
                if(distance(0, 0, x, y) <= 6000) {
                    tankersIn.push_back(newTanker);
                } else {
                    tankersOut.push_back(newTanker);
                }
            } else if(unitType == 5) {
                Skill* newSkill = new Skill(unitId, unitType, radius, x, y, extra);
                tarPools.push_back(newSkill);
            } else if(unitType == 6) {
                Skill* newSkill = new Skill(unitId, unitType, radius, x, y, extra);
                oilPools.push_back(newSkill);
            } else {
                if(unitType == 0) {
                    if(playerId == 0) {
                        reaper.update(unitId, unitType, radius, x, y, vx, vy, mass, playerId);   
                    } else if(playerId == 1) {
                        oReaper1.update(unitId, unitType, radius, x, y, vx, vy, mass, playerId);
                    } else {
                        oReaper2.update(unitId, unitType, radius, x, y, vx, vy, mass, playerId);
                    }
                }

                if(unitType == 1 && playerId == 0) {
                    if(playerId == 0) {   
                        destroyer.update(unitId, unitType, radius, x, y, vx, vy, mass, playerId);
                    } else if(playerId == 1) {
                        oDestroyer1.update(unitId, unitType, radius, x, y, vx, vy, mass, playerId);
                    } else {
                        oDestroyer2.update(unitId, unitType, radius, x, y, vx, vy, mass, playerId);
                    }
                }

                if(unitType == 2 && playerId == 0) {
                    if(playerId == 0) {   
                        doof.update(unitId, unitType, radius, x, y, vx, vy, mass, playerId);
                    } else if(playerId == 1) {
                        oDoof1.update(unitId, unitType, radius, x, y, vx, vy, mass, playerId);
                    } else {
                        oDoof2.update(unitId, unitType, radius, x, y, vx, vy, mass, playerId);
                    }
                }
            }

        }

        vector<Wreck*> problemWrecks;
        for(auto oilPool:oilPools) {
            for(auto currentWreck : wrecks) {
                if(distance2(oilPool, currentWreck) < 1000) {
                    if(find(problemWrecks.begin(), problemWrecks.end(), currentWreck) == problemWrecks.end()) {
                        problemWrecks.push_back(currentWreck);
                    }
                }
            }
        }

        for(auto problemWreck:problemWrecks) {
            wrecks.erase(find(wrecks.begin(),wrecks.end(), problemWreck));
        }
        problemWrecks.clear();

        vector<vector<Wreck*>> overlappingWrecks;
        for(auto currentWreck : wrecks) {
            for (auto wreck : wrecks) {
                if(wreck != currentWreck && distance2(currentWreck, wreck) < currentWreck->radius + wreck->radius) {
                    vector<Wreck*> v;
                    v.push_back(currentWreck);
                    v.push_back(wreck);
                    overlappingWrecks.emplace_back(v);
                }
            }
        }

        Reaper* bestOReaper;
        if(opponent1.score > opponent2.score) {
            bestOReaper = &oReaper1;
        } else {
            bestOReaper = &oReaper2;
        }

        reaper.action(wrecks, tankersIn, tankersOut, player, opponent1, opponent2, overlappingWrecks);
        destroyer.action(wrecks, tankersIn, *bestOReaper, player, opponent1, opponent2);
        doof.action(wrecks, *bestOReaper, player);


        for(auto element:tankersIn) {
            delete element;
        }
        for(auto element:tankersOut) {
            delete element;
        }
        for(auto element:wrecks) {
            delete element;
        }
        for(auto element:tarPools) {
            delete element;
        }
        for(auto element:oilPools) {
            delete element;
        }
        tankersIn.clear();
        tankersOut.clear();
        wrecks.clear();
        tarPools.clear();
        oilPools.clear();
        overlappingWrecks.clear();
    }
}
