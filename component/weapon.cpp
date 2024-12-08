#include "weapon.hpp"

Weapon::Weapon(int _x, int _y, int _width, int _height, int _cooldown, Cell** _character)
: Component(_x, _y, _width, _height, _character) {
    cooldown = _cooldown;
    attackTime = 0;
}

/*화살*/
Arrow::Arrow(int _x, int _y, int _direction, Cell** _character)
: Component(_x, _y, 1, 1, _character) {
    direction = _direction;
}

int Arrow::isHitEnemy(vector<Enemy*>& enemyArr) {
    /*
    화살과 적이 부딪혔다면 적의 인덱스를 반환
    부딪히지 않았다면 -1 반환
    */
   
    for (size_t i = 0; i < enemyArr.size(); i++) {
        int ex = enemyArr[i] -> x;
        int ey = enemyArr[i] -> y;
        if (ex <= x && x <= ex + 2 && ey <= y && y <= ey + 2) { //overlap 체크
            return i;
        }
    }
    return -1;
}

void Arrow::move() {
    /*
    나가는 방향에 따라 화살 움직임
    */

    switch(direction) {
        case LEFT:
            x -= 2;
            break;
        case RIGHT:
            x += 2;
            break;
        case BACK:
            y++;
            break;
        case FRONT:
            y--;
            break;
    }
}

bool Arrow::isHitBlock(vector<Block*>& blockArr) {
    /*
    블럭과 만났는지 체크
    */

    for (size_t i = 0; i < blockArr.size(); i++) {
        int bx = blockArr[i] -> x;
        int by = blockArr[i] -> y;
        int bh = blockArr[i] -> height;

        for (int r = 0; r < bh; r++) {
            for (int c = 0; c < bh * 2; c++) {
                if (x == c + bx && y == r + by) {
                    return true;
                }
            }
        }
    }

    return false;
}

/*활*/
Bow::Bow(int _x, int _y, Cell** _character)
: Weapon(_x, _y, 3, 1, 5, _character) {
    
}

void Bow::attack(int direction) {
    /*
    공격 방향에 따라 화살 배열에 화살 추가
    */

    BowShape bowShape;

    switch(direction) {
        case LEFT:
            arrowArr.push_back(new Arrow(x, y, direction, bowShape.arrowLeft));
            break;
        case RIGHT:
            arrowArr.push_back(new Arrow(x + 2, y, direction, bowShape.arrowRight));
            break;
        case BACK:
            arrowArr.push_back(new Arrow(x + 1, y + 2, direction, bowShape.arrowUp));
            break;
        case FRONT:
            arrowArr.push_back(new Arrow(x + 1, y, direction, bowShape.arrowDown));
            break;
    }
}

void Bow::changeBowDirection(int direction, int playerX, int playerY) {
    /*
    플레이어가 바라보는 방향에 따라 화살 이미지 변경
    */

    BowShape bowshape;
    
    switch (direction) {
        case LEFT:
            changeCharacter(bowshape.bowLeft);
            x = playerX - 3;
            y = playerY + 1;
            break;
        case RIGHT:
            changeCharacter(bowshape.bowRight);
            x = playerX + 3;
            y = playerY + 1;
            break;
        case BACK:
            changeCharacter(bowshape.bowUp);
            x = playerX;
            y = playerY + 1;
            break;
        case FRONT:
            changeCharacter(bowshape.bowDown);
            x = playerX;
            y = playerY + 1;
            break;
    }
}

void Bow::drawArrows(Display* display) {
    for (size_t i = 0; i < arrowArr.size(); i++) {
        arrowArr[i] -> draw(display);
    }
}

void Bow::moveArrows() {
    for (size_t i = 0; i < arrowArr.size(); i++) {
        arrowArr[i] -> move();
    }
}

void Bow::checkCollision(vector<Enemy*>& enemyArr, vector<Block*>& blockArr) {
    for (size_t i = 0; i < arrowArr.size(); i++) {
        int enemyIdx = arrowArr[i] -> isHitEnemy(enemyArr);
        if (enemyIdx != -1) {
            enemyArr.erase(enemyArr.begin() + enemyIdx);
        } else if (arrowArr[i] -> isHitBlock(blockArr)) {
            arrowArr.erase(arrowArr.begin() + i);
        }
    }
}

/*검*/
Sword::Sword(int _x, int _y, Cell** _character)
: Weapon(_x, _y, 5, 6, 5, _character) {
    direction = RIGHT;
}

void Sword::attack(int input, int playerX, vector<Enemy*>& enemyArr) {
    SwordShape swordShape;

    if (input == RIGHT || input == LEFT) direction = input;

    if (attackTime > 5) return; 

    if (direction == RIGHT) {
        x = playerX + 3;
        changeCharacter(swordShape.swordRight[attackTime - 1]);
    } else if (direction == LEFT) {
        x = playerX - 5;
        changeCharacter(swordShape.swordLeft[attackTime - 1]);
    }

    for (size_t i = 0; i < enemyArr.size(); i++) {
        int ex = enemyArr[i] -> x;
        int ey = enemyArr[i] -> y;
        int ew = enemyArr[i] -> width;
        int eh = enemyArr[i] -> height;

        if (y <= ey + eh - 1 && ey <= y + height - 1 && x <= ex + ew - 1 && ex <= x + width - 1) {
            enemyArr.erase(enemyArr.begin() + i);
        }
    }
}

void Sword::changeSwordDirection(int input, int playerX) {
    SwordShape swordShape;

    if (input == RIGHT || input == LEFT) direction = input;

    if (direction == LEFT) {
        x = playerX - 5;
        changeCharacter(swordShape.swordLeft[0]);
    } else if (direction == RIGHT) {
        x = playerX + 3;
        changeCharacter(swordShape.swordRight[0]);
    }
}

/*원반*/
Eraser::Eraser(int _x, int _y, Cell** _character)
: Weapon(_x, _y, 7, 5, 5, _character) {
    
}

void Eraser::attack(vector<Enemy*>& enemyArr) {
    EraserShape eraserShape;
    for (size_t i = 0; i < enemyArr.size(); i++) {
        changeCharacter(eraserShape.eraserActive);
        int ex = enemyArr[i] -> x;
        int ey = enemyArr[i] -> y;
        int ew = enemyArr[i] -> width;
        int eh = enemyArr[i] -> height;

        if (y <= ey + eh - 1 && ey <= y + height - 1 && x <= ex + ew - 1 && ex <= x + width - 1) {
            enemyArr.erase(enemyArr.begin() + i);
        }
    }
}