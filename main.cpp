#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <list>
#include <math.h>
#include <time.h>
#include <vector>
using namespace sf;

const int W = 1200;
const int RealH = 800;
const int H = 650;
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
  int life;
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
class torpedo : public Entity {
public:
  torpedo() { name = "torpedo"; }

  void update() {
    dx = cos(angle * DEGTORAD) * 6;
    dy = sin(angle * DEGTORAD) * 6;
    // angle += rand() % 7 - 3; /*try this*/
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
  bool visibility = true;
  std::string powerType = "blue";
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
class Power : public Entity {

  std::string song;

public:
  Animation anim;
  Power(std::string names, std::string songs = "",
        Animation anims = Animation()) {
    name = names;
    anim = anims;
    song = songs;
  }
  Entity *action(int x, int y, float angle, Music &songPower) {
    Entity *e = new Entity();
    e->settings(anim, x, y, angle, 100);
    e->name = name;
    e->start = reloj.getElapsedTime();
    if (song != "") {
      songPower.openFromFile("music/effects/" + song);
      songPower.play();
    }
    return e;
  }
  Entity *action(player &e, Music &songPower) {
    Entity *ee = new Entity();
    ee->settings(anim, 0, 0, 0, 0);
    ee->name = name;
    ee->start = reloj.getElapsedTime();
    if (song != "") {
      songPower.openFromFile("music/effects/" + song);
      songPower.play();
    }
    e.visibility = false;
    return ee;
  }
  Entity *action(player &e) {
    Entity *ee = new Entity();
    ee->settings(anim, 0, 0, 0, 0);
    ee->name = name;
    ee->start = reloj.getElapsedTime();
    e.powerType = "red";
    return ee;
  }
};

bool isCollide(Entity *a, Entity *b) {
  return (b->x - a->x) * (b->x - a->x) + (b->y - a->y) * (b->y - a->y) <
         (a->R + b->R) * (a->R + b->R);
}

int main() {
  srand(time(0));
  bool gameover = false;
  int score = 0;

  RenderWindow app(VideoMode(W, RealH), "Asteroids!");
  sf::RectangleShape rectangle(Vector2f(W + 50, RealH - H));
  rectangle.setFillColor(Color::Black);
  rectangle.setPosition(0, H + 25);
  app.setFramerateLimit(60);

  Texture t1, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13;
  t1.loadFromFile("images/spaceship.png");
  t2.loadFromFile("images/backgrounds/background.jpg");
  t3.loadFromFile("images/explosions/type_C.png");
  t4.loadFromFile("images/rock.png");
  t5.loadFromFile("images/fire_blue.png");
  t13.loadFromFile("images/fire_red.png");
  t6.loadFromFile("images/rock_small.png");
  t7.loadFromFile("images/explosions/type_B.png");
  t8.loadFromFile("images/powers/shell.png");
  t10.loadFromFile("images/powers/life.png");
  t11.loadFromFile("images/powers/invi.png");
  t12.loadFromFile("images/powers/arma.png");
  t9.loadFromFile("images/powers/shield-120x120.png");

  t1.setSmooth(true);
  t2.setSmooth(true);

  Sprite background(t2);

  Animation sExplosion(t3, 0, 0, 256, 256, 48, 0.5);
  Animation sRock(t4, 0, 0, 64, 64, 16, 0.2);
  Animation sRock_small(t6, 0, 0, 64, 64, 16, 0.2);
  Animation sBullet(t5, 0, 0, 32, 64, 16, 0.8);
  Animation sTorpedo(t13, 0, 0, 32, 64, 16, 0.8);
  Animation sPlayer(t1, 40, 0, 40, 40, 1, 0);
  Animation sPlayer_go(t1, 40, 40, 40, 40, 1, 0);
  Animation sExplosion_ship(t7, 0, 0, 192, 192, 64, 0.5);
  Animation sShell(t8, 0, 0, 47, 47, 1, 0);
  Animation sShield(t9, 0, 0, 240, 243, 1, 0);
  Animation sLife(t10, 0, 0, 47, 47, 1, 0);
  Animation sInvi(t11, 0, 0, 47, 47, 1, 0);
  Animation sRed(t12, 0, 0, 47, 47, 1, 0);

  sf::Font font;
  if (!font.loadFromFile("fonts/UbuntuMonoNerdFont-Regular.ttf"))
    printf("UbuntuMonoNerdFont.ttf not found");
  // Timer text
  sf::Text TimerGame;
  TimerGame.setFont(font);
  TimerGame.setString("Duration:" + std::to_string(0));
  TimerGame.setFillColor(sf::Color(255, 255, 255, 255));
  TimerGame.setCharacterSize(24);
  TimerGame.setPosition(10, H + 20);
  // Score text
  sf::Text ScoreGame;
  ScoreGame.setFont(font);
  ScoreGame.setString("Score:" + std::to_string(0));
  ScoreGame.setFillColor(sf::Color(255, 255, 255, 255));
  ScoreGame.setCharacterSize(24);
  ScoreGame.setPosition(10, H + 40);
  // life text
  sf::Text lifeTime;
  lifeTime.setFont(font);
  lifeTime.setString("life:" + std::to_string(0));
  lifeTime.setFillColor(sf::Color(255, 255, 255, 255));
  lifeTime.setCharacterSize(24);
  lifeTime.setPosition(W - 100, H + 20);
  // life text
  sf::Text PowersText;
  PowersText.setFont(font);
  PowersText.setString("<\tNone\t>");
  PowersText.setFillColor(sf::Color(255, 255, 255, 255));
  PowersText.setCharacterSize(24);
  PowersText.setPosition(W / 2, H + 20);

  Music music, blazer, songPower;

  if (!blazer.openFromFile("music/effects/blazer.wav"))
    return -1;
  if (!music.openFromFile("music/theme.wav"))
    return -1;
  music.setVolume(20.f);
  music.setLoop(true);
  blazer.setVolume(100.f);
  songPower.setVolume(100.f);
  music.play();
  std::list<Entity *> entities;
  std::vector<Power *> powers;

  for (int i = 0; i < 1; i++) {
    asteroid *a = new asteroid();
    a->settings(sRock, rand() % W, rand() % H, rand() % 360, 25);
    entities.push_back(a);
  }

  for (int i = 0; i < 3; i++) {
    Entity *s = new Entity();
    s->name = "life";
    s->settings(sLife, rand() % W, rand() % H, rand() % 360, 25);
    entities.push_back(s);
  }
  for (int i = 0; i < 1; i++) {

    shell *s = new shell();
    s->settings(sShell, rand() % W, rand() % H, rand() % 360, 25);
    entities.push_back(s);
  }
  for (int i = 0; i < 1; i++) {
    Entity *s = new Entity();
    s->name = "invi";
    s->settings(sInvi, rand() % W, rand() % H, rand() % 360, 25);
    entities.push_back(s);
  }
  for (int i = 0; i < 1; i++) {
    Entity *s = new Entity();
    s->name = "arma";
    s->settings(sRed, rand() % W, rand() % H, rand() % 360, 25);
    entities.push_back(s);
  }
  player *p = new player();
  p->settings(sPlayer, 200, 200, 0, 20);
  p->life = 3;
  entities.push_back(p);

  Time duration = reloj.getElapsedTime();
  /////main loop/////
  while (app.isOpen()) {
    Event event;
    while (app.pollEvent(event)) {
      if (event.type == Event::Closed)
        app.close();
      if (!gameover) {
        if (Joystick::isConnected(0)) {
          if (event.type == Event::JoystickButtonPressed) {
            if (event.joystickButton.button == 5) {
              if (p->powerType == "blue") {
                bullet *b = new bullet();
                blazer.play();
                b->settings(sBullet, p->x, p->y, p->angle, 10);
                entities.push_back(b);
              }
              if (p->powerType == "red") {
                torpedo *b = new torpedo();
                blazer.play();
                b->settings(sTorpedo, p->x, p->y, p->angle, 10);
                entities.push_back(b);
              }
            }
            if (event.joystickButton.button == 4 && !powers.empty()) {
              Entity *b;
              if (powers.at(0)->name == "Invi")
                b = powers.at(0)->action(*p, songPower);
              else if (powers.at(0)->name == "Red")
                b = powers.at(0)->action(*p);
              else
                b = powers.at(0)->action(p->x, p->y, 1, songPower);
              entities.push_back(b);
              powers.erase(powers.begin());
            }
            if (event.joystickButton.button == 2 && !powers.empty()) {
              powers.push_back(powers.at(0));
              powers.erase(powers.begin());
            }

            if (event.joystickButton.button == 7 ||
                event.joystickButton.button == 6) {
              app.close();
            }
          }
        } else {
          if (event.type == Event::KeyPressed) {
            if (event.key.code == Keyboard::Space) {
              if (p->powerType == "blue") {
                bullet *b = new bullet();
                blazer.play();
                b->settings(sBullet, p->x, p->y, p->angle, 10);
                entities.push_back(b);
              }
              if (p->powerType == "red") {
                torpedo *b = new torpedo();
                blazer.play();
                b->settings(sTorpedo, p->x, p->y, p->angle, 10);
                entities.push_back(b);
              }
            }
            if (event.key.code == Keyboard::E && !powers.empty()) {
              Entity *b;
              if (powers.at(0)->name == "Invi")
                b = powers.at(0)->action(*p, songPower);
              else if (powers.at(0)->name == "Red")
                b = powers.at(0)->action(*p);
              else
                b = powers.at(0)->action(p->x, p->y, 1, songPower);
              entities.push_back(b);
              powers.erase(powers.begin());
            }
            if (event.key.code == Keyboard::Q && !powers.empty()) {
              powers.push_back(powers.at(0));
              powers.erase(powers.begin());
            }
          }
        }
      }
    }
    if (!gameover) {
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
            !gameover ? score += 1 : score += 0;
          }

        if (a->name == "asteroid" && b->name == "torpedo")
          if (isCollide(a, b)) {
            a->life = false;
            b->life = false;

            // sonido aqui
            Entity *e = new Entity();
            e->settings(sExplosion, a->x, a->y);
            e->name = "explosion";
            entities.push_back(e);

            !gameover ? score += 1 : score += 0;
          }
        if (a->name == "asteroid" && b->name == "Shield")
          if (isCollide(a, b)) {
            a->life = false;

            // sonido aqui
            Entity *e = new Entity();
            e->settings(sExplosion, a->x, a->y);
            e->name = "explosion";
            entities.push_back(e);
            !gameover ? score += 1 : score += 0;
          }

        if (a->name == "player" && b->name == "shell")
          if (isCollide(a, b)) {
            /*Entity *e = new Entity();
            e->settings(sShield, b->x, b->y, b->angle, 100);
            e->name = "Shield";
            e->start = reloj.getElapsedTime();
            entities.push_back(e);*/
            powers.push_back(new Power("Shield", "", sShield));
            b->life = 0;
          }
        if (a->name == "player" && b->name == "life")
          if (isCollide(a, b)) {
            b->life = false;
            p->life += 1;
          }
        if (a->name == "player" && b->name == "invi")
          if (isCollide(a, b)) {
            powers.push_back(new Power("Invi", "invi.wav"));
            b->life = 0;
          }
        if (a->name == "player" && b->name == "arma")
          if (isCollide(a, b)) {
            powers.push_back(new Power("Red", ""));
            b->life = 0;
          }

        if (a->name == "player" && b->name == "asteroid" && p->visibility)
          if (isCollide(a, b)) {
            b->life = false;

            Entity *e = new Entity();
            e->settings(sExplosion_ship, a->x, a->y);
            e->name = "explosion";
            entities.push_back(e);
            p->life -= 1;
            if (p->life == false) {
              music.stop();
              blazer.stop();
              p->settings(sPlayer, W / 2, H / 2, 0, 20);
              p->dx = 0;
              p->dy = 0;
              p->life = 1;
              gameover = true;
            }
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
        if (e->updateTime.asSeconds() - e->start.asSeconds() > 20.f) {
          e->life = 0;
        } else {
          e->updateTime = reloj.getElapsedTime();
        }
      }
      if (e->name == "Invi") {
        if (e->updateTime.asSeconds() - e->start.asSeconds() > 10.f) {
          e->life = 0;
          p->visibility = true;
          songPower.stop();
        } else {
          e->updateTime = reloj.getElapsedTime();
        }
      }
      if (e->name == "Red") {
        if (e->updateTime.asSeconds() - e->start.asSeconds() > 10.f) {
          e->life = 0;
          p->powerType = "blue";
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
    float TimeDuration;
    TimeDuration =
        !gameover ? tempTime.asSeconds() - duration.asSeconds() : TimeDuration;
    //////draw//////
    if (!gameover) {
      TimerGame.setString("Duration: " + std::to_string(TimeDuration));
      lifeTime.setString("life:" + std::to_string(p->life));
      ScoreGame.setString("Score:" + std::to_string(score));
      PowersText.setString(
          "<\t" + (powers.empty() ? "None" : powers.at(0)->name) + "\t>");
      app.draw(background);
      for (auto i : entities) {
        i->draw(app);
      }
      app.draw(rectangle);
      app.draw(TimerGame);
      app.draw(lifeTime);
      app.draw(ScoreGame);
      app.draw(PowersText);
    } else {

      // Text
      sf::Text GameOver;
      GameOver.setFont(font);
      GameOver.setString("GAME OVER \n\nScore: " + std::to_string(score) +
                         " \n\nTime:" + std::to_string(TimeDuration));
      GameOver.setFillColor(sf::Color(255, 255, 255, 255));
      GameOver.setCharacterSize(50);
      GameOver.setPosition(W / 2, H / 2);
      // background
      sf::RectangleShape rectangleOver(Vector2f(W + 50, RealH));
      rectangleOver.setFillColor(Color::Black);
      rectangleOver.setPosition(0, 0);

      // Draw
      app.draw(rectangleOver);
      app.draw(GameOver);
    }
    app.display();
  }

  return 0;
}
