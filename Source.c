#define _CRT_SECURE_NO_WARNINGS
#define M_PI 3.14159265358979323846
#define WIDTH 100
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include "Molecule.h"
#include "Dust.h"

molecule * GenerateModel(object dust, int count)
{
	molecule *a;
	int i;
	a = (molecule*)malloc(count * sizeof(molecule));
	for (i = 0; i < count; i++)
	{
	while (1)
	{
	a[i].x = rand() % 100;
	a[i].y = rand() % 100;
	if (sqrt(pow(a[i].x - dust.x, 2) + pow(a[i].y - dust.y, 2)) <= dust.radius)
	continue;
	else
	break;
	}
	a[i].speed = rand() % 10;
	a[i].speed = a[i].speed / 200;
	a[i].angle = (rand() % 360) * M_PI / 360;
	a[i].mass = 1;
	}

	/*a[0].x = 0;
	a[0].y = 50;
	a[0].speed = 5;
	a[0].speed = a[0].speed / 200;
	a[0].angle = 0;
	a[0].mass = 1;*/
	return a;
}

object GenerateObject()
{
	object dust;
	dust.x = 50;
	dust.y = 50;
	dust.radius = 25;
	dust.speed = 0;
	dust.angle = 0;
	dust.mass = 100;
	return dust;
}

int ColisionCheckXrigth(molecule a)
{
	return (a.x + a.speed*cos(a.angle) > WIDTH);
}
int ColisionCheckXLeft(molecule a)
{
	return (a.x + a.speed*cos(a.angle) < 0);
}
int ColisionCheckYtop(molecule a)
{
	return (a.y + a.speed*sin(a.angle) > WIDTH);
}
int ColisionCheckYbottom(molecule a)
{
	return (a.y + a.speed*sin(a.angle) < 0);
}
int ColisionCheckXrigthDust(object dust)
{
	return (dust.x + dust.speed*cos(dust.angle) > WIDTH - dust.radius);
}
int ColisionCheckXLeftDust(object dust)
{
	return (dust.x + dust.speed*cos(dust.angle) < dust.radius);
}
int ColisionCheckYtopDust(object dust)
{
	return (dust.y + dust.speed*sin(dust.angle) > WIDTH - dust.radius);
}
int ColisionCheckYbottomDust(object dust)
{
	return (dust.y + dust.speed*sin(dust.angle) < dust.radius);
}

int ColisionCheckDust(molecule a, object dust)
{
	double len, impulsex, impulsey;
	impulsex = a.mass*a.speed*cos(a.angle) + dust.mass*dust.speed*cos(dust.angle);
	impulsey = a.mass*a.speed*sin(a.angle) + dust.mass*dust.speed*sin(dust.angle);
	len = sqrt((dust.x + dust.speed*cos(dust.angle) - (a.x + a.speed*cos(a.angle)))*(dust.x + dust.speed*cos(dust.angle) - (a.x + a.speed*cos(a.angle))) + (dust.y + dust.speed*sin(dust.angle) - (a.y + a.speed*sin(a.angle)))*(dust.y + dust.speed*sin(dust.angle) - (a.y + a.speed*sin(a.angle))));
	//printf("distance = %f\t impulse X = %f\t impulse Y = %f\n", len - dust.radius, impulsex, impulsey);
	return (len <= dust.radius);
}
double AngleStandartization(double a)
{
	if (a > M_PI)
		a = a - 2 * M_PI;
	else
		if (a < -M_PI)
			a = a + 2 * M_PI;
	return a;
}
int ColisionType(molecule a, object dust)
{
	double k0, b0, d, k1, b1, x1, x2, r, y1, y2, k2, k3, b3;
	if (!(fabs(cos(a.angle)) <= 0.0001))
	{
		k1 = tan(a.angle);
		b1 = a.y - k1*a.x;//вертикальный сдвиг этой прямой
		r = dust.radius;
		d = 4 * (pow(k1*b1 - dust.x - k1*dust.y, 2)) - 4 * (1 + k1)*(dust.x*dust.x + b1*b1 - 2 * b1*dust.y + dust.y*dust.y - r*r);
		x1 = (-2 * (k1*b1 - dust.x - k1*dust.y) + sqrt(d)) / (2 * (1 + k1));
		x2 = (-2 * (k1*b1 - dust.x - k1*dust.y) - sqrt(d)) / (2 * (1 + k1));
		if (fabs(a.x - x1) > fabs(a.x - x2))
			x1 = x2;//абсцисса точки удара
		y1 = k1*x1 + b1;//ордината точки удара
	}
	else
	{
		x1 = a.x;
		y1 = dust.y + sqrt(2 * dust.x*x1 - x1*x1 - dust.x*dust.x + dust.radius*dust.radius);
		y2 = dust.y - sqrt(2 * dust.x*x1 - x1*x1 - dust.x*dust.x + dust.radius*dust.radius);
		if (fabs(a.y - y1) > fabs(a.y - y2))
			y1 = y2;
	}
	printf("x1 %f, y1 %f\n", x1, y1);
	if (x1 >= dust.x)
		if (y1 >= dust.y)
			return 1;
		else
			return 2;
	else
		if (y1 >= dust.y)
			return 3;
		else
			return 4;
}

object Movement(molecule a[], object dust, int count)
{
	int i, tmp, bool_y, colided = -1;
	double x1, x2, y1, y2, k2, r, len, k1, k0, b1, d, angle, len1, A, C, D, a1, b, v21, v22, B, impulse_beforeX, impulse_afterX, impulse_beforeY, impulse_afterY, tmp_speed, k_dust, tmp_angle;
	for (i = 0; i < count; i++)
	{
		if (ColisionCheckDust(a[i], dust) == 1)
		{
			colided = i;
			printf("%d\n", i);
		}
		else
		{
			if (ColisionCheckXrigth(a[i]) == 1)
				if (ColisionCheckYtop(a[i]) == 1)
				{
					a[i].x = 2 * WIDTH - (a[i].x + a[i].speed*cos(a[i].angle));
					a[i].y = 2 * WIDTH - (a[i].y + a[i].speed*sin(a[i].angle));
					a[i].angle = a[i].angle + M_PI;
				}
				else
					if (ColisionCheckYbottom(a[i]) == 1)
					{
						a[i].x = 2 * WIDTH - (a[i].x + a[i].speed*cos(a[i].angle));
						a[i].y = -(a[i].y + a[i].speed*sin(a[i].angle));
						a[i].angle = a[i].angle + M_PI;
					}
					else
					{
						a[i].x = 2 * WIDTH - (a[i].x + a[i].speed*cos(a[i].angle));
						a[i].y = a[i].y + a[i].speed * sin(a[i].angle);
						a[i].angle = M_PI - a[i].angle;
					}
			else
				if (ColisionCheckXLeft(a[i]) == 1)
					if (ColisionCheckYtop(a[i]) == 1)
					{
						a[i].x = -(a[i].x + a[i].speed*cos(a[i].angle));
						a[i].y = 2 * WIDTH - (a[i].y + a[i].speed*sin(a[i].angle));
						a[i].angle = a[i].angle + M_PI;
					}
					else
						if (ColisionCheckYbottom(a[i]) == 1)
						{
							a[i].x = -(a[i].x + a[i].speed*cos(a[i].angle));
							a[i].y = -(a[i].y + a[i].speed*sin(a[i].angle));
							a[i].angle = a[i].angle + M_PI;
						}
						else
						{
							a[i].x = -(a[i].x + a[i].speed*cos(a[i].angle));
							a[i].y = a[i].y + a[i].speed*sin(a[i].angle);
							a[i].angle = M_PI - a[i].angle;
						}
				else
					if (ColisionCheckYtop(a[i]) == 1)
					{
						a[i].x = a[i].x + a[i].speed * cos(a[i].angle);
						a[i].y = 2 * WIDTH - (a[i].y + a[i].speed*sin(a[i].angle));
						a[i].angle = 2 * M_PI - a[i].angle;
					}
					else
						if (ColisionCheckYbottom(a[i]) == 1)
						{
							a[i].x = a[i].x + a[i].speed * cos(a[i].angle);
							a[i].y = -(a[i].y + a[i].speed*sin(a[i].angle));
							a[i].angle = 2 * M_PI - a[i].angle;
						}
						else
						{
							a[i].x = a[i].x + a[i].speed * cos(a[i].angle);
							a[i].y = a[i].y + a[i].speed * sin(a[i].angle);
						}
		}
		a[i].angle = AngleStandartization(a[i].angle);
	}
	i = colided;
	if (colided != -1)
	{
		if (!(cos(a[i].angle) <= 0.0001 && cos(a[i].angle) >= -0.0001))
		{
			bool_y = 0;
			k1 = tan(a[i].angle);
			b1 = a[i].y - k1*a[i].x;
			r = dust.radius;
			d = 4 * (pow(k1*b1 - dust.x - k1*dust.y, 2)) - 4 * (1 + k1*k1)*(dust.x*dust.x + b1*b1 - 2 * b1*dust.y + dust.y*dust.y - r*r);
			x1 = (-2 * (k1*b1 - dust.x - k1*dust.y) + sqrt(d)) / (2 * (1 + k1*k1));
			x2 = (-2 * (k1*b1 - dust.x - k1*dust.y) - sqrt(d)) / (2 * (1 + k1*k1));
			if (fabs(a[i].x - x1) > fabs(a[i].x - x2))
				x1 = x2;
			y1 = k1*x1 + b1;
		}
		else
		{
			bool_y = 1;
			x1 = a[i].x;
			y1 = dust.y + sqrt(2 * dust.x*x1 - x1*x1 - dust.x*dust.x + dust.radius*dust.radius);
			y2 = dust.y - sqrt(2 * dust.x*x1 - x1*x1 - dust.x*dust.x + dust.radius*dust.radius);
			if (fabs(a[i].y - y1) > fabs(a[i].y - y2))
				y1 = y2;
		}
		k_dust = (y1 - dust.y) / (x1 - dust.x);//тангенс под которым полетит пылинка
		if (x1 > dust.x)
			tmp_angle = M_PI + atan(k_dust);
		else
			tmp_angle = atan(k_dust);
		if (x1 == dust.x)
			k2 = 0;
		else
			k2 = -1 / (y1 - dust.y) * (x1 - dust.x);
		len1 = sqrt(a[i].speed*cos(a[i].angle)*a[i].speed*cos(a[i].angle) + a[i].speed*sin(a[i].angle)*a[i].speed*sin(a[i].angle));
		len = sqrt((x1 - a[i].x - a[i].speed*cos(a[i].angle))*(x1 - a[i].x - a[i].speed*cos(a[i].angle)) + (y1 - a[i].y - a[i].speed*sin(a[i].angle))*(y1 - a[i].y - a[i].speed*sin(a[i].angle)));
		//printf("len1/len %f/%f\n", len1, len);
		if (len > len1)
			printf("SOME ERROR OCCURED WHILE COUNTING LENGTH IN MOLECULE %d\n", i);
		switch (ColisionType(a[i], dust))
		{
		case 1:
			angle = -M_PI / 2 - atan(k2);
			break;
		case 2:
			angle = M_PI / 2 - atan(k2);
			break;
		case 3:
			angle = M_PI / 2 - atan(k2);
			break;
		case 4:
			angle = -M_PI / 2 - atan(k2);
			break;
		}
		impulse_beforeX = a[i].mass*a[i].speed*cos(a[i].angle) + dust.mass*dust.speed*cos(dust.angle);
		impulse_beforeY = a[i].mass*a[i].speed*sin(a[i].angle) + dust.mass*dust.speed*sin(dust.angle);
		A = a[i].mass*a[i].mass*cos(dust.angle)*cos(dust.angle) + a[i].mass*dust.mass*cos(tmp_angle)*cos(tmp_angle);
		B = -2 * a[i].mass*cos(dust.angle)*(a[i].mass*a[i].speed*cos(a[i].angle) + dust.mass*dust.speed*cos(-2 * angle - a[i].angle));
		C = a[i].mass*a[i].speed*(a[i].mass*a[i].speed*cos(a[i].angle)*cos(a[i].angle) + 2 * dust.mass*dust.speed*cos(-2 * angle - a[i].angle)*cos(dust.angle) - dust.mass*a[i].speed*cos(tmp_angle)*cos(tmp_angle)) + dust.mass*dust.mass*dust.speed*dust.speed*(cos(-2 * angle - a[i].angle)*cos(-2 * angle - a[i].angle) + cos(tmp_angle)*cos(tmp_angle));
		D = B*B - 4 * A*C;
		tmp_speed = a[i].speed;
		a[i].speed = fabs((-B - sqrt(D)) / (2 * A));
		dust.speed = fabs((a[i].mass*tmp_speed*cos(a[i].angle) + dust.mass*dust.speed*cos(dust.angle) - a[i].mass*a[i].speed*cos(-2 * angle - a[i].angle)) / (dust.mass*cos(tmp_angle)));
		a[i].angle = M_PI - 2 * angle - a[i].angle;
		a[i].angle = AngleStandartization(a[i].angle);
		dust.angle = tmp_angle;
		a[i].x = a[i].x + len1*cos(a[i].angle);
		a[i].y = a[i].y + len1*sin(a[i].angle);
		dust.x = dust.x + dust.speed*cos(dust.angle);
		dust.y = dust.y + dust.speed*sin(dust.angle);
		impulse_afterX = a[i].mass*a[i].speed*cos(a[i].angle) + dust.mass*dust.speed*cos(dust.angle);
		impulse_afterY = a[i].mass*a[i].speed*sin(a[i].angle) + dust.mass*dust.speed*sin(dust.angle);
		//printf("impulse on X before/after %f/%f\n", impulse_beforeX, impulse_afterX);
		//printf("impulse on Y before/after %f/%f\n", impulse_beforeY, impulse_afterY);
		//printf("full impulse before/after %f/%f\n", impulse_beforeX + impulse_beforeY, impulse_afterX + impulse_afterY);
		//getchar();
	}
	if (colided == -1)
	{
		if (ColisionCheckXrigthDust(dust) == 1)
			if (ColisionCheckYtopDust(dust) == 1)
			{
				dust.x = 2 * (WIDTH - 25) - (dust.x + dust.speed*cos(dust.angle));
				dust.y = 2 * (WIDTH - 25) - (dust.y + dust.speed*sin(dust.angle));
				dust.angle = dust.angle + M_PI;
			}
			else
				if (ColisionCheckYbottomDust(dust) == 1)
				{
					dust.x = 2 * (WIDTH - 25) - (dust.x + dust.speed*cos(dust.angle));
					dust.y = dust.radius - (dust.y + dust.speed*sin(dust.angle));
					dust.angle = dust.angle + M_PI;
				}
				else
				{
					dust.x = 2 * (WIDTH - 25) - (dust.x + dust.speed*cos(dust.angle));
					dust.y = dust.radius + dust.y + dust.speed * sin(dust.angle);
					dust.angle = M_PI - dust.angle;
				}
		else
			if (ColisionCheckXLeftDust(dust) == 1)
				if (ColisionCheckYtopDust(dust) == 1)
				{
					dust.x = dust.radius - (dust.x + dust.speed*cos(dust.angle));
					dust.y = 2 * (WIDTH - 25) - (dust.y + dust.speed*sin(dust.angle));
					dust.angle = dust.angle + M_PI;
				}
				else
					if (ColisionCheckYbottomDust(dust) == 1)
					{
						dust.x = dust.radius - (dust.x + dust.speed*cos(dust.angle));
						dust.y = dust.radius - (dust.y + dust.speed*sin(dust.angle));
						dust.angle = dust.angle + M_PI;
					}
					else
					{
						dust.x = dust.radius - (dust.x + dust.speed*cos(dust.angle));
						dust.y = dust.y + dust.speed*sin(dust.angle);
						dust.angle = M_PI - dust.angle;
					}
			else
				if (ColisionCheckYtopDust(dust) == 1)
				{
					dust.x = dust.radius + dust.x + dust.speed * cos(dust.angle);
					dust.y = 2 * (WIDTH - 25) - (dust.y + dust.speed*sin(dust.angle));
					dust.angle = 2 * M_PI - dust.angle;
				}
				else
					if (ColisionCheckYbottomDust(dust) == 1)
					{
						dust.x = dust.radius + dust.x + dust.speed * cos(dust.angle);
						dust.y = dust.radius - (dust.y + dust.speed*sin(dust.angle));
						dust.angle = 2 * M_PI - dust.angle;
					}
					else
					{
						dust.x = dust.x + dust.speed * cos(dust.angle);
						dust.y = dust.y + dust.speed * sin(dust.angle);
					}
	}
	dust.angle = AngleStandartization(dust.angle);
	return dust;
}
void ParametersCheck(molecule a[], object dust, int count)
{
	int i;
	for (i = 0; i < count; i++)
	{
		if (a[i].x > WIDTH || a[i].x < 0)
		{
			printf("SOME ERROR OCCURED WITH MOLECULE NUMBER %d IN X AXIS\n", i);
			continue;
		}
		else
			if (a[i].y > WIDTH || a[i].y < 0)
			{
				printf("SOME ERROR OCCURED WITH MOLECULE NUMBER %d IN Y AXIS\n", i);
				continue;
			}
	}
}
void Spectate(molecule a[], object dust, int count)
{
	int i;
	printf("Molecules:\n");
	printf("|     x     |     y     |   speed  |   angle   |\n");
	for (i = 0; i < count; i++)
	{
		printf("|%10f |%10f |%10f|%11f|\n", a[i].x, a[i].y, a[i].speed, a[i].angle / M_PI * 180);
	}
	printf("Object:\n");
	printf("|     x     |     y     |   speed  |   angle   |\n");
	printf("|%10f |%10f |%10f|%11f|\n", dust.x, dust.y, dust.speed, dust.angle / M_PI * 180);
}
int main()
{
	molecule *a;
	object dust = GenerateObject();
	int i, count;
	char c;
	printf("Enter molecules number: ");
	scanf("%d", &count);
	getchar();
	a = GenerateModel(dust, count);
	while (1)
	{
		for (i = 0; i < 200; i++)
		{
			ParametersCheck(a, dust, count);
			dust = Movement(a, dust, count);
		}
		Spectate(a, dust, count);
		printf("\npress any button to continue");
		scanf("%c", &c);
		if (c == '\n')
			continue;
		else
			break;
	}
	free(a);
	return 0;
}