#include <graphics.h>
#include <conio.h>
#include <time.h>
#include "EasyXPng.h"
#include <mmsystem.h>
#include <vector>
#include <string>
#include <windows.h>
#pragma comment(lib, "winmm.lib")

#define WIDTH 560
#define HEIGHT 800
#define MaxBulletNum 200
#define MaxHealth 5
#define GAME_TIME 100 // 100 seconds game time

bool soundPlayed = false;

IMAGE im_bk, im_bullet, im_rocket, im_ufo, im_heart, im_explode;
int health = MaxHealth;
bool gameOver = false;
bool gameWon = false;
int gameTime = GAME_TIME;
std::vector<std::pair<int, int>> explosions;


class Rocket {
public:
    float x, y;
    float width, height;

    void draw() const {
        putimagePng(x - width / 2, y - height / 2, &im_rocket);
    }

    void update(float mx, float my) {
        x = mx;
        y = my;
    }
};

class Bullet {
public:
    float x, y;
    float vx, vy;
    float radius;
    bool active = true;

    void draw() const {
        if (active) putimagePng(x - radius, y - radius, &im_bullet);
    }

    void update() {
        if (!active) return;
        x += vx;
        y += vy;
        if (x <= 0 || x >= WIDTH) vx = -vx;
        if (y <= 0 || y >= HEIGHT) vy = -vy;
    }
};

class UFO {
public:
    float x, y;
    float speed;
    bool active = true;

    void draw() const {
        if (active) putimagePng(x - im_ufo.getwidth() / 2, y - im_ufo.getheight() / 2, &im_ufo);
    }

    void update(const Rocket& rocket) {
        if (!active) return;
        float dx = rocket.x - x;
        float dy = rocket.y - y;
        float dist = sqrt(dx * dx + dy * dy);
        if (dist != 0) {
            x += speed * (dx / dist);
            y += speed * (dy / dist);
        }
    }
};


std::vector<Bullet> bullets;
Rocket rocket;
UFO ufo;

void startup() {
    srand(time(0));
    loadimage(&im_bk, _T("background.png"));
    loadimage(&im_bullet, _T("bullet.png"));
    loadimage(&im_rocket, _T("rocket.png"));
    loadimage(&im_heart, _T("heart.png"));
    loadimage(&im_ufo, _T("ufo.png"));
    loadimage(&im_explode, _T("blowup.png"));

    rocket.width = im_rocket.getwidth();
    rocket.height = im_rocket.getheight();

    ufo.x = WIDTH / 2;
    ufo.y = HEIGHT / 4;
    ufo.speed = 2.0;

    initgraph(WIDTH, HEIGHT);
    BeginBatchDraw();
}

void playExplosionSound() {
    if (!soundPlayed) {
        MCIERROR error = mciSendString(_T("play explode.mp3"), NULL, 0, NULL);
        if (error != 0) {
            TCHAR errorMsg[256];
            mciGetErrorString(error, errorMsg, 256);
            MessageBox(NULL, errorMsg, _T("Sound Error"), MB_OK | MB_ICONERROR);
        }
        soundPlayed = true;
    }
}

void handleGameOver(int x, int y) {
    explosions.push_back({ x, y });
    gameOver = true;
    playExplosionSound();  // Play sound only when game over
}

void resetGame() {
    health = MaxHealth;
    gameOver = false;
    gameWon = false;
    gameTime = GAME_TIME;
    bullets.clear();
    explosions.clear();
    soundPlayed = false;  // Add this line

    rocket.x = WIDTH / 2;
    rocket.y = HEIGHT - 100;

    ufo.x = WIDTH / 2;
    ufo.y = HEIGHT / 4;
    ufo.active = true;
}

void show() {
    putimage(0, 0, &im_bk);

    for (int i = 0; i < health; i++) {
        putimagePng(10 + i * im_heart.getwidth(), 10, &im_heart);
    }

    for (const auto& bullet : bullets)
        bullet.draw();

    if (ufo.active) ufo.draw();
    if (!gameOver) rocket.draw();

    // Display timer in the top right corner
    TCHAR s[20];
    _stprintf_s(s, _T("%d"), gameTime);
    settextcolor(WHITE);
    settextstyle(30, 0, _T("Arial"));
    outtextxy(WIDTH - 70, 10, s);

    // Draw explosions
    for (const auto& explosion : explosions) {
        putimagePng(explosion.first - im_explode.getwidth() / 2,
            explosion.second - im_explode.getheight() / 2, &im_explode);

        // Play explosion sound if there are any new explosions
        //static size_t lastExplosionCount = 0;
        //if (explosions.size() > lastExplosionCount) {
           // playExplosionSound();
           // lastExplosionCount = explosions.size();
       // }
    }

   

    if (gameOver) {
        setbkmode(TRANSPARENT);

        // Title message with bold impact style and fiery orange
        settextstyle(60, 0, _T("Impact"));
        settextcolor(RGB(255, 69, 0));  // Red-Orange
        outtextxy(WIDTH / 2 - 200, HEIGHT / 2 - 100, _T("Game Over!"));

        // First instruction with a cool, lighter blue
        settextstyle(30, 0, _T("Arial"));
        settextcolor(RGB(135, 206, 250));  // Light Sky Blue
        outtextxy(WIDTH / 2 - 180, HEIGHT / 2 + 10, _T("Missed it! Ready for another round?"));

        // Restart prompt in a soft orange tone
        settextstyle(30, 0, _T("Comic Sans MS"));
        settextcolor(RGB(255, 165, 0));  // Soft Orange
        outtextxy(WIDTH / 2 - 140, HEIGHT / 2 + 60, _T("Press Enter to restart the game"));

        // Exit prompt with a calm teal
        settextstyle(30, 0, _T("Comic Sans MS"));
        settextcolor(RGB(72, 209, 204));  // Medium Turquoise
        outtextxy(WIDTH / 2 - 100, HEIGHT / 2 + 100, _T("Press Esc to exit"));
    }


    if (gameWon) {
        setbkmode(TRANSPARENT);

        // Congratulations message in bold, shiny gold
        settextstyle(50, 0, _T("Impact"));
        settextcolor(RGB(255, 223, 0));  // Gold
        outtextxy(WIDTH / 2 - 250, HEIGHT / 2 - 100, _T("Mission Accomplished!"));

        // First line with elegant lime green for Shakib-124 signature
        settextstyle(30, 0, _T("Times New Roman"));
        settextcolor(RGB(50, 205, 50));  // Lime Green
        outtextxy(10, HEIGHT / 2 - 60, _T("Congratulations from Shakib-2021521460124"));

        // Second line with a bright white
        settextstyle(40, 0, _T("Arial Black"));
        settextcolor(RGB(255, 255, 255));  // White
        outtextxy(WIDTH / 2 - 170, HEIGHT / 2 - 20, _T("You passed the mission!"));

        // Next challenge prompt in vibrant gold
        settextstyle(30, 0, _T("Comic Sans MS"));
        settextcolor(RGB(255, 215, 0));  // Vibrant Gold
        outtextxy(WIDTH / 2 - 200, HEIGHT / 2 + 50, _T("Press Enter to take on a new challenge"));

        // Exit prompt with deep orange for contrast
        settextstyle(30, 0, _T("Comic Sans MS"));
        settextcolor(RGB(255, 140, 0));  // Dark Orange
        outtextxy(WIDTH / 2 - 100, HEIGHT / 2 + 90, _T("Press Esc to exit"));
    }

    FlushBatchDraw();



}

void updateWithoutInput() {
    static clock_t lastTime = clock();
    clock_t now = clock();

    if (now - lastTime >= CLOCKS_PER_SEC) {
        lastTime = now;
        if (gameTime > 0) gameTime--;
        if (gameTime == 0 && !gameOver) {
            gameWon = true;
        }
    }

    if (bullets.size() < MaxBulletNum && rand() % 100 == 0) {
        Bullet newBullet;
        newBullet.x = WIDTH / 2;
        newBullet.y = 10;
        float angle = (rand() / double(RAND_MAX) - 0.5) * 0.9 * PI;
        float scalar = 2 * rand() / double(RAND_MAX) + 2;
        newBullet.vx = scalar * sin(angle);
        newBullet.vy = scalar * cos(angle);
        newBullet.radius = im_bullet.getwidth() / 2;
        bullets.push_back(newBullet);
    }

    int collisions = 0;
    for (auto& bullet : bullets) {
        bullet.update();

        if (bullet.active) {
            float distX = bullet.x - rocket.x;
            float distY = bullet.y - rocket.y;
            if (sqrt(distX * distX + distY * distY) < (bullet.radius + rocket.width / 2)) {
                collisions++;
                bullet.active = false;
            }
        }
    }

    if (collisions > 0) {
        health -= collisions;
        if (health <= 0) {
            handleGameOver(rocket.x, rocket.y);
        }
        
    }

    if (ufo.active) {
        float ufoDistX = ufo.x - rocket.x;
        float ufoDistY = ufo.y - rocket.y;
        if (sqrt(ufoDistX * ufoDistX + ufoDistY * ufoDistY) < (im_ufo.getwidth() / 2 + rocket.width / 2)) {
            handleGameOver(rocket.x, rocket.y);
            ufo.active = false;
        }
    }

    if (ufo.active) ufo.update(rocket);

    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [](const Bullet& b) { return !b.active; }), bullets.end());
}

void updateWithInput() {

    MOUSEMSG m;
    while (MouseHit()) {
        m = GetMouseMsg();
        if (m.uMsg == WM_MOUSEMOVE)
            rocket.update(m.x, m.y);
    }

    if (_kbhit()) {
        char key = _getch();
        if (key == 27) { // ESC key
            exit(0);
        }
        else if (key == 13) { // Enter key
            if (gameOver || gameWon) {
                resetGame();
            }
        }
    }
}

int main() {
    startup();

    while (true) {
        show();
        if (!gameOver && !gameWon) {
            updateWithoutInput();
            updateWithInput();
        }
        else {
            updateWithInput(); // Only check for input to restart or exit
        }
        Sleep(10);
    }

    closegraph();
    return 0;
}