#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

//Actor class that serves as the base class for all game actors
class Actor : public GraphObject
{
public:
    Actor(int id, int x, int y, int direction, int depth, double size, StudentWorld* world);
    virtual void doSomething() = 0;
    
    virtual ~Actor(){};

    bool getAlive() {return m_alive;}

    virtual bool isSquare() {return false;}

    virtual bool isCoinSquare() { return false; }
    virtual bool isDirectionSquare() {return false;}

    int getStartX() const {return m_startX;}
    int getStartY() const {return m_startY;}


    virtual bool isBad() {return false;}

    virtual bool isBowser() {return false;}
    virtual bool isBoo() {return false;}
    virtual bool isVortex() {return false;}
    virtual bool isActive() {return false;}


    StudentWorld* getWorld() {return m_world;}

    bool getLanded() {return m_landed;}
    void setLanded(bool land) {m_landed = land;}



private:
    StudentWorld* m_world;
    bool m_alive;
    int m_startX;
    int m_startY;
    int curX;
    int curY;
    bool m_landed;

    
};

class Move : public Actor
{
public:
    Move(int id, int x, int y, StudentWorld* world);

    void setTicks(int ticks) {m_ticks = ticks;}
    int getTicks() {return m_ticks;}

    int getRolls() {return (m_ticks + 7) / 8;}

    bool isWalking() {return m_walking;}
    void setWalking(bool state) {m_walking = state;}

    void setmoveDirection(int direction) {m_direction = direction;}
    int getmoveDirection() {return m_direction;}
    void changeDirection(int direction);

    void findrandomLegalDirection();
    void findValidDirection();
    void findrandomValidForkDirection();

    void findValidDirectionforBowserandBoo();




private:
    int m_direction;
    int m_ticks;
    bool m_walking;

};

class Player : public Move
{
public:
    Player(int id, int x, int y, int pnum, StudentWorld* world);
    virtual void doSomething();
    int getPlayerNumber() {return m_playernumber;};
    
    void minusCoins(int coins);
    void setCoins(int coins) {m_coins = coins;} 
    void addCoins(int coins) {m_coins += coins;}
    int getCoins() {return m_coins;}

    int getStars() {return m_stars;}
    void setStars(int stars) {m_stars = stars;} 
    void addStar();
    void minusStars(int stars);

    void givetoBank();

    void setVortex(bool vortex) {m_vortex = vortex;}
    bool getVortex() {return m_vortex;}

    bool getTouchedBower() {return m_touchedBowser;}
    void setTouchedBowser(bool touched) {m_touchedBowser = touched;}

    bool getTouchedBoo() {return m_touchedBoo;}
    void setTouchedBoo(bool touched) {m_touchedBoo = touched;}


private:
    int m_playernumber;
    int m_coins;
    int m_stars;
    bool m_vortex;

    bool m_touchedBowser;
    bool m_touchedBoo;
};

class Bad : public Move
{
public:
    Bad(int id, int x, int y, StudentWorld* world);
    virtual void doSomething(){};
    void setCoins(int coins);
    void minuspauseCounter() {m_pauseCounter -= 1;}
    void setpauseCounter(int count) {m_pauseCounter = count;}
    int getpauseCounter() {return m_pauseCounter;}
    void badMove();

    void setImpacted(bool impacted) {m_impacted = impacted;}
    bool getImpacted() {return m_impacted;}

    virtual bool isBad() {return true;}


private:
    int m_pauseCounter;
    bool m_bowser;
    bool m_impacted;

};

class Bowser : public Bad
{
    public:
        Bowser(int id, int x, int y, StudentWorld* world);
        virtual void doSomething();
        virtual bool isBowser() {return true;}
        virtual bool isBad() {return true;}


};

class Boo : public Bad
{
    public:
        Boo(int id, int x, int y, StudentWorld* world);
        virtual void doSomething();
        virtual bool isBoo() {return true;}
        virtual bool isBad() {return true;}


};

class Vortex : public Move
{
    public:
        Vortex(int id, int x, int y, int direction, StudentWorld* world);
        virtual void doSomething();
        bool getVortexDirection() {return m_direction;}
        virtual bool isActive() {return m_active;}
        void setActive(bool state) {m_active = state;}
        virtual bool isVortex() {return true;}


    private:
        bool m_direction;
        bool m_active;
};


class CoinSquare : public Actor
{
public:
    CoinSquare(int id, int x, int y, bool color, StudentWorld* world);
    void doSomething();
    bool isBlue() {return m_blue;}
    virtual bool isSquare() {return true;}

private:
    bool m_blue;

};

class StarSquare : public Actor
{
public:
    StarSquare(int id, int x, int y, StudentWorld* world);
    void doSomething();
    bool isStarSquare() {return true;}
    virtual bool isSquare() {return true;}
};

class BankSquare : public Actor
{
public:
    BankSquare(int id, int x, int y, StudentWorld* world);
    void doSomething();
    bool isBankSquare() {return true;}
    virtual bool isSquare() {return true;}
};


class DirectionSquare : public Actor
{
public:
    DirectionSquare(int id, int x, int y, int direction, StudentWorld* world);
    void doSomething();
    bool isDirectionSquare() {return true;}
    virtual bool isSquare() {return true;}



private:

};

class EventSquare : public Actor
{
public:
    EventSquare(int id, int x, int y, StudentWorld* world);
    void doSomething();
    bool isEventSquare() {return true;}
    virtual bool isSquare() {return true;}

};

class DroppingSquare : public Actor
{
    public: 
        DroppingSquare(int id, int x, int y, StudentWorld* world);
        void doSomething();
        bool isDroppingSquare() {return true;}
        virtual bool isSquare() {return true;}

};








#endif // ACTOR_H_
