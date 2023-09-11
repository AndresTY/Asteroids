#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <list>
#include <math.h>
#include <time.h>
#include <vector>
using namespace sf;

const int W = 1200;
const int H = 800;
Texture t2;
float DEGTORAD = 0.017453f;
Clock reloj;
std::string map[5] = {"images/backgrounds/background.jpg",
                      "images/backgrounds/background_derecha.png",
                      "images/backgrounds/background_izquierda.jpg",
                      "images/backgrounds/background_abajo.jpg",
                      "images/backgrounds/background_arriba.jpg"};

class Animation {
public:
  float Frame, speed;
  Sprite sprite;
  std::vector<IntRect> frames;

  Animation() {}

  Animation(Texture &t, int x, int y, int w, int h, int count, float Speed) {
    Frame = 0;
    speed = Speed;

    for (int i = 0; i < count; i++)
      frames.push_back(IntRect(x + i * w, y, w, h));

    sprite.setTexture(t);
    sprite.setOrigin(w / 2, h / 2);
    sprite.setTextureRect(frames[0]);
  }

  void update() {
    Frame += speed;
    int n = frames.size();
    if (Frame >= n)
      Frame -= n;
    if (n > 0)
      sprite.setTextureRect(frames[int(Frame)]);
  }

  bool isEnd() { return Frame + speed >= frames.size(); }
};

class Entity {
public:
  float x, y, dx, dy, R, angle;
  bool life;
  std::string name;
  Animation anim;
  Time start;
  Time updateTime;
  Entity() { life = 1; }

  void settings(Animation &a, int X, int Y, float Angle = 0, int radius = 1) {
    anim = a;
    x = X;
    y = Y;
    angle = Angle;
    R = radius;
  }

  virtual void update(){};

  void draw(RenderWindow &app) {
    anim.sprite.setPosition(x, y);
    anim.sprite.setRotation(angle + 90);
    app.draw(anim.sprite);

    CircleShape circle(R);
    circle.setFillColor(Color(255, 0, 0, 170));
    circle.setPosition(x, y);
    circle.setOrigin(R, R);
    // app.draw(circle);
  }

  virtual ~Entity(){};
};

class shell : public Entity {
public:
  shell() { name = "shell"; }
};

class asteroid : public Entity {
public:
  asteroid() {
    dx = rand() % 8 - 4;
    dy = rand() % 8 - 4;
    name = "asteroid";
  }

  void update() {
    x += dx;
    y += dy;

    if (x > W)
      x = 0;
    if (x < 0)
      x = W;
    if (y > H)
      y = 0;
    if (y < 0)
      y = H;
  }
};

class bullet : public Entity {
public:
  bullet() { name = "bullet"; }

  void update() {
    dx = cos(angle * DEGTORAD) * 6;
    dy = sin(angle * DEGTORAD) * 6;
    // angle+=rand()%7-3;  /*try this*/
    x += dx;
    y += dy;

    if (x > W || x < 0 || y > H || y < 0)
      life = 0;
  }
};

class player : public Entity {
public:
  bool thrust;
  bool back = false;
  std::string temp;
  player() { name = "player"; }

  void update() {
    if (thrust) {
      dx += cos(angle * DEGTORAD) * 0.2;
      dy += sin(angle * DEGTORAD) * 0.2;
    } else if (back) {
      dx -= cos(angle * DEGTORAD) * 0.2;
      dy -= sin(angle * DEGTORAD) * 0.2;
    } else {
      dx *= 0.99;
      dy *= 0.99;
    }

    int maxSpeed = 15;
    float speed = sqrt(dx * dx + dy * dy);
    if (speed > maxSpeed) {
      dx *= maxSpeed / speed;
      dy *= maxSpeed / speed;
    }

    x += dx;
    y += dy;

    if (x > W) {
      temp = map[0];
      map[0] = map[1];
      map[1] = map[2];
      map[2] = temp;
      t2.loadFromFile(map[0]);
      t2.setSmooth(true);
      x = 0;
    }
    if (x < 0) {
      temp = map[0];
      map[0] = map[2];
      map[2] = map[1];
      map[1] = temp;
      t2.loadFromFile(map[0]);
      t2.setSmooth(true);
      x = W;
    }
    if (y > H) {
      temp = map[0];
      map[0] = map[3];
      map[3] = map[4];
      map[4] = temp;
      t2.loadFromFile(map[0]);
      t2.setSmooth(true);
      y = 0;
    }
    if (y < 0) {
      temp = map[0];
      map[0] = map[4];
      map[4] = map[3];
      map[3] = temp;
      t2.loadFromFile(map[0]);
      t2.setSmooth(true);
      y = H;
    }
  }
};

bool isCollide(Entity *a, Entity *b) {
  return (b->x - a->x) * (b->x - a->x) + (b->y - a->y) * (b->y - a->y) <
         (a->R + b->R) * (a->R + b->R);
}

int main() {
  srand(time(0));

  RenderWindow app(VideoMode(W, H), "Asteroids!");
  app.setFramerateLimit(60);

  Texture t1, t3, t4, t5, t6, t7, t8, t9;
  t1.loadFromFile("images/spaceship.png");
  t2.loadFromFile("images/backgrounds/background.jpg");
  t3.loadFromFile("images/explosions/type_C.png");
  t4.loadFromFile("images/rock.png");
  t5.loadFromFile("images/fire_blue.png");
  t6.loadFromFile("images/rock_small.png");
  t7.loadFromFile("images/explosions/type_B.png");
  t8.loadFromFile("images/powers/shell.png");
  t9.loadFromFile("images/powers/shield-120x120.png");

  t1.setSmooth(true);
  t2.setSmooth(true);

  Sprite background(t2);

  Animation sExplosion(t3, 0, 0, 256, 256, 48, 0.5);
  Animation sRock(t4, 0, 0, 64, 64, 16, 0.2);
  Animation sRock_small(t6, 0, 0, 64, 64, 16, 0.2);
  Animation sBullet(t5, 0, 0, 32, 64, 16, 0.8);
  Animation sPlayer(t1, 40, 0, 40, 40, 1, 0);
  Animation sPlayer_go(t1, 40, 40, 40, 40, 1, 0);
  Animation sExplosion_ship(t7, 0, 0, 192, 192, 64, 0.5);
  Animation sShell(t8, 0, 0, 47, 47, 1, 0);
  Animation sShield(t9, 0, 0, 240, 243, 1, 0);

  sf::Font font;
  if (!font.loadFromFile("fonts/UbuntuMonoNerdFont-Regular.ttf"))
    printf("UbuntuMonoNerdFont.ttf not found");
  sf::Text TimerGame;
  TimerGame.setFont(font);
  TimerGame.setString("Duration:" + std::to_string(0));
  TimerGame.setFillColor(sf::Color(255, 255, 255, 255));
  TimerGame.setCharacterSize(24);
  TimerGame.setPosition(10, 20);

  Music music, blazer;

  if (!blazer.openFromFile("music/effects/blazer.wav"))
    return -1;
  if (!music.openFromFile("music/theme.wav"))
    return -1;
  music.setVolume(20.f);
  music.setLoop(true);
  blazer.setVolume(100.f);
  music.play();
  std::list<Entity *> entities;

  for (int i = 0; i < 5; i++) {
    asteroid *a = new asteroid();
    a->settings(sRock, rand() % W, rand() % H, rand() % 360, 25);
    entities.push_back(a);
  }

  for (int i = 0; i < 5; i++) {
    shell *s = new shell();
    s->settings(sShell, rand() % W, rand() % H, rand() % 360, 25);
    entities.push_back(s);
  }

  player *p = new player();
  p->settings(sPlayer, 200, 200, 0, 20);
  entities.push_back(p);

  Time duration = reloj.getElapsedTime();
  /////main loop/////
  while (app.isOpen()) {
    Event event;
    while (app.pollEvent(event)) {
      if (event.type == Event::Closed)
        app.close();

      if (Joystick::isConnected(0)) {
        if (event.type == Event::JoystickButtonPressed) {
          if (event.joystickButton.button == 5) {
            bullet *b = new bullet();
            blazer.play();
            b->settings(sBullet, p->x, p->y, p->angle, 10);
            entities.push_back(b);
          }
          if (event.joystickButton.button == 7 ||
              event.joystickButton.button == 6) {
            app.close();
          }
        }
      } else {
        if (event.type == Event::KeyPressed)
          if (event.key.code == Keyboard::Space) {
            bullet *b = new bullet();
            blazer.play();
            b->settings(sBullet, p->x, p->y, p->angle, 10);
            entities.push_back(b);
          }
      }
    }
    if (Joystick::isConnected(0)) {

      if (Joystick::getAxisPosition(0, Joystick::PovX) > 0)
        p->angle += 3;
      else if (Joystick::getAxisPosition(0, Joystick::PovX) < 0)
        p->angle -= 3;
      else if (Joystick::isButtonPressed(0, 0))
        p->thrust = Joystick::isButtonPressed(0, 0);
      else if (Joystick::isButtonPressed(0, 1))
        p->back = Joystick::isButtonPressed(0, 1);
      else {
        p->thrust = false;
        p->back = false;
      }
    } else {

      if (Keyboard::isKeyPressed(Keyboard::D))
        p->angle += 3;
      else if (Keyboard::isKeyPressed(Keyboard::A))
        p->angle -= 3;
      else if (Keyboard::isKeyPressed(Keyboard::W))
        p->thrust = Keyboard::isKeyPressed(Keyboard::W);
      else if (Keyboard::isKeyPressed(Keyboard::S))
        p->back = Keyboard::isKeyPressed(Keyboard::S);
      else {
        p->thrust = false;
        p->back = false;
      }
    }
    for (auto a : entities)
      for (auto b : entities) {
        if (a->name == "asteroid" && b->name == "bullet")
          if (isCollide(a, b)) {
            a->life = false;
            b->life = false;

            // sonido aqui
            Entity *e = new Entity();
            e->settings(sExplosion, a->x, a->y);
            e->name = "explosion";
            entities.push_back(e);

            for (int i = 0; i < 2; i++) {
              if (a->R == 15)
                continue;
              Entity *e = new asteroid();
              e->settings(sRock_small, a->x, a->y, rand() % 360, 15);
              entities.push_back(e);
            }
          }

        if (a->name == "asteroid" && b->name == "Shield")
          if (isCollide(a, b)) {
            a->life = false;

            // sonido aqui
            Entity *e = new Entity();
            e->settings(sExplosion, a->x, a->y);
            e->name = "explosion";
            entities.push_back(e);
          }

        if (a->name == "player" && b->name == "shell")
          if (isCollide(a, b)) {
            b->life = false;
            Entity *e = new Entity();
            e->settings(sShield, b->x, b->y, b->angle, 100);
            e->name = "Shield";
            e->start = reloj.getElapsedTime();
            entities.push_back(e);
            b->life = 0;
          }

        if (a->name == "player" && b->name == "asteroid")
          if (isCollide(a, b)) {
            b->life = false;

            Entity *e = new Entity();
            e->settings(sExplosion_ship, a->x, a->y);
            e->name = "explosion";
            entities.push_back(e);

            p->settings(sPlayer, W / 2, H / 2, 0, 20);
            p->dx = 0;
            p->dy = 0;
          }
        if (a->name == "Shield") {
          a->x = p->x;
          a->y = p->y;
        }
      }

    if (p->thrust || p->back)
      p->anim = sPlayer_go;
    else
      p->anim = sPlayer;

    for (auto e : entities) {
      if (e->name == "explosion")
        if (e->anim.isEnd())
          e->life = 0;
      if (e->name == "Shield") {
        std::cout << e->updateTime.asSeconds() - e->start.asSeconds()
                  << std::endl;
        if (e->updateTime.asSeconds() - e->start.asSeconds() > 20.f) {
          e->life = 0;
        } else {
          e->updateTime = reloj.getElapsedTime();
        }
      }
    }
    if (rand() % 150 == 0) {
      asteroid *a = new asteroid();
      a->settings(sRock, 0, rand() % H, rand() % 360, 25);
      entities.push_back(a);
    }

    for (auto i = entities.begin(); i != entities.end();) {
      Entity *e = *i;

      e->update();
      e->anim.update();

      if (e->life == false) {
        i = entities.erase(i);
        delete e;
      } else
        i++;
    }

    Time tempTime = reloj.getElapsedTime();
    //////draw//////
    TimerGame.setString("Duration: " + std::to_string(tempTime.asSeconds() -
                                                      duration.asSeconds()));
    app.draw(background);
    app.draw(TimerGame);
    for (auto i : entities)
      i->draw(app);
    app.display();
  }

  return 0;
}
