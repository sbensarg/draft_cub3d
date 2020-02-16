#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mlx.h>
#include <math.h>
#define KEY_UP 126
#define KEY_DOWN 125
#define KEY_LEFT 123
#define KEY_RIGHT 124

#define KEY_W 13
#define KEY_S 1
#define KEY_A 0
#define KEY_D 2
#define PI 3.14159

const		int tile_size = 32;
const		int map_num_rows = 11;
const		int map_num_cols = 15;
const		int window_width = map_num_cols * tile_size;
const		int window_height = map_num_rows * tile_size;
const		int ray_width = 1;
const		int num_of_rays = window_width / ray_width;
const		float fov_ang = 60 * (PI / 180);
const		float	angleinc = (60 * (PI / 180)) / num_of_rays;
const		float mini_map_scale_fact = 0.2;
int			angleisup = 0;
int			angleisleft = 0;
int			angleisdown = 0;
int			angleisright = 0;

int grid[11][15] = {
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
		{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
		{1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1},
		{1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
	};

typedef struct
{
	void          *mlx_ptr;
	void          *mlx_win;
}					data_t;
typedef struct
{
	float					    x;
	float 				    y;
	float 			   radius;
	float 		turndirection;
	float		walkdirection;
	float 		rotationangle;
	float 			movespeed;
	float 		rotationspeed;
}					 player_t;

player_t player;
data_t        data;
void	vec(data_t data, int tileX, int tileY, int color)
{
	int	i;
	int	j;

	i = -3;
	j = -3;
	while (i < 3)
	{
		while (j < 3)
		{
			if(j == 0 || i == 0)
			{
				mlx_pixel_put(data.mlx_ptr, data.mlx_win, tileX + i, tileY + j, 0x00FFFFFF);
			}
			else
			{
				mlx_pixel_put(data.mlx_ptr, data.mlx_win, tileX + i, tileY + j, color);
			}
			
			j++;
		}
		j = -3;
		i++;
	}
}

void init_player(void)
{
	player.x = window_width / 2;
	player.y = window_height / 2;
 	player.radius = 3;
	player.turndirection = 0;
	player.walkdirection = 0;
	player.rotationangle = PI / 2;
	player.movespeed = 8;
	player.rotationspeed = 10 * (PI / 180);
}

int		nextposallowed(float newplayerx, float newplayery);

void	checkangle(float angle)
{
	if (angle > 0 && angle < PI)
		angleisup = 1;
	else
		angleisdown = 1;
	
	if ((angle > 0 && angle < PI / 2) || (angle > 1.5 * PI && angle < 2 * PI))
		angleisleft = 1;
	else
		angleisright = 1;
}

int		iswall(float x,	float y)
{
	int	gridx;
	int	gridy;
	
	if (x < 0 || x > window_width || y < 0 || y > window_height)
		return (1);
	gridx = floor(x / tile_size);
	gridy = floor(y / tile_size);

	return (grid[gridy][gridx]);
}

float	putray(float angle)
{
	float		i;
	float	xa;
	float	ya;
	float	xva;
	float	yva;
	float	xstep;
	float	ystep;
	float	hlenght;
	float	vlenght;
	int		b;

	i = 0;
	angle = fmod(angle, 2 * PI);
	if (angle < 0)
		angle = (2 * PI) + angle;
	checkangle(angle);
	if (angleisup)
	{
		ya = floor(player.y / tile_size) * tile_size;
		xa = player.x - ((player.y - ya) / tan(angle));
	}
	else
	{
		ya = ceil(player.y / tile_size) * tile_size;
		xa = player.x + ((ya - player.y) / tan(angle));
	}

	if (angleisleft)
	{
		xva = floor(player.x / tile_size) * tile_size;
		yva = player.y - ((player.x - xva) * tan(angle));
	}
	else
	{
		xva = ceil(player.x / tile_size) * tile_size;
		yva = player.y + ((xva - player.x) * tan(angle));
	}
	xstep = tile_size / tan(angle);
	ystep = tile_size * tan(angle);
	while (1)
	{
		if (angleisup && angleisright)
		{
			if ((b = iswall(xa + 1 , ya - 1)) == 1)
				break;
			xa -= xstep;
			ya -= tile_size;
		}
		else if (angleisup && angleisleft)
		{
			if ((b = iswall(xa - 1 , ya - 1)) == 1)
				break;
			xa -= xstep;
			ya -= tile_size;
		}
		else if (angleisdown && angleisleft)
		{
			if ((b = iswall(xa - 1 , ya + 1)) == 1)
				break;
			xa += xstep;
			ya += tile_size;
		}
		else if (angleisdown && angleisright)
		{
			if ((b = iswall(xa + 1 , ya + 1)) == 1)
				break;
			xa += xstep;
			ya += tile_size;
		}
	}
	while (1)
	{
		if (angleisup && angleisright)
		{
			
			if ((b = iswall(xva + 1 , yva - 1)) == 1)
				break;
				xva += tile_size;
			yva += ystep;
		}
		else if (angleisup && angleisleft)
		{
			
			if ((b = iswall(xva - 1 , yva - 1)) == 1)
				break;
				xva -= tile_size;
			yva -= ystep;
		}
		else if (angleisdown && angleisleft)
		{
			
			if ((b = iswall(xva - 1 , yva + 1)) == 1)
				break;
				xva -= tile_size;
			yva -= ystep;
		}
		else if (angleisdown && angleisright)
		{
			
			if ((b = iswall(xva + 1 , yva + 1)) == 1)
				break;
				xva += tile_size;
			yva += ystep;
		}
	}
	hlenght = sqrt(pow(fabs(player.x - xva), 2) + pow(fabs(player.y - yva), 2));
	vlenght = sqrt(pow(fabs(player.x - xa), 2) + pow(fabs(player.y - ya), 2));
	if (hlenght < vlenght)
		i = hlenght;
	else
		i = vlenght;

	while (i > 0)
	{
		xa = player.x - cos(angle) * (i);
		ya = player.y - sin(angle) * (i);
		mlx_pixel_put(data.mlx_ptr, data.mlx_win, mini_map_scale_fact * xa, mini_map_scale_fact * ya, 0x00FCC25F);	
		i -= 1;
	}
	angleisup = 0;
	angleisleft = 0;
	angleisdown = 0;
	angleisright = 0;
	return (i);
}

void	rect(data_t data, int tileX, int tileY, int color)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (i < tile_size)
	{
		while (j < tile_size)
		{
			if(j == 0 || i == 0)
			{
				mlx_pixel_put(data.mlx_ptr, data.mlx_win, (tileX + i) * mini_map_scale_fact, (tileY + j) * mini_map_scale_fact, 0x00FFFFFF);
			}
			else
			{
				mlx_pixel_put(data.mlx_ptr, data.mlx_win, (tileX + i) * mini_map_scale_fact, (tileY + j) * mini_map_scale_fact, color);
			}
			
			j++;
		}
		j = 0;
		i++;
	}
}

void	render_map(data_t data)
{
	int tileX;
	int tileY;
	int tileColor;
	int	i;
	int	j;

	i = 0;
	j = 0;
	while(i < map_num_rows)
	{
		while(j < map_num_cols)
		{
			tileX = j * tile_size;
			tileY = i * tile_size;
			tileColor = grid[i][j] == 1 ? 0x00FF7F7F : 0x00FFFFFF;
			if (grid[i][j] == 1)
				rect(data, tileX, tileY, tileColor);
			/*else
			{
				rect(data, tileX, tileY, 0x0);
			}*/
			
			j++;
		}
		j = 0;
		i++;
	}
}

void	render_rays()
{
	float	first_ray_ang;
	float	last_ray_ang;
	float	ray_lenght;

	first_ray_ang = player.rotationangle - (30 * (PI / 180));
	last_ray_ang = player.rotationangle + (30 * (PI / 180));
	while (first_ray_ang <= last_ray_ang)
	{
		ray_lenght = putray(first_ray_ang);
		first_ray_ang += angleinc;
	}
}
/*
void render3dprojectedwalls()
{
	float	first_ray_ang;
	float	last_ray_ang;
	float distanceprojectionplane;
	float	wallstripheight;
	float	ray_lenght;
	int i;
	i = 0;
	first_ray_ang = player.rotationangle - (30 * (PI / 180));
	last_ray_ang = player.rotationangle + (30 * (PI / 180));
	while (first_ray_ang <= last_ray_ang)
	{
		ray_lenght = putray(first_ray_ang);
		first_ray_ang += angleinc;

		distanceprojectionplane = (window_width / 2) / tan(fov_ang / 2);
		wallstripheight = (tile_size / ray_lenght) * distanceprojectionplane;
		rect(data, i * ray_width, (window_height / 2) - (wallstripheight / 2),0x00FCC25F);
	}
}
*/
void    render_player(data_t data, int r, int color)
{
    float	ac;
    float	ab;
    float	bc;
	float		i;
	int		xa;
	int		ya;
    
	ac = r;
	i = 0;
    while (ac > 0)
    {
	
        ab = r;
        bc = sqrt(pow(ab, 2) - pow(ac, 2));
        mlx_pixel_put(data.mlx_ptr, data.mlx_win, mini_map_scale_fact * (ac + player.x), mini_map_scale_fact * (player.y + bc), color);
        mlx_pixel_put(data.mlx_ptr, data.mlx_win, mini_map_scale_fact * (ac + player.x), mini_map_scale_fact * (player.y - bc), color);
        mlx_pixel_put(data.mlx_ptr, data.mlx_win, mini_map_scale_fact * (player.x - ac), mini_map_scale_fact * (player.y + bc), color);
        mlx_pixel_put(data.mlx_ptr, data.mlx_win, mini_map_scale_fact * (player.x - ac), mini_map_scale_fact * (player.y - bc), color);
        ac -= 0.01;
    }
	while (i < 15)
	{
		xa = player.x - cos(player.rotationangle) * (i);
		ya = player.y - sin(player.rotationangle) * (i);
		mlx_pixel_put(data.mlx_ptr, data.mlx_win, mini_map_scale_fact * xa,mini_map_scale_fact * ya, 0x00FFFFFF);
		i += 0.01;
	}
}


int		nextposallowed(float newplayerx, float newplayery)
{
	int	gridx;
	int	gridy;

	if (newplayerx < 0 || newplayerx > window_width || newplayery < 0 || newplayery > window_height)
		return (1);
	if (player.x - newplayerx <= 0)
		newplayerx += 5;
	else
		newplayerx -= 5;
	if (player.y - newplayery <= 0)
		newplayery += 5;
	else
		newplayery -= 5;
	
	gridx = floor(newplayerx / tile_size);
	gridy = floor(newplayery / tile_size);

	return (grid[gridy][gridx]);
}

void	update()
{
	float		movestep;
	float newplayerx;
	float newplayery;

	movestep += player.walkdirection * player.movespeed;
	newplayerx = player.x + cos(player.rotationangle) * movestep;
	newplayery = player.y + sin(player.rotationangle) * movestep;
	if (nextposallowed(newplayerx, newplayery) == 0)
	{
		player.x = newplayerx;
		player.y = newplayery;
	}
	if (player.turndirection == 1)
		player.rotationangle += player.rotationspeed;
	if (player.turndirection == -1)
		player.rotationangle -= player.rotationspeed;
	mlx_clear_window(data.mlx_ptr, data.mlx_win);
	render_map(data);
	render_rays();
	render_player(data, 5, 0x00FF7FFF);
}

int		ft_keyPressed(int keycode,void *param)
{
	if(keycode == KEY_UP)
	{
		player.walkdirection = -1;
	}
	if (keycode == KEY_DOWN)
		player.walkdirection = +1;
	if (keycode == KEY_RIGHT)
		player.turndirection = +1;
	if (keycode == KEY_LEFT)
		player.turndirection = -1;
	update();
	//printf("[%f, %f], %f\n", player.x, player.y, player.rotationangle);
	return (0);
}

int		ft_keyReleased(int keycode,void *param)
{
	if(keycode == KEY_UP)
		player.walkdirection = 0;
	if (keycode == KEY_DOWN)
		player.walkdirection = 0;
	if (keycode == KEY_RIGHT)
		player.turndirection = 0;
	if (keycode == KEY_LEFT)
		player.turndirection = 0;
	return (0);
}

int main(void)
{
	int i = 0;
	int j = 0;
	init_player();

	if ((data.mlx_ptr = mlx_init()) == NULL)
		return (EXIT_FAILURE);
	if ((data.mlx_win = mlx_new_window(data.mlx_ptr, window_width, window_height, "Hello world")) == NULL)
		return (EXIT_FAILURE);
	render_map(data);
	render_player(data, 5, 0x00FF7FFF);
	render_rays();
	render3dprojectedwalls();
	mlx_hook(data.mlx_win, 2, 0, ft_keyPressed, (void *)0);
	mlx_hook(data.mlx_win, 3, 0, ft_keyReleased, (void *)0);
	mlx_loop(data.mlx_ptr);
	return (EXIT_SUCCESS);
}
