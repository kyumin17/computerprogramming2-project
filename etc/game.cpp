#include "game.hpp"

Game::Game() {
    weaponType = 1;
    player = new Player(65, 14, 3, 3, playerCharacter.front);
    bow = new Bow(player -> x, player -> y + 1, bowShape.bowDown);
    sword = new Sword(player -> x + 3, player -> y - 2, swordShape.swordRight[0]);
    eraser = new Eraser(player -> x - 2, player -> y - 1, eraserShape.eraserNonactive);
    enemyNum = 10;
    time = 0;
    score = 0;
    end = false;
}

int Game::getDirection(char ch) {
    switch(ch) {
        case 'a':
            return LEFT;
            break;
        case 'd':
            return RIGHT;
            break;
        case 'w':
            return BACK;
            break;
        case 's':
            return FRONT;
            break;
        default:
            return -1;
    }
}

void Game::createMap() {
    //랜덤
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> disPos(-500, 492);
    std::uniform_int_distribution<int> disLen(4, 8);
    std::uniform_int_distribution<int> disAttack(1, 3);

    //block 생성
    for (int i = 0; i < 2500; i++) {
        if (disAttack(gen) == 1) {
            blockArr.push_back(new Block(disPos(gen), disPos(gen), disLen(gen), true));
        } else {
            blockArr.push_back(new Block(disPos(gen), disPos(gen), disLen(gen), false));
        }
    }

    for (int i = 0; i < 2500; i++) {
        if (blockArr[i] -> isOverlap(player -> x, player -> y, player -> width, player -> height)) { //플레이어 위치에 블럭 생성되지 않도록
            blockArr.erase(blockArr.begin() + i);
        }
    }
}

void Game::createEnemy() {
    //랜덤
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> disX(-30, 160);
    std::uniform_int_distribution<int> disY(-30, 60);
    std::uniform_int_distribution<int> disType(1, 4);

    int createNum = enemyNum - enemyArr.size();
    if (score != 0) score += createNum;

    //적 생성
    for (int i = 0; i < createNum; i++) {
        int ex = disX(gen); //적 x좌표
        int ey = disY(gen); //적 y좌표
        int ew, eh;
        int etype = disType(gen);

        //예외처리
        bool isValidPos = true;
        if (55 <= ex && ex <= 75) { //플레이어 근방
            isValidPos = false;
        }

        switch(etype) {
            case 1:
            case 2:
                ew = 3;
                eh = 3;
                break;
            case 3:
                ew = 4;
                eh = 1;
                break;
            case 4:
                ew = 4;
                eh = 2;
                break; 
        }

        for (size_t i = 0; i < blockArr.size(); i++) { //블럭과 만남
            if (blockArr[i] -> isOverlap(ex, ey, ew, eh)) { //블럭 위치에 적 생성되지 않도록
                isValidPos = false;
                break;
            }
        }

        if (!isValidPos) continue;

        //적 생성
        switch(etype) {
            case 1:
                enemyArr.push_back(new Enemy(ENEMY1, ex, ey, ew, eh, enemyCharacter.enemy1));
                break;
            case 2:
                enemyArr.push_back(new Enemy(ENEMY2, ex, ey, ew, eh, enemyCharacter.enemy2));
                break;
            case 3:
                enemyArr.push_back(new Enemy(ENEMY3, ex, ey, ew, eh, enemyCharacter.enemy3Right));
                break;
            case 4:
                enemyArr.push_back(new Enemy(ENEMY4, ex, ey, ew, eh, enemyCharacter.enemy4));
                break; 
        }
    }
}

void Game::draw() {
    /*
    오브젝트들을 화면에 출력
    */
    player -> draw(&display);
    
    switch(weaponType) {
        case 1:
            bow -> draw(&display);
            bow -> drawArrows(&display);
            break;
        case 2:
            sword -> draw(&display);
            break;
        case 3:
            eraser -> draw(&display);
            break;
    }

    for (size_t i = 0; i < enemyArr.size(); i++) {
        enemyArr[i] -> draw(&display);
    }

    display.printDisplay();

    for (size_t i = 0; i < blockArr.size(); i++) {
        blockArr[i] -> draw();
    }

    printBackground();
}

void Game::moveEnemy() {
    /*
    20프레임마다 적 움직임
    앞에 블럭 혹은 다른 적 있을 시 피해서 감
    */

    for (size_t i = 0; i < enemyArr.size(); i++) {
        if (time % 20 == 0) {
            enemyArr[i] -> moveX(player -> x, player -> y, blockArr); //일정 시간마다 적이 움직임
            if (enemyArr[i] -> isBlock(blockArr) || isEnemy(i)) { //다른 적 혹은 block과 충돌 시 원위치
                enemyArr[i] -> x -= enemyArr[i] -> dx;
            }
            enemyArr[i] -> moveY(player -> x, player -> y, blockArr); //일정 시간마다 적이 움직임
            if (enemyArr[i] -> isBlock(blockArr) || isEnemy(i)) { //다른 적 혹은 block과 충돌 시 원위치
                enemyArr[i] -> y -= enemyArr[i] -> dy;
            }
        }
    }
}

bool Game::isEnemy(int idx) {
    int x = enemyArr[idx] -> x;
    int y = enemyArr[idx] -> y;
    int w = enemyArr[idx] -> width;
    int h = enemyArr[idx] -> height;

    for (size_t i = 0; i < enemyArr.size(); i++) {
        if ((int)i == idx) continue;
        int ex = enemyArr[i] -> x;
        int ey = enemyArr[i] -> y;
        int ew = enemyArr[i] -> width;
        int eh = enemyArr[i] -> height;

        if (y <= ey + eh - 1 && ey <= y + h - 1 && x <= ex + ew - 1 && ex <= x + w - 1) {
            return true;
        }
    }

    return false;
}

void Game::movePlayer(int direction) {
    if (!player -> isBlock(blockArr, direction, end)) { //앞에 블럭 없으면 감
        player -> move(direction, enemyArr, blockArr, playerCharacter);
    }
}

void Game::changeWeapon(int type) {
    weaponType = type;
}

void Game::updateWeapon(int input) {
    /*
    프레임마다 무기 update
    */
    switch (weaponType) {
        case 1:
            //플레이어 방향에 따라 활 방향 변경
            bow -> changeBowDirection(player -> direction, player -> x, player -> y);
            for (size_t i = 0; i < bow -> arrowArr.size(); i++) {
                //플레이어 움직임에 따라 활 움직임
                switch(input) {
                    case LEFT:
                        bow -> arrowArr[i] -> x++;
                        break;
                    case RIGHT:
                        bow -> arrowArr[i] -> x--;
                        break;
                    case BACK:
                        bow -> arrowArr[i] -> y--;
                        break;
                    case FRONT:
                        bow -> arrowArr[i] -> y++;
                        break;
                }
            }
            //시간에 따라 활 움직임
            if (time % 10 == 0) {
                if (bow -> attackTime != 0) bow -> attackTime++;
                bow -> moveArrows();
                bow -> checkCollision(enemyArr, blockArr); //충돌 시 활 없애기
                if (bow -> attackTime == bow -> cooldown) { //쿨타임
                    bow -> attackTime = 0;
                }
            }
            break;
        case 2:
            sword -> changeSwordDirection(player -> direction, player -> x);
            //시간에 따라 막대기 움직임
            if (time % 5 == 0) {
                if (sword -> attackTime != 0) {
                    sword -> attack(player -> direction, player -> x, enemyArr);
                    sword -> attackTime++;
                }
                if (sword -> attackTime == sword -> cooldown) {
                    sword -> attackTime = 0;
                }
            }
            break;
        case 3:
            if (time % 10 == 0) {
                if (eraser -> attackTime != 0) {
                    eraser -> attack(enemyArr);
                    eraser -> attackTime++;
                }
                if (eraser -> attackTime == eraser -> cooldown) {
                    eraser -> attackTime = 0;
                    eraser -> changeCharacter(eraserShape.eraserNonactive);
                }
            }
            break;
    }
}

void Game::attack() {
    /*
    공격 키 입력했을 때 실행
    */
    switch(weaponType) {
        case 1:
            if (bow -> attackTime == 0) {
                bow -> attackTime = 1;
                bow -> attack(player -> direction);
            }
            break;
        case 2:
            if (sword -> attackTime == 0) {
                sword -> attackTime = 1;
            }
            break;
        case 3:
            if (eraser -> attackTime == 0) {
                eraser -> attackTime = 1;
                eraser -> attack(enemyArr);
            }
            break;
    }
}

void Game::printBackground() {
    attron(COLOR_PAIR(11));
    mvprintw(1, 119, "SCORE: %d", score);
    switch(weaponType) {
        case 1:
            mvprintw(3, 119, "arrow(1)");
            break;
        case 2:
            mvprintw(3, 119, "sword(2)");
            break;
        case 3:
            mvprintw(3, 119, "eraser(3)");
            break;
    }
    attroff(COLOR_PAIR(11));
}