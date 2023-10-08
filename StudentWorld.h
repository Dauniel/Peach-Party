#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Board.h"

#include <string>
#include <vector>

#include "Actor.h"

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();

    ~StudentWorld();

    Player* getPlayer1() const;
    Player* getPlayer2() const;

    Actor* getBowser(int x, int y);
    Actor* getBoo(int x, int y);

    void deleteVortex();

    bool isBowser(int x, int y);
    bool isBoo(int x, int y);

    


    bool onDirectionSquare(int x, int y) const;
    bool onSquare(int x, int y) const;

    int giveBanktoPlayer();
    void addtoBank(int coins) {m_bankCoins += coins;}
    int getBankCoins() {return m_bankCoins;}



    int getNumberOfActors() const;

    bool checkRight(int x, int y) const;
    bool checkLeft(int x, int y) const;
    bool checkUp(int x, int y) const;
    bool checkDown(int x, int y) const;

    void addDroppingSquare(int x, int y);

    void deleteSquare(int x, int y);

    bool isFork(int x, int y, int direction) const;
    bool isForkforBowserandBoo(int x, int y, int direction) const;

    void addVortex(int x, int y, int direction);
    void deleteVortex(int x, int y);


    void randomTeleport(Actor* thing);

    bool isHit(int x, int y);

    void swapPlayers(Player* p1, Player* p2);

    
private:

    Player* m_players[2];
    std::vector<Actor*> m_actors;

    bool m_clean;

    int m_bankCoins;

};

#endif // STUDENTWORLD_H_
