#include <math.h>

void bezier(int x[4], int y[4]) {

  int i;

  double t;

  for (t = 0.0; t < 1.0; t += 0.05)

  {

    double xt = pow(1 - t, 3) * x[0] + 3 * t * pow(1 - t, 2) * x[1] +
                3 * pow(t, 2) * (1 - t) * x[2] + pow(t, 3) * x[3];

    double yt = pow(1 - t, 3) * y[0] + 3 * t * pow(1 - t, 2) * y[1] +
                3 * pow(t, 2) * (1 - t) * y[2] + pow(t, 3) * y[3];

    pixel(xt, yt, 'A', 0, 12);

    for (int i = 0; i < 4; ++i)

    {

      pixel(x[i], y[i], 'o', 0, 4);
    }
  }
}
