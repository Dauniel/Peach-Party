#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"

using namespace std;

//Actor class that serves as the base class for all game actors
Actor::Actor(int id, int x, int y, int direction, int depth, double size, StudentWorld* world)
    : GraphObject(id, x, y, direction, depth, size), m_world(world), m_alive(true), m_startX(x), m_startY(y), m_landed(true) {}

//Base class for objects that can move
Move::Move(int id, int x, int y, StudentWorld* world)
    : Actor(id, x, y, 0, 0, 1, world),
     m_direction(0), m_ticks(0), m_walking(false) {}

void Move::changeDirection(int direction)
{
    //changing moving direction
    setmoveDirection(direction);
    //changing sprite direction
    if(direction == up or direction == down)
        setDirection(right);
    else 
        setDirection(direction);
}


Player::Player(int id, int x, int y, int pnum, StudentWorld* world)
    : Move(id, x, y, world),
    m_playernumber(pnum), m_coins(0), m_stars(0), m_vortex(true), m_touchedBowser(false){}

void Player::doSomething()
{
    if(!isWalking())
    {
        switch(getWorld()->getAction(m_playernumber))
        {
            case ACTION_ROLL:
                setTicks(randInt(1,10) * 8);
                // setTicks(8);
                setWalking(true);
                setTouchedBowser(false);
                setTouchedBoo(false);
                break;
            case ACTION_FIRE:
                if(!getVortex())
                    break;
                getWorld()->addVortex(getX(), getY(), getmoveDirection());
                getWorld()->playSound(SOUND_PLAYER_FIRE);
                setVortex(false);
                break;
            default:
                return;
        }
    }
            else if (isWalking())
            {

                int curX = getX();
                int curY = getY();
                    if(getWorld()->isFork(curX, curY, getmoveDirection()) and (curX != getStartX() or curY != getStartY()))
                    {
                        int action = getWorld()->getAction(m_playernumber);

                        if (action == ACTION_UP and getmoveDirection() != down and getWorld()->checkUp(curX, curY))
                         {
                            setmoveDirection(up);
                            setDirection(right);
                        }
                        else if (action == ACTION_DOWN and getmoveDirection() != up and getWorld()->checkDown(curX, curY)) 
                        {
                            setmoveDirection(down);
                            setDirection(right);
                        }
                        else if (action == ACTION_RIGHT and getmoveDirection() != left and getWorld()->checkRight(curX, curY)) 
                        {
                            setmoveDirection(right);
                            setDirection(right);
                        }
                        else if (action == ACTION_LEFT and getmoveDirection() != right and getWorld()->checkLeft(curX, curY))
                         {
                            setmoveDirection(left);
                            setDirection(left);
                        }
                        else {
                            return;
                        }
                    }

                    // Update player movement
                findValidDirection();
                moveAtAngle(getmoveDirection(), 2);
                setTicks(getTicks() - 1);
                if (getTicks() == 0)
                    setWalking(false);
            }
}

void Player::minusCoins(int coins)
{
    if(m_coins <= 0 or m_coins - coins <= 0)
        m_coins = 0;
    else    
        m_coins -= coins;
}

void Player::addStar()
{
        if(m_coins < 20)
            return;
        else
        {
        m_coins -= 20;
        m_stars += 1;
        }
}

void Player::minusStars(int stars)
{
    if(stars <= 0 or m_stars - stars <= 0)
        m_stars = 0;
    else
        m_stars -= 1;
}

void Player::givetoBank()
{
    if(m_coins <= 5)
    {
        getWorld()->addtoBank(m_coins);
        m_coins = 0;
    }
    else
    {
        getWorld()->addtoBank(5);
        m_coins -= 5;
    }
}

Bad::Bad(int id, int x, int y, StudentWorld* world)
    : Move(id, x, y, world), m_pauseCounter(180), m_impacted(false) {}
Bowser::Bowser(int id, int x, int y, StudentWorld* world)
    : Bad(id, x, y, world) {}

Boo::Boo(int id , int x, int y, StudentWorld* world)
    : Bad(id, x, y, world) {}

void Bowser::doSomething()
{
    Player* p[2] = {getWorld()->getPlayer1(), getWorld()->getPlayer2()};

    for (int i = 0; i < 2; i++)
    {
        if (!isWalking())
        {
            if (getX() == p[i]->getX() and getY() == p[i]->getY() and !p[i]->isWalking() and !p[i]->getTouchedBower())
            {
                if (randInt(1, 2) == 1)
                {
                    p[i]->minusCoins(p[i]->getCoins());
                    p[i]->minusStars(p[i]->getStars());
                    getWorld()->playSound(SOUND_BOWSER_ACTIVATE);
                    p[i]->setTouchedBowser(true);
                }
            }
        }
    }
    minuspauseCounter();
    if (getpauseCounter() == 0)
    {
        setTicks(randInt(1, 10) * 8);
        findrandomLegalDirection();
    }
    if(isWalking())
    {
        findrandomValidForkDirection();
        findValidDirectionforBowserandBoo();
        moveAtAngle(getmoveDirection(), 2);
        setTicks(getTicks() - 1);
        if(getTicks() == 0)
        {
            setWalking(false);
            setpauseCounter(180);
            if(randInt(1,4) == 1)
            {
                getWorld()->addDroppingSquare(getX(), getY());
                getWorld()->playSound(SOUND_DROPPING_SQUARE_CREATED);
            }   
        }
    }
}

void Boo::doSomething()
{
        Player* p[2] = {getWorld()->getPlayer1(), getWorld()->getPlayer2()};
        for(int i = 0; i < 2; i++)
        {
        if(!isWalking())
        {
            if(getX() == p[i]->getX() and getY() == p[i]->getY() and !p[i]->isWalking() and !p[i]->getTouchedBoo())
            {
                if(randInt(1, 2) == 1)
                {

                    int tempCoins = p[i]->getCoins();
                    p[i]->setCoins(p[1-i]->getCoins());
                    p[1-i]->setCoins(tempCoins);
                    p[i]->setTouchedBoo(true);
                    getWorld()->playSound(SOUND_BOO_ACTIVATE);
                }
                else
                {
                    int tempStars = p[i]->getStars();
                    p[i]->setStars(p[1-i]->getStars());
                    p[1-i]->setStars(tempStars);
                    p[i]->setTouchedBoo(true);
                    getWorld()->playSound(SOUND_BOO_ACTIVATE);
                }
            }
        }
    }
    minuspauseCounter();
    if (getpauseCounter() == 0)
    {
        setTicks(randInt(1, 3) * 8);
        findrandomLegalDirection();
    }
    if(isWalking())
    {
        findrandomValidForkDirection();
        findValidDirectionforBowserandBoo();
        moveAtAngle(getmoveDirection(), 2);
        setTicks(getTicks() - 1);
        if(getTicks() == 0)
        {
            setWalking(false);
            setpauseCounter(180); 
        }
    }  
}

Vortex::Vortex(int id, int x, int y, int direction, StudentWorld* world)
    : Move(id, x, y, world), m_direction(direction), m_active(true) 
    {
        setWalking(true);
    }

void Vortex::doSomething()
{
    if(isActive() == false)
        return;
    else
    {
        if(getX() < 0 or getX() >= VIEW_WIDTH or getY() < 0 or getY() >= VIEW_HEIGHT)
        {
            setActive(false);
            // getWorld()->deleteVortex(getX(), getY());
        }
        moveAtAngle(getVortexDirection(), 2);
        if(getWorld()->isHit(getX(), getY())) //and getWorld()->isBowser(getX(), getY()))
        {
            getWorld()->randomTeleport(getWorld()->getBowser(getX(), getY()));
            setActive(false);
            getWorld()->playSound(SOUND_HIT_BY_VORTEX);
            // getWorld()->deleteVortex(getX(), getY());
        }
        else if(getWorld()->isHit(getX(), getY())) //and getWorld()->isBoo(getX(), getY()))
        {
            getWorld()->randomTeleport(getWorld()->getBoo(getX(), getY()));
            setActive(false);
            getWorld()->playSound(SOUND_HIT_BY_VORTEX);
            // getWorld()->deleteVortex(getX(), getY());
        }
    }
}

CoinSquare::CoinSquare(int id, int x, int y, bool color, StudentWorld* world)
    : Actor(id, x, y, 0, 1, 1, world), m_blue(color) {}

void CoinSquare::doSomething()
{

    Player* p[2] = {getWorld()->getPlayer1(), getWorld()->getPlayer2()};

    for(int i = 0; i < 2; i++)
    {
    //if a player is on the coin square
    if(getX() == p[i]->getX() and getY() == p[i]->getY())
        {
            //if lands on blue coin square
            if(!p[i]->isWalking() and !p[i]->getLanded() and isBlue())
                {
                    p[i]->addCoins(3); 
                    getWorld()->playSound(SOUND_GIVE_COIN);
                    p[i]->setLanded(true);
                }
            //if lands on red
            else if(!p[i]->isWalking() and !p[i]->getLanded() and !isBlue())
                {
                    p[i]->minusCoins(3);
                    getWorld()->playSound(SOUND_TAKE_COIN);
                    p[i]->setLanded(true);
                }
        }
    //if a player is walking, then set that player to not stationary
    if(p[i]->isWalking() == true)
    {
        p[i]->setLanded(false);
    }
    }

}

StarSquare::StarSquare(int id, int x, int y, StudentWorld* world)
    : Actor(id, x, y, 0, 1, 1, world)
{
}

void StarSquare::doSomething()
{
    Player* p[2] = {getWorld()->getPlayer1(), getWorld()->getPlayer2()};

    for(int i = 0; i < 2; i++)
    {
    //if a player is on the coin square
    if(getX() == p[i]->getX() and getY() == p[i]->getY())
        {
            if(!p[i]->getLanded() and !p[i]->isWalking())
                {
                    p[i]->addStar();
                    p[i]->setLanded(true);
                    getWorld()->playSound(SOUND_GIVE_STAR);
                }
        }  
        if(p[i]->isWalking() == true)
        {
            p[i]->setLanded(false);
        }
    }

}

BankSquare::BankSquare(int id, int x, int y, StudentWorld* world)
    : Actor(id, x, y, 0, 1, 1, world)
{

}

void BankSquare::doSomething()
{
    Player* p[2] = {getWorld()->getPlayer1(), getWorld()->getPlayer2()};

    for(int i = 0; i < 2; i++)
    {
    if(getX() == p[i]->getX() and getY() == p[i]->getY())
        {
            if(!p[i]->getLanded() and p[i]->isWalking())
                {
                    p[i]->givetoBank();
                    getWorld()->playSound(SOUND_DEPOSIT_BANK);
                    p[i]->setLanded(true);
                }
            else if(!p[i]->getLanded() and !p[i]->isWalking())
                {
                    p[i]->addCoins(getWorld()->giveBanktoPlayer());
                    getWorld()->playSound(SOUND_WITHDRAW_BANK);
                    p[i]->setLanded(true);
                }
        }  
        if(p[i]->isWalking() == true)
        {
            p[i]->setLanded(false);
        }
    }
}


EventSquare::EventSquare(int id, int x, int y, StudentWorld* world)
    : Actor(id, x, y, 0, 1, 1, world) {}

void EventSquare::doSomething()
{
Player* p[2] = {getWorld()->getPlayer1(), getWorld()->getPlayer2()};

for (int i = 0; i < 2; i++)
{
    int option = randInt(1, 3);
    // int option = 1;
    if (getX() == p[i]->getX() and getY() == p[i]->getY())
    {
        //give player vortex
        if (option == 1 and !p[i]->isWalking() and !p[i]->getLanded())
        {
            p[i]->setVortex(true);
            // cout << "gave player " << i + 1 << " a vortex" << endl;
            p[i]->setLanded(true);
            getWorld()->playSound(SOUND_GIVE_VORTEX);
        }
        //swap locations with other player
        else if (option == 2 and !p[i]->isWalking() and !p[i]->getLanded())
        {
            // cout << "player " << i + 1 << " swapped with player " << 3 - i << endl;
            getWorld()->swapPlayers(p[0], p[1]);
            p[i]->setLanded(true);
            getWorld()->playSound(SOUND_PLAYER_TELEPORT);
        }
        //teleport player to random square
        else if (option == 3 and !p[i]->isWalking() and !p[i]->getLanded())
        {
            getWorld()->randomTeleport(p[i]);
            getWorld()->playSound(SOUND_PLAYER_TELEPORT);
        }
        }
        //if a player is walking, then set that player to not stationary
        if (p[i]->isWalking() == true)
        {
            p[i]->setLanded(false);
        }
    }
}

DirectionSquare::DirectionSquare(int id, int x, int y, int direction, StudentWorld* world)
    : Actor(id, x, y, direction, 1, 1, world)
{
}

void DirectionSquare::doSomething()
{
    Player* p[2] = {getWorld()->getPlayer1(), getWorld()->getPlayer2()};
    for(int i = 0; i < 2; i++)
    {
        if(getX() == p[i]->getX() and getY() == p[i]->getY())
        {
            if(!p[i]->getLanded() and p[i]->isWalking())
            {
                p[i]->changeDirection(getDirection());
                p[i]->setLanded(true);
            }
            else if(!p[i]->isWalking() and !p[i]->getLanded())
            {
                p[i]->changeDirection(getDirection());
                p[i]->setLanded(true);
            }
        }
        //if a player is walking, then set that player to not stationary
        if(p[i]->isWalking() == true)
        {
            p[i]->setLanded(false);
        }
    }
}

DroppingSquare::DroppingSquare(int id, int x, int y, StudentWorld* world)
    : Actor(id, x, y, 0, 1, 1, world)
{
}

void DroppingSquare::doSomething()
{
    Player* p[2] = {getWorld()->getPlayer1(), getWorld()->getPlayer2()};
    for(int i = 0; i < 2; i++)
    {
        int option = randInt(1, 2);
        if(option == 1 and getX() == p[i]->getX() and getY() == p[i]->getY())
        {
            if(!p[i]->isWalking() and !p[i]->getLanded())
            {
                p[i]->minusCoins(10);
                getWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
                p[i]->setLanded(true);
            }
        }
        else if(option == 2 and getX() == p[i]->getX() and getY() == p[i]->getY())
        {
            if(!p[i]->isWalking() and !p[i]->getLanded())
            {
                p[i]->minusStars(1);
                getWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
                p[i]->setLanded(true);
            }
        }
    }
}

void Move::findValidDirection()
{
    if(getmoveDirection() == right and !getWorld()->checkRight(getX(), getY()) and !getWorld()->onDirectionSquare(getX(), getY()))
        {
            if(getWorld()->checkUp(getX(), getY()))
            {
                setmoveDirection(up);               
                setDirection(right);
            }
            else if(getWorld()->checkDown(getX(), getY()))
            {
                setmoveDirection(down);
                setDirection(right);
            }
        }
        else if(getmoveDirection() == up and !getWorld()->checkUp(getX(), getY()) and !getWorld()->onDirectionSquare(getX(), getY()))
        {
            if(getWorld()->checkRight(getX(), getY()))
            {
                setmoveDirection(right);
                setDirection(right);
            }
            else if(getWorld()->checkLeft(getX(), getY()))
            {
                setmoveDirection(left);
                setDirection(left);
            }
        }
        else if(getmoveDirection() == left and !getWorld()->checkLeft(getX(), getY()) and !getWorld()->onDirectionSquare(getX(), getY()))
        {
            if(getWorld()->checkUp(getX(), getY()))
            {
                setmoveDirection(up);
                setDirection(right);
            }
            else if(getWorld()->checkDown(getX(), getY()))
            {
                setmoveDirection(down);
                setDirection(right);
            }
        }
        else if(getmoveDirection() == down and !getWorld()->checkDown(getX(), getY()) and !getWorld()->onDirectionSquare(getX(), getY()))
        {
            if(getWorld()->checkRight(getX(), getY()))
            {
                setmoveDirection(right);
                setDirection(right);
            }
            else if(getWorld()->checkLeft(getX(), getY()))
            {
                setmoveDirection(left);
                setDirection(left);
            }
        }
}

void Move::findValidDirectionforBowserandBoo()
{
    if(getmoveDirection() == right and !getWorld()->checkRight(getX(), getY()))
        {
            if(getWorld()->checkUp(getX(), getY()))
            {
                setmoveDirection(up);               
                setDirection(right);
            }
            else if(getWorld()->checkDown(getX(), getY()))
            {
                setmoveDirection(down);
                setDirection(right);
            }
        }
        else if(getmoveDirection() == up and !getWorld()->checkUp(getX(), getY()))
        {
            if(getWorld()->checkRight(getX(), getY()))
            {
                setmoveDirection(right);
                setDirection(right);
            }
            else if(getWorld()->checkLeft(getX(), getY()))
            {
                setmoveDirection(left);
                setDirection(left);
            }
        }
        else if(getmoveDirection() == left and !getWorld()->checkLeft(getX(), getY()))
        {
            if(getWorld()->checkUp(getX(), getY()))
            {
                setmoveDirection(up);
                setDirection(right);
            }
            else if(getWorld()->checkDown(getX(), getY()))
            {
                setmoveDirection(down);
                setDirection(right);
            }
        }
        else if(getmoveDirection() == down and !getWorld()->checkDown(getX(), getY()))
        {
            if(getWorld()->checkRight(getX(), getY()))
            {
                setmoveDirection(right);
                setDirection(right);
            }
            else if(getWorld()->checkLeft(getX(), getY()))
            {
                setmoveDirection(left);
                setDirection(left);
            }
        }
}
void Move::findrandomValidForkDirection()
{
        if(getWorld()->isForkforBowserandBoo(getX(), getY(), getmoveDirection()))
        {
            vector<int> Directions;
            if(getWorld()->checkRight(getX(), getY()))
            {
                Directions.push_back(0);
            }
            if(getWorld()->checkLeft(getX(), getY()))
            {
                Directions.push_back(180);
            }
            if(getWorld()->checkUp(getX(), getY()))
            {
                Directions.push_back(90);
            }
            if(getWorld()->checkDown(getX(), getY()))
            {
                Directions.push_back(270);
            }
            int randIndex = randInt(0, Directions.size() - 1);
            setmoveDirection(Directions[randIndex]);
            setDirection((Directions[randIndex] == 180) ? left : right);
        }
}
void Move::findrandomLegalDirection()
{
    vector<int> validDirections;
    if (getWorld()->checkRight(getX(), getY())) {
        validDirections.push_back(0);
    }
    if (getWorld()->checkLeft(getX(), getY())) {
        validDirections.push_back(180);
    }
    if (getWorld()->checkUp(getX(), getY())) {
        validDirections.push_back(90);
    }
    if (getWorld()->checkDown(getX(), getY())) {
        validDirections.push_back(270);
    }
    if (!validDirections.empty()) 
    {
        int randIndex = randInt(0, validDirections.size()-1);
        setmoveDirection(validDirections[randIndex]);
        setDirection((validDirections[randIndex] == 180) ? left : right);
        setWalking(true);
    }
}