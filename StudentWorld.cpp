#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"

#include <string>

#include <sstream>



using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), m_bankCoins(0), m_clean(false)
{
}

int StudentWorld::init()
{
    Board bd;
    string board_file = assetPath() + "board0" + to_string(getBoardNumber()) + ".txt";
    Board::LoadResult result = bd.loadBoard(board_file);
    if (result == Board::load_fail_file_not_found)
        cerr << "Could not find board01.txt data file\n";
    else if (result == Board::load_fail_bad_format)
        cerr << "Your board was improperly formatted\n";
    else if (result == Board::load_success)
    {
        cerr << "Successfully loaded board\n";
        for (int x = 0; x < 16; x++)
        {
            for (int y = 0; y < 16; y++)
            {
                Board::GridEntry ge = bd.getContentsOf(x, y);
                switch (ge) {
                case Board::empty:
                    break;
                case Board::boo:
                    m_actors.push_back(new CoinSquare(IID_BLUE_COIN_SQUARE, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, true, this));
                    m_actors.push_back(new Boo(IID_BOO, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, this));
                    break;
                case Board::bowser:
                    m_actors.push_back(new CoinSquare(IID_BLUE_COIN_SQUARE, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, true, this));
                    m_actors.push_back(new Bowser(IID_BOWSER, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, this));
                    break;
                case Board::player:
                    m_players[0] = new Player(IID_PEACH, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, 1, this);
                    m_players[1] = new Player(IID_YOSHI, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, 2, this);
                    m_actors.push_back(new CoinSquare(IID_BLUE_COIN_SQUARE, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, true, this));
                    break;
                case Board::red_coin_square:
                    m_actors.push_back(new CoinSquare(IID_RED_COIN_SQUARE, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, false, this));
                    break;
                case Board::blue_coin_square:
                    m_actors.push_back(new CoinSquare(IID_BLUE_COIN_SQUARE, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, true, this));
                    break;
                case Board::star_square:
                    m_actors.push_back(new StarSquare(IID_STAR_SQUARE, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, this));
                    break;
                case Board::bank_square:
                    m_actors.push_back(new BankSquare(IID_BANK_SQUARE, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, this));
                    break;
                case Board::event_square:
                    m_actors.push_back(new EventSquare(IID_EVENT_SQUARE, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, this));
                    break;
                case Board::left_dir_square:
                    m_actors.push_back(new DirectionSquare(IID_DIR_SQUARE, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, 180, this));
                    break;
                case Board::right_dir_square:
                    m_actors.push_back(new DirectionSquare(IID_DIR_SQUARE, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, 0, this));
                    break;
                case Board::up_dir_square:
                    m_actors.push_back(new DirectionSquare(IID_DIR_SQUARE, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, 90, this));
                    break;
                case Board::down_dir_square:
                    m_actors.push_back(new DirectionSquare(IID_DIR_SQUARE, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, 270, this));
                    break;
                }
            }
        }
        startCountdownTimer(99);   
        return GWSTATUS_CONTINUE_GAME;
    }
        return GWSTATUS_NOT_IMPLEMENTED;
}

int StudentWorld::move()
{
    if(timeRemaining() < 0)
    {
        if(m_players[0]->getStars() == m_players[1]->getStars() and m_players[0]->getCoins() == m_players[1]->getCoins())
            {
                int winner = randInt(1,2);
                if(winner == 1)
                {
                    setFinalScore(m_players[0]->getStars(), m_players[0]->getCoins());
                    playSound(SOUND_GAME_FINISHED);
                    return GWSTATUS_PEACH_WON;
                }
                else    
                {
                    setFinalScore(m_players[1]->getStars(), m_players[1]->getCoins());
                    playSound(SOUND_GAME_FINISHED);
                    return GWSTATUS_YOSHI_WON;
                }
            }

        int winnerCoins = 0;
        int winnerStars = 0;
        bool peachWinner;

        if(m_players[0]->getStars() > m_players[1]->getStars())
        {
            winnerStars = m_players[0]->getStars();
            winnerCoins = m_players[0]->getCoins();
            peachWinner = true;
        }
        else if(m_players[1]->getStars() > m_players[1]->getStars())    
        {
            winnerStars = m_players[1]->getStars();
            winnerCoins = m_players[1]->getCoins();
            peachWinner = false;
        }
        else if(m_players[0]->getStars() == m_players[1]->getStars())
        {
            if(m_players[0]->getCoins() > m_players[1]->getCoins())
                {
                    winnerCoins = m_players[0] ->getCoins();
                    winnerStars = m_players[0] ->getStars();
                    peachWinner = true;
                }
            else if(m_players[1]->getCoins() > m_players[1]->getStars())
                {
                    winnerCoins = m_players[1] -> getCoins();
                    winnerStars = m_players[1] -> getStars();
                    peachWinner = false;
                }
        }

        
        setFinalScore(winnerStars, winnerCoins);
        if(peachWinner == true)
        {
            playSound(SOUND_GAME_FINISHED);
            return GWSTATUS_PEACH_WON;
        }
        else if(peachWinner == false)
        {
            playSound(SOUND_GAME_FINISHED);
            return GWSTATUS_YOSHI_WON;
        }

    }

    m_players[0]->doSomething();
    m_players[1]->doSomething();

    for(auto actor : m_actors)
    {
        actor->doSomething();
        if(actor->isVortex() and !actor->isActive())
            deleteVortex(actor->getX(), actor->getY());
    }


    ostringstream oss;
    oss << "P1 Roll: " << m_players[0] -> getRolls() << " Stars: " << m_players[0]->getStars() << " $$: " << m_players[0]->getCoins() << (m_players[0]->getVortex() ? " VOR "
    : "") <<  " | Time: " << timeRemaining() << " | Bank: " << getBankCoins() 
    << " | P2 Roll: " << m_players[1] -> getRolls() << " Stars: " << m_players[1] -> getStars() << " $$: " 
    << m_players[1] -> getCoins() << (m_players[1]->getVortex() ? " VOR " : "");
    setGameStatText(oss.str());

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{      
    // Delete all players
    for (auto player : m_players) {
        delete player;
    }
    
    // Delete all actors
    for (auto actor : m_actors) {
        delete actor;
    }

    m_clean = true;

}

StudentWorld::~StudentWorld()
{
    if(!m_clean)
        cleanUp();
}


int StudentWorld::getNumberOfActors() const
{
    return m_actors.size();
}

Player* StudentWorld::getPlayer1() const
{
    return m_players[0];
}

Player* StudentWorld::getPlayer2() const
{
    return m_players[1];
}

Actor* StudentWorld::getBowser(int x, int y)
{
    for(auto actor : m_actors)
    {
        if(actor->isBowser() and actor->getX() == x and actor->getY() == y)
            return actor;
    }
    return nullptr;
}

Actor* StudentWorld::getBoo(int x, int y)
{
    for(auto actor : m_actors)
    {
        if(actor->isBoo() and actor->getX() == x and actor->getY() == y)
            return actor;
    }
    return nullptr;
}

bool StudentWorld::isBowser(int x, int y)
{
    for(auto actor : m_actors)
    {
        if(actor->isBowser())
            if(actor->getX() == x and actor->getY() == y)
                return true;
    }
    return false;
}

bool StudentWorld::isBoo(int x, int y)
{
    for(auto actor : m_actors)
    {
        if(actor->isBoo())
            if(actor->getX() == x and actor->getY() == y)
                return true;
    }
    return false;
}


int StudentWorld::giveBanktoPlayer()
{
    int temp = m_bankCoins;
    m_bankCoins = 0;
    return temp;

}

bool StudentWorld::onDirectionSquare(int x, int y) const
{

    for (auto actor : m_actors)
    {
        if(actor->isDirectionSquare() and actor->getX() == x and actor->getY() == y)
            return true;
    }
    return false;
}

bool StudentWorld::onSquare(int x, int y) const
{
    for(auto actor : m_actors)
    {
        if(actor->isSquare() and actor->getX() == x and actor->getY() == y)
            return true;
    }
    return false;
}

//false indicates that it is empty and cannot be moved onto
//true indicates that something is there and be moved onto
 
bool StudentWorld::checkRight(int x, int y) const
{
    // Check if the given coordinates are out of bounds
    if (x < 0 || x >= VIEW_WIDTH || y < 0 || y >= VIEW_HEIGHT) {
        return false; // Coordinates are out of bounds, so not empty
    }

    // Iterate over all game objects and check if any of them occupy the given coordinates
    for (auto actor : m_actors) {
        if (actor->getX() == x + SPRITE_WIDTH and actor->getY() == y and actor->isSquare())
        {
            return true; // Object occupies the coordinates, so not empty
        }
    }

    // No game objects occupy the coordinates, so they are empty
    return false;
}

bool StudentWorld::checkLeft(int x, int y) const
{
    // Check if the given coordinates are out of bounds
    if (x < 0 || x >= VIEW_WIDTH || y < 0 || y >= VIEW_HEIGHT) {
        return false; // Coordinates are out of bounds, so not empty
    }

    // Iterate over all game objects and check if any of them occupy the given coordinates
    for (auto actor : m_actors) {
        if (actor->getX() == x - SPRITE_WIDTH and actor->getY() == y and actor->isSquare())
        {
            return true; // Object occupies the coordinates, so not empty
        }
    }

    // No game objects occupy the coordinates, so they are empty
    return false;
}

bool StudentWorld::checkUp(int x, int y) const
{
    // Check if the given coordinates are out of bounds
    if (x < 0 || x >= VIEW_WIDTH || y + SPRITE_HEIGHT >= VIEW_HEIGHT) {
        return false; // Coordinates are out of bounds or at top row, so not empty
    }

    // Iterate over all game objects and check if any of them occupy the coordinate above
    for (auto actor : m_actors) {
        if (actor->getX() == x and actor->getY() == y + SPRITE_HEIGHT and actor->isSquare())
        {
            return true; //true means not empty
        }
    }

    // false means empty
    return false;
}

bool StudentWorld::checkDown(int x, int y) const
{
    // Check if the given coordinates are out of bounds
    if (x < 0 || x >= VIEW_WIDTH || y - SPRITE_HEIGHT < 0) {
        return false; // Coordinates are out of bounds or at bottom row, so not empty
    }

    // Iterate over all game objects and check if any of them occupy the coordinate below
    for (auto actor : m_actors) {
        if (actor->getX() == x and actor->getY() == y - SPRITE_HEIGHT and actor->isSquare())
        {
            return true; // Object occupies the coordinate below, so not empty
        }
    }

    // No game objects occupy the coordinate below,
    return false;
}

bool StudentWorld::isFork(int x, int y, int direction) const 
{
    // Check if the given cell has more than two passages
    int numPassages = 0;
    if (checkUp(x, y) and direction != 270 and !onDirectionSquare(x, y))
    {
        numPassages++;
    }
    if (checkDown(x, y) and direction != 90 and !onDirectionSquare(x, y)) 
    {
        numPassages++;
    }
    if (checkLeft(x, y) and direction != 0 and !onDirectionSquare(x, y)) 
    {
        numPassages++;
    }
    if (checkRight(x, y) and direction != 180 and !onDirectionSquare(x, y)) 
    {
        numPassages++;
    }
    return numPassages > 1;
}

bool StudentWorld::isForkforBowserandBoo(int x, int y, int direction) const
{
    // Check if the given cell has more than two passages
    int numPassages = 0;
    if (checkUp(x, y) and direction != 270)
    {
        numPassages++;
    }
    if (checkDown(x, y) and direction != 90) 
    {
        numPassages++;
    }
    if (checkLeft(x, y) and direction != 0) 
    {
        numPassages++;
    }
    if (checkRight(x, y) and direction != 180) 
    {
        numPassages++;
    }
    return numPassages > 1;
}

void StudentWorld::addDroppingSquare(int x, int y)
{
    for(int i = 0; i < m_actors.size(); i++)
    {
        if(m_actors[i]->getX() == x and m_actors[i]->getY() == y and m_actors[i]->isSquare())
        {
            Actor* temp = m_actors[i];
            m_actors.erase(m_actors.begin() + i);
            delete temp;
            m_actors.push_back(new DroppingSquare(IID_DROPPING_SQUARE, x, y, this));
        }
    }
}

void StudentWorld::addVortex(int x, int y, int direction)
{
    m_actors.push_back(new Vortex(IID_VORTEX, x, y, direction, this));
}

void StudentWorld::deleteVortex(int x, int y)
{
    for(int i = 0; i < m_actors.size(); i++)
    {
        if(m_actors[i]->getX() == x and m_actors[i]->getY() == y and m_actors[i]->isVortex())
        {
            Actor* temp = m_actors[i];
            m_actors.erase(m_actors.begin() + i);
            delete temp;
        }
    }
}

bool StudentWorld::isHit(int vorX, int vorY)
{
    for(auto actor : m_actors)
    {
        // if((actor->isBowser() or actor->isBoo()) and actor->getX() == vorX and actor->getY() == vorY) 
        if((actor->isBowser() or actor->isBoo()) 
        and actor->getX() - vorX < 16 
        and actor->getY() - vorY < 16)
            {
                return true;
            }
    }
    return false;
}


void StudentWorld::randomTeleport(Actor* thing)
{
    for(int i = 0; i < 16; i++)
    {
        for(int j = 0; j < 16; j++)
        {
        int teleportX = randInt(i, j) * SPRITE_WIDTH;
        int teleportY = randInt(i, j) * SPRITE_HEIGHT;
        if (onSquare(teleportX, teleportY))
        {
            thing->moveTo(teleportX, teleportY);
            thing->setLanded(true);
        }
        }
    }
}

void StudentWorld::swapPlayers(Player* p1, Player* p2)
{
    int tempX = p1->getX();
    int tempY = p1->getY();
    int tempMoveDirection = p1->getmoveDirection();
    int tempSpriteDirection = p1->getDirection();
    bool tempWalkingState = p1->isWalking();
    int tempTicks = p1->getTicks();

    p1->moveTo(p2->getX(), p2->getY());
    p1->setmoveDirection(p2->getmoveDirection());
    p1->setDirection(p2->getDirection());
    p1->setWalking(p2->isWalking());
    p1->setTicks(p2->getTicks());

    p2->moveTo(tempX, tempY);
    p2->setmoveDirection(tempMoveDirection);
    p2->setDirection(tempSpriteDirection);
    p2->setWalking(tempWalkingState);
    p2->setTicks(tempTicks);

    p1->setLanded(true);
}






