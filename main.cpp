#include <iostream>
#include <cmath>

#include <SDL2/SDL.h>

#define MAP_WIDTH 24
#define MAP_HEIGHT 24

SDL_Color side_color(SDL_Color color) {
	color.r /= 2;
	color.g /= 2;
	color.b /= 2;
	return color;
}

int world_map[MAP_WIDTH][MAP_HEIGHT] =
{
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

int main(int argc, char* argv[]) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "SDL could not be initialized. SDL Error: " << SDL_GetError() << '\n';
		return -1;
	}

	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 800;

	const int SCREEN_FPS = 60;
	const int SCREEN_TICKS_PER_FRAME = 1000/SCREEN_FPS;
	double pos_x = 22, pos_y = 12; // position of the player
	double dir_x = -1, dir_y = 0; // direction the player is looking at
	double plane_x = 0, plane_y = 0.66; // 2d raycaster version of camera plane

	SDL_Window* window = SDL_CreateWindow("Raycaster With SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (window == NULL) {
		std::cout << "Window could not be initialized. SDL Error: " << SDL_GetError() << '\n';
		return -1;
	}

	if (renderer == NULL) {
		std::cout << "Renderer could not be initialied. SDL Error: " << SDL_GetError() << '\n';
		return -1;
	}

	bool quit = false;
	SDL_Event e;

	Uint32 frame_start;
	Uint32 frame_time;

	double move_speed = 0.1;
	double rot_speed = 0.05;

	double old_dir_x;
	double old_plane_x;

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	while (!quit) {
		frame_start = SDL_GetTicks();

		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				quit = true;
				break;
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		for (int x = 0; x < SCREEN_WIDTH; x++) {
			double camera_x = 2 * x / double(SCREEN_WIDTH) - 1;
			double ray_dir_x = dir_x + plane_x * camera_x;
			double ray_dir_y = dir_y + plane_y * camera_x;

			int map_x = int(pos_x);
			int map_y = int(pos_y);

			double side_dist_x;
			double side_dist_y;

			double delta_dist_x = (ray_dir_x == 0) ? 1e30 : std::abs(1 / ray_dir_x);
			double delta_dist_y = (ray_dir_y == 0) ? 1e30 : std::abs(1 / ray_dir_y);
			double perp_wall_dist;

			int step_x;
			int step_y;

			int hit = 0;
			int side;

			if (ray_dir_x < 0) {
				step_x = -1;
				side_dist_x = (pos_x - map_x) * delta_dist_x;
			} else {
				step_x = 1;
				side_dist_x = (map_x + 1.0 - pos_x) * delta_dist_x;
			}

			if (ray_dir_y < 0) {
				step_y = -1;
				side_dist_y = (pos_y - map_y) * delta_dist_y;
			} else {
				step_y = 1;
				side_dist_y = (map_y + 1.0 - pos_y) * delta_dist_y;
			}

			while (hit == 0) {
				if (side_dist_x < side_dist_y) {
					side_dist_x += delta_dist_x;
					map_x += step_x;
					side = 0;
				}
				else {
					side_dist_y += delta_dist_y;
					map_y += step_y;
					side = 1;
				}
				if (world_map[map_x][map_y] > 0) {
					hit = 1;
				}
			}

			if (side == 0) {
				perp_wall_dist = (side_dist_x - delta_dist_x);
			} else {
				perp_wall_dist = (side_dist_y - delta_dist_y);
			}

			int line_height = (int)(SCREEN_HEIGHT / perp_wall_dist);

			int draw_start = -line_height / 2 + SCREEN_HEIGHT / 2;

			if (draw_start < 0) {
				draw_start = 0;
			}

			int draw_end = line_height / 2 + SCREEN_HEIGHT / 2;

			if (draw_end >= SCREEN_HEIGHT) {
				draw_end = SCREEN_HEIGHT - 1;
			}
			
			SDL_Color color;
			switch (world_map[map_x][map_y]) {
				case 1:
					color = {255, 0, 0, 255};
					break;
				case 2:
					color = {0, 255, 0, 255};
					break;
				case 3:
					color = {0, 0, 255, 255};
					break;
				case 4:
					color = {255, 255, 255, 255};
					break;
				default:
					color = {255, 255, 0, 255};
					break;
			}
			
			if (side == 1) {
				color = side_color(color);
			}

			SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
			SDL_RenderDrawLine(renderer, x, draw_start, x, draw_end);
		}

		SDL_RenderPresent(renderer);

		frame_time = SDL_GetTicks() - frame_start;

		if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W]) {
			if (world_map[int(pos_x + dir_x * move_speed)][int(pos_y)] == false) pos_x += dir_x * move_speed;
			if (world_map[int(pos_x)][int(pos_y + dir_y * move_speed)] == false) pos_y += dir_y * move_speed;
		}
		if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S]) {
			if (world_map[int(pos_x - dir_x * move_speed)][int(pos_y)] == false) pos_x -= dir_x * move_speed;
			if (world_map[int(pos_x)][int(pos_y - dir_y * move_speed)] == false) pos_y -= dir_y * move_speed;
		}
		if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D]) {
			old_dir_x = dir_x;
			dir_x = dir_x * cos(-rot_speed) - dir_y * sin(-rot_speed);
			dir_y = old_dir_x * sin(-rot_speed) + dir_y * cos(-rot_speed);
			old_plane_x = plane_x;
			plane_x = plane_x * cos(-rot_speed) - plane_y * sin(-rot_speed);
			plane_y = old_plane_x * sin(-rot_speed) + plane_y * cos(-rot_speed);
		}
		if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A]) {
			old_dir_x = dir_x;
			dir_x = dir_x * cos(rot_speed) - dir_y * sin(rot_speed);
			dir_y = old_dir_x * sin(rot_speed) + dir_y * cos(rot_speed);
			old_plane_x = plane_x;
			plane_x = plane_x * cos(rot_speed) - plane_y * sin(rot_speed);
			plane_y = old_plane_x * sin(rot_speed) + plane_y * cos(rot_speed);
		}

		if (frame_time < SCREEN_TICKS_PER_FRAME) {
			SDL_Delay(SCREEN_TICKS_PER_FRAME - frame_time);
		}

	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
