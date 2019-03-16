#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>

struct Vector2D
{
    float x;
    float y;
    
    Vector2D(void) {
        x = 0;
        y = 0;
    }
    
    Vector2D(float x, float y) {
        this->x = x;
        this->y = y;
    }
    
    Vector2D &operator=(Vector2D const& other) {
        x = other.x;
        y = other.y;
    }
};

struct Checkpoint
{
    Vector2D pos;
};

struct Opponent
{
    Opponent(int id) {
        this->id = id;
        laps = 0;
        nextCheckpointID = 1;
    }
    
    int id;
    Vector2D position;
    Vector2D velocity;
    int angle;
    int nextCheckpointID;
    int laps;
};

int getDistance(int ax, int ay, int bx, int by) {
    return sqrt(pow(ax - bx, 2) + pow(ay - by, 2));
}

class Player
{
public:
    Player(int id) : id_(id) {
        boostUsed_ = false;
        previousPos_.x = -1;
        previousPos_.y = -1;
        nextCheckpointID_ = 1;
        
        shield_ = false;
        shieldCpt_ = 0;
        
        laps_ = 0;
    }
    
    float distance2(Vector2D const& p1, Vector2D const& p) {
        return (p1.x - p.x)*(p1.x - p.x) + (p1.y - p.y)*(p1.y - p.y);
    }

    float distance(Vector2D const& p1, Vector2D const& p) {
        return std::sqrt(distance2(p1, p));
    }
    
    float getAngle(Vector2D const& p) {
        float d = distance(position_, p);
        float dx = (p.x - position_.x) / d;
        float dy = (p.y - position_.y) / d;
        float a = std::acos(dx) * 180.0 / M_PI;
    
        if (dy < 0) {
            a = 360.0 - a;
        }
    
        return a;
    }
    
    float diffAngle(Vector2D const& p) {
        float a = getAngle(p);
    
        float right = (angle_ <= a) ? a - angle_ : 360.0 - angle_ + a;
        float left = (angle_ >= a) ? angle_ - a : angle_ + 360.0 - a;
    
        if (right < left) {
            return right;
        }
        return -left;
    }
    
    Opponent *collision(std::vector<Opponent *> const& opponents) {
        for (auto it : opponents) {
            int dx = (it->position.x) - (position_.x);
            int dy = (it->position.y) - (position_.y);
            int radii = 401 + 401;
            if ( ( dx * dx )  + ( dy * dy ) < radii * radii ) {
                return it;
            }
        }
        return nullptr;
    }
    
    Checkpoint *getNextCheckpoint(std::vector<Checkpoint *> const& checkpoints) {
        if (nextCheckpointID_ + 1 < checkpoints.size()) {
            nextCheckpointID_++;
            return checkpoints.at(nextCheckpointID_);
        }
        nextCheckpointID_ = 0;
        return checkpoints.at(0);
    }
    
    void play(std::vector<Player *> const& players, std::vector<Checkpoint *> const& checkpoints, std::vector<Opponent *> const& opponents, int laps) {    
        //if (id_ == 0) {
            playPod1(players, checkpoints, opponents, laps);
        /*} else if (id_ == 1) {
            playPod2(players, checkpoints, opponents, laps);
        }*/
    }
    
    void onChangeCheckpoint(int angle) {
        std::cerr << "on change checkpoint angle: " << angle << std::endl;
    }
    
    void setPosition(Vector2D const& position) {
        position_ = position;
    }
    
    void setVelocity(Vector2D const& velocity) {
        velocity_ = velocity;
    }
    
    void setAngle(float angle) {
        angle_ = angle;   
    }
    
    void setNextCheckpointID(int id) {
        if (nextCheckpointID_ != id) {
            laps_++;   
        }
        nextCheckpointID_ = id;
    }
    
    void setThrust(int thrust) {
        thrust_ = thrust;   
    }
    
    void playPod1(std::vector<Player *> const& players, std::vector<Checkpoint *> const& checkpoints, std::vector<Opponent *> const& opponents, int laps) {
        Checkpoint next = *checkpoints.at(nextCheckpointID_);
        
        /*Opponent *collisioned = nullptr;
        if ((collisioned = collision(opponents))) {
            //int d1 = distance(position_, velocity_);
            //int d2 = distance(collisioned->position, collisioned->velocity);
            int v = (collisioned->position.x - collisioned->velocity.x + collisioned->velocity.y - collisioned->position.y);
            std::cerr << "v: " << v << std::endl;
            if (shieldCpt_ == 0 && v > 120) {
                std::cerr << "#" << id_ + 1 << " collisionned" << std::endl;
                shield_ = true;
            } else {
                shield_ = false;   
            }
            std::cerr << "#" << id_ + 1 << " collision !!!" << std::endl;   
        } else {
            shield_ = false;   
        }
        if (shieldCpt_ > 0) {
            --shieldCpt_;   
        }*/
        
        std::cerr << "#" << id_ << " laps: " << laps_ << std::endl;
        
        bool boost = false;
        Vector2D nextPos = next.pos;
        int difAngle = diffAngle(next.pos);

        Vector2D lastDistance;
        lastDistance.x = previousPos_.x - position_.x;
        lastDistance.y = previousPos_.y - position_.y;

        int distance = getDistance(position_.x, position_.y, next.pos.x, next.pos.y);
        
        std::cerr << "distance: " << distance << std::endl;
        //std::cerr << "diffAngle: " << difAngle << std::endl;
     
        thrust_ = 100;
        
        if (!boostUsed_) {
            if (distance > 4000 /*&& distance <= 5000*/) {
                int tmp = nextCheckpointID_;
                Checkpoint *chk1 = getNextCheckpoint(checkpoints);
                Checkpoint *chk2 = getNextCheckpoint(checkpoints);
                nextCheckpointID_ = tmp;
                
                float tmpAngle = diffAngle(next.pos);
                
                std::cerr << "tmpAngle: " << tmpAngle << std::endl;
                if (tmpAngle > -45 && tmpAngle < 45) {
                    boostUsed_ = true;
                    boost = true;
                }
            }
            /*else if (distance > 5000) {
                boostUsed_ = true;
                boost = true;
            }*/
        }

        if (previousPos_.x > -1) {
            nextPos.x += (lastDistance.x * 3);
            nextPos.y += (lastDistance.y * 3);
        }
        
        //std::cerr << "diffAngle: " << difAngle << " angle: " << angle_ << std::endl;
        if (angle_ > -1) {
             if (difAngle < -120 || difAngle > 120) {
                thrust_ = 0;
            } else if (difAngle < -90 || difAngle > 90) {
                thrust_ = 70;
            } else if (difAngle < -45 || difAngle > 45) {
                thrust_ = 100;
            }
        }
        if (id_ == 1 && angle_ == -1 && !boostUsed_) {
            boost = true;
            boostUsed_ = true;
        }
        
        //Vector2D v(position_.x + velocity_.x * 4.f, position_.y + velocity_.y * 4.f);
        Vector2D v(position_.x + velocity_.x * 4.f * 0.85f, position_.y + velocity_.y * 4.f * 0.85f);
        //float distV = distance(v);
        //if (std::pow(nextX - position_.x, 2) + std::pow(nextY - position_.y, 2) < std::pow(600, 2)) {
        if (std::pow(v.x - next.pos.x, 2) + std::pow(v.y - next.pos.y, 2) < std::pow(600, 2)) {
            std::cerr << "decelere" << std::endl;
            int tmpC = nextCheckpointID_;
            Checkpoint *chkNext = getNextCheckpoint(checkpoints);
            nextPos.x = chkNext->pos.x + lastDistance.x * 3;
            nextPos.y = chkNext->pos.y + lastDistance.y * 3;
            nextCheckpointID_ = tmpC;
            
            float tmp = diffAngle(chkNext->pos);
            if (tmp < -120 || tmp > 120) {
                thrust_ = 0;
            } else if (tmp < -90 || tmp > 90) {
                thrust_ = 70;
            } else /*if (tmp < -45 || tmp > 45)*/ {
                thrust_ = 100;
            }
        }
        
        if (laps == 3 && nextCheckpointID_ == checkpoints.size()) {
            thrust_ = 100;
        }
        
        previousPos_.x = position_.x;
        previousPos_.y = position_.y;
        std::cout << nextPos.x << " " << nextPos.y << " ";
        if (boost) {
            std::cout << "BOOST" << std::endl;
        } else if (shield_) {
            std::cout << "SHIELD" << std::endl;  
        } else {
            std::cout << thrust_ << std::endl;
        }
    }
    
    /*void playPod2(std::vector<Player *> const& players, std::vector<Checkpoint *> const& checkpoints, std::vector<Opponent *> const& opponents, int laps) {
        Opponent *better = getBetterOpponent(players, opponents);
        if (better) {
            Checkpoint *chk = checkpoints.at(nextCheckpointID_);
            
            float distanceToBetter = distance(position_, better->position);
            float distanceToNext = distance(position_, chk->pos);
            float distanceToBNext = distance(better->position, chk->pos);
            
            if (distanceToNext < distanceToBNext) {
                std::cout << chk->pos.x << " " << chk->pos.y << " 100" << std::endl;
            } else {
                std::cout << better->position.x << " " << better->position.y << " 100" << std::endl;   
            }
            // better->pos;
            // better->velocity
            
            std::cout << "0" << " 0 " << " 100" << std::endl;   
        } else {
            playPod1(players, checkpoints, opponents, laps);
        }
    }
    
    Opponent *getBetterOpponent(std::vector<Player *> const& players, std::vector<Opponent *> const& opponents) const {
        int nmate = (id_ == 0) ? 1 : 0;
        Player *mate = players.at(nmate);
        
        for (auto it : opponents) {
            if (it->laps >= mate->getLaps()) {
                std::cerr << it->id << " is better than " << nmate << std::endl;
                return it;   
            }
        }
        return nullptr;
    }*/
    
    int getLaps(void) const {
        return laps_;   
    }
    
private:
    int id_;

    Vector2D position_;
    Vector2D previousPos_;
    Vector2D velocity_;
    float angle_;
    int nextCheckpointID_;
        
    int thrust_;
    bool boostUsed_;
    int laps_;
    
    bool shield_;
    int shieldCpt_;
};

class Board
{
 public:
 
    void init(void) {
        int laps;
        int checkpointCount;
        
        std::cin >> laps;
        std::cin.ignore();
        
        std::cin >> checkpointCount;
        std::cin.ignore();
    
        for (int i = 0 ; i < checkpointCount ; i++) {
            Checkpoint *checkpoint = new Checkpoint;
        
            std::cin >> checkpoint->pos.x >> checkpoint->pos.y;
            std::cin.ignore();
            
            checkpoints_.push_back(checkpoint);    
        }
        
        players_.push_back(new Player(0));
        players_.push_back(new Player(1));
        
        opponents_.push_back(new Opponent(0));
        opponents_.push_back(new Opponent(1));
    }
 
    void refresh(void) {
        Vector2D position;
        Vector2D velocity;
        float angle;
        int nextCheckpointID;
          
        for (int i = 0 ; i < 2 ; i++) {
            Player *player = players_.at(i);
            
            std::cin >> position.x >> position.y >> velocity.x >> velocity.y >> angle >> nextCheckpointID;
            std::cin.ignore();
            
            player->setPosition(position);
            player->setVelocity(velocity);
            player->setAngle(angle);
            player->setNextCheckpointID(nextCheckpointID);
        }
        
        for (int i = 0 ; i < 2 ; i++) {
            Opponent *opponent = opponents_.at(i);
            
            std::cin >> position.x >> position.y >> velocity.x >> velocity.y >> angle >> nextCheckpointID;
            std::cin.ignore();
            
            opponent->position = position;
            opponent->velocity = velocity;
            opponent->angle = angle;
            
            if (opponent->nextCheckpointID != nextCheckpointID) {
                opponent->laps++;
                std::cerr << "o" << opponent->id << " " << opponent->laps << std::endl;
            }
            opponent->nextCheckpointID = nextCheckpointID;
        }
    }
    
    void play(void) {
        for (auto it : players_) {
            it->play(players_, checkpoints_, opponents_, laps_);   
        }
    }
 
 private:
    std::vector<Checkpoint *> checkpoints_;
    std::vector<Opponent *> opponents_;
    std::vector<Player *> players_;
    int laps_;
};

int main(void)
{
    Board board;
 
    board.init();
    while (1) {
        board.refresh();    
        board.play();
    }
}
