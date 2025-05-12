# Asteroids

`Asteroids is a space-themed multidirectional shooter arcade video game designed by Lyle Rains and Ed Logg released in November 1979 by Atari, Inc.[4].`

> This project is focused as a learning project on the use of the SFML library for graphic management and the use of classes and polymorphism.

## Instructions
### Game controls - Keyboard

|  Key  | Value  | Value as SFML reference  |
|---|---|---|
| w  | Up  | Keyboard::W  |
| s  | Down  | Keyboard::S  |
| a  | Left  | Keyboard::A  |
| d  | Right  | Keyboard::D  |
| space  | shot  | Keyboard::Space  |

### Game controls - xbox/ps/generic controller

> Add Iamge controller. https://en.sfml-dev.org/forums/index.php?topic=16748.msg120279#msg120279

## NOTE:
This project is a game developed in C++ using the SFML library, specifically version 2.x. It is designed to work **only** with SFML 2.x due to breaking changes in SFML 3.x.

### 🛠 Installation on Arch Linux (or derivatives) with `yay`

1. **Install SFML 2.6 from the AUR:**

```bash
yay -S sfml2
````

> This will install SFML 2.6 without interfering with any existing SFML 3 installation.

2. **Install gamepad drivers (optional but recommended):**

* For Xbox and compatible controllers:

```bash
yay -S xpad-dkms
```

* For extended support (e.g., Xbox One, Xbox Series X):

```bash
yay -S xpadneo-dkms
```

## TODO (2023/09/11)

- Text of game time, score and relevant superpowers data. (BUFFERS).
- Add superpowers like COD zombies (list of superpowers to be collected and used as needed).
- Apply movement to asteroids when changing map (bezier).
- Create a sequential map structure, such as adventure.
- Migrate from SFML 2.x to the latest version of SFML
