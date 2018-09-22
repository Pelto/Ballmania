// Includes
#include "myLib.h"
#include "main.h"
#include "affine.h"
#include "fixed.h"

// Sounds
#include "bounce_sound.raw.h"
#include "game_music.raw.h"

// Sprites and backgronds
#include "spritemap.raw.h"
#include "level1_background.raw.h"
#include "level2_background.raw.h"
#include "level1_collisionmap.raw.h"
#include "level2_collisionmap.raw.h"
#include "splashscreen.raw.h"
#include "victoryscreen.raw.h"

// The games current state
int current_state;

// The vertical and horizontal offset of 
// the camera in fake floats
int voff0 = TO_FLOAT(512 - 160);
int voff1 = TO_FLOAT(512 - 160);
int hoff0 = 0;
int hoff1 = 0;

// Cheat swithcers
int cheat_nogravity = FALSE;
int cheat_nocol	= FALSE;

int can_vbounce;	// If the ball has bounced upp or down
int can_hbounce;	// If the ball has bounced left or right

// The width and height of the current map
int game_width = 512;
int game_height = 512;

// Collision maps
unsigned short *collision_map;						// The collision map being used
int collision_map_size;								// The width of the collision map

int all_taken = FALSE;

LEVEL level1;
LEVEL level2;
LEVEL *current_level;

SOUND background_music;
SOUND bouncing_sound;

int mosaic_level = 0;
int mosaic_counter = 0;

int main()
{
	level1.tiles = level1_backgroundTiles;
	level1.map = level1_backgroundMap;
	level1.palatte = level1_backgroundPal;
	level1.tiles_length = level1_backgroundTilesLen >> 1;
	level1.collision_map = level1_collisionmapBitmap;
	level1.collision_map_size = 64;
	
	// Level 2
	level2.tiles = level2_backgroundTiles;
	level2.map = level2_backgroundMap;
	level2.palatte = level2_backgroundPal;
	level2.tiles_length = level2_backgroundTilesLen >> 1;
	level2.collision_map = level2_collisionmapBitmap;
	level2.collision_map_size = 64;
	
	background_music.freqvency = GAME_MUSIC_FREQ;
	background_music.lenght = GAME_MUSIC_LENGTH;
	background_music.sound = game_music;
	background_music.repeat = TRUE;

	bouncing_sound.freqvency = BOUNCE_FREQ;
	bouncing_sound.lenght = BOUNCE_LEN;
	bouncing_sound.sound = bounce_sound;
	bouncing_sound.repeat = FALSE;

	// Initializes
	init_lib(MODE0 | BG0_ENABLE | BG1_ENABLE | SPRITE_ENABLE);
	
	enter_state(STATE_SPLASH);

	// Game loop
	while (TRUE)
	{	
		switch (current_state)
		{
		case STATE_LEVEL1: game_state(); break;
		case STATE_LEVEL2: game_state(); break;
		case STATE_GAMEOVER: gameover_state(); break;
		case STATE_SPLASH: splashscreen_state(); break;
		case STATE_GAMEWON: gamewon_state(); break;
		}
	}
}


void rotate_sprite(int sprite, float delta)
{
	sprite_handler[sprite].angle += delta;

	// Make sure that the value is always between 0 and 360
	if (sprite_handler[sprite].angle > 359.0)
	{
		sprite_handler[sprite].angle -= 359.0;
	}
	if (sprite_handler[sprite].angle < 0.0)
	{
		sprite_handler[sprite].angle += 359.0;
	}
}

int sprite_collide(SPRITE_HANDLER *sprite1, SPRITE_HANDLER *sprite2)
{
	// If any of the following condititions are fulfilled it is impossible
	// for the sprites to be in collision.
	if (TO_INTEGER(sprite1->x) + sprite1->size < TO_INTEGER(sprite2->x) || 
		TO_INTEGER(sprite1->y) + sprite1->size < TO_INTEGER(sprite2->y) || 
		TO_INTEGER(sprite1->x) > TO_INTEGER(sprite2->x) + sprite2->size || 
		TO_INTEGER(sprite1->y) > TO_INTEGER(sprite2->y) + sprite2->size)
	{
		return FALSE;
	}

	return TRUE;
}
int valid_move(int x, int y)
{
	// Check the screen bounds
	if ((x < 0 || x > game_width) || (y < 0 || y > game_width))
	{
		return FALSE;
	}

	// If we don't have a collision map, return true
	if (collision_map_size == 0 || cheat_nocol)
	{
		return TRUE;
	}

	// Check against the collision map
	if (collision_map[OFFSET(y / 8, x / 8, collision_map_size)] == 0)
	{
		return FALSE;
	}

	// No collision, return true
	return TRUE;
}



void handle_ball(SPRITE_HANDLER *ball)
{
	// Check if the ball can move in any of the following 
	// directions based on the maps bounds and the
	// current collision map.
	int down	=	valid_move(TO_INTEGER(ball->x), TO_INTEGER(ball->y) + 1 + 32)
				&&	valid_move(TO_INTEGER(ball->x) + 8, TO_INTEGER(ball->y) + 1 + 32)
				&&	valid_move(TO_INTEGER(ball->x) + 16, TO_INTEGER(ball->y) + 1 + 32)
				&&	valid_move(TO_INTEGER(ball->x) + 24, TO_INTEGER(ball->y) + 1 + 32);
	int up		=	valid_move(TO_INTEGER(ball->x), TO_INTEGER(ball->y) - 1)
				&&	valid_move(TO_INTEGER(ball->x) + 8, TO_INTEGER(ball->y) - 1)
				&&	valid_move(TO_INTEGER(ball->x) + 16, TO_INTEGER(ball->y) - 1)
				&&	valid_move(TO_INTEGER(ball->x) + 24, TO_INTEGER(ball->y) - 1);
	int right	=	valid_move(TO_INTEGER(ball->x) + 1 + 32, TO_INTEGER(ball->y) + 0)
				&&	valid_move(TO_INTEGER(ball->x) + 1 + 32, TO_INTEGER(ball->y) + 8)
				&&	valid_move(TO_INTEGER(ball->x) + 1 + 32, TO_INTEGER(ball->y) + 16)
				&&	valid_move(TO_INTEGER(ball->x) + 1 + 32, TO_INTEGER(ball->y) + 24);
	int left	=	valid_move(TO_INTEGER(ball->x) - 1, TO_INTEGER(ball->y) + 0)
				&&	valid_move(TO_INTEGER(ball->x) - 1, TO_INTEGER(ball->y) + 8)
				&&	valid_move(TO_INTEGER(ball->x) - 1, TO_INTEGER(ball->y) + 16)
				&&	valid_move(TO_INTEGER(ball->x) - 1, TO_INTEGER(ball->y) + 24);

	// Check where the ball is going to end up as well
	// This will prevent some glitches when the ball is 
	// going to fast.
	down	&= valid_move(TO_INTEGER(ball->x) + 0, TO_INTEGER(ball->y + ball->vertical_speed) + 32)
			&& valid_move(TO_INTEGER(ball->x) + 8, TO_INTEGER(ball->y + ball->vertical_speed) + 32)
			&& valid_move(TO_INTEGER(ball->x) + 16, TO_INTEGER(ball->y + ball->vertical_speed) + 32)
			&& valid_move(TO_INTEGER(ball->x) + 24, TO_INTEGER(ball->y + ball->vertical_speed) + 32);
	up		&= valid_move(TO_INTEGER(ball->x) + 0, TO_INTEGER(ball->y + ball->vertical_speed))
			&& valid_move(TO_INTEGER(ball->x) + 8, TO_INTEGER(ball->y + ball->vertical_speed))
			&& valid_move(TO_INTEGER(ball->x) + 16, TO_INTEGER(ball->y + ball->vertical_speed))
			&& valid_move(TO_INTEGER(ball->x) + 24, TO_INTEGER(ball->y + ball->vertical_speed));
	right	&= valid_move(TO_INTEGER(ball->x + ball->horizontal_speed) + 32, TO_INTEGER(ball->y) + 0)
			&& valid_move(TO_INTEGER(ball->x + ball->horizontal_speed) + 32, TO_INTEGER(ball->y) + 8)
			&& valid_move(TO_INTEGER(ball->x + ball->horizontal_speed) + 32, TO_INTEGER(ball->y) + 16)
			&& valid_move(TO_INTEGER(ball->x + ball->horizontal_speed) + 32, TO_INTEGER(ball->y) + 24);
	left	&= valid_move(TO_INTEGER(ball->x + ball->horizontal_speed), TO_INTEGER(ball->y) + 0)
			&& valid_move(TO_INTEGER(ball->x + ball->horizontal_speed), TO_INTEGER(ball->y) + 8)
			&& valid_move(TO_INTEGER(ball->x + ball->horizontal_speed), TO_INTEGER(ball->y) + 16)
			&& valid_move(TO_INTEGER(ball->x + ball->horizontal_speed), TO_INTEGER(ball->y) + 24);

	// Apply the gravity unless we cheating
	if (!cheat_nogravity)
	{
		ball->vertical_speed += GRAVITY;
	}

	// If the ball is done falling, stop the vertical speed
	if (!down && ball->vertical_speed == GRAVITY)		
	{
		ball->vertical_speed = 0;
	}
	// If the ball has hit the ground
	else if (!down && ball->vertical_speed > GRAVITY)
	{
		// Decrease it's speed
		ball->vertical_speed -= BOUNCE_FRICTION;

		// If it was the last bump, stop the ball, otherwise 
		// invert the speed so that the ball bouncces
		if (ball->vertical_speed < 0)
		{
			ball->vertical_speed = 0;
		}
		else
		{
			start_channel(CHANNEL2);
			ball->vertical_speed *= -1;
		}

		// Enable jumping
		can_vbounce = TRUE;
	}

	// If the ball is heading right
	if (ball->horizontal_speed > 0)
	{
		// If the ball is rolling, only decrease the speed a little until we come to a stop
		// Otherwise is the ball has hit something decrease and invert the speed
		if (!down && right)
		{
			ball->horizontal_speed -= 1;
		}
		else if (!right && !(!down && ball->vertical_speed != 0))
		{
			// Decrease the speed a little
			ball->horizontal_speed -= ROLL_FRICTION;

			// If the friction reversed the ball, stop the ball
			if (ball->horizontal_speed < 0)
			{
				ball->horizontal_speed = 0;
			}

			// Invert the speed
			ball->horizontal_speed *= -1;
			start_channel(CHANNEL2);

			// Enable pushing
			can_hbounce = TRUE;
		}
	}
	// If the ball is going left
	else if (ball->horizontal_speed < 0)
	{
		// If the ball is rolling, only decrease the speed a little until we come to a stop
		// Otherwise is the ball has hit something decrease and invert the speed
		if (!down && left)
		{
			ball->horizontal_speed += 1;
		}
		else if (!left && !(!down && ball->vertical_speed != 0))
		{
			// Decrease the speed a little
			ball->horizontal_speed += ROLL_FRICTION;

			// If the friction reversed the ball, stop the ball
			if (ball->horizontal_speed > 0)
			{
				ball->horizontal_speed = 0;
			}

			// Invert the speed
			ball->horizontal_speed *= -1;
			start_channel(CHANNEL2);

			// Enable pushing
			can_hbounce = TRUE;
		}
	}
	// If we aren't moving left or right, make sure we can push the ball
	else if (!can_hbounce)
	{
		can_hbounce = TRUE;
	}

	// If the ball as hit somehting on the way up
	// just reverse the vertical speed
	if (!up)
	{
		ball->vertical_speed *= -1;
		start_channel(CHANNEL2);

		// Enable jumping
		can_vbounce = TRUE;
	}

	// Update the speed
	ball->x += ball->horizontal_speed;
	ball->y += ball->vertical_speed;

	// Make sure that we never go out of bounds
	if (TO_INTEGER(ball->x) < 0)
	{
		ball->x = 0;
	}
	else if (TO_INTEGER(ball->x) + ball->size > game_width)
	{
		ball->x = TO_FLOAT(game_width - ball->size);
	}
	if (TO_INTEGER(ball->y) < 0)
	{
		ball->y = 0;
	}
	else if (TO_INTEGER(ball->y) + ball->size > game_height)
	{
		ball->y = TO_FLOAT(game_height - ball->size);
	}

	int i = 0;

	// Check all the coins for collisions
	for (i; i < 2; i++)
	{
		if (sprite_collide(ball, current_level->coins[i].sprite))
		{
			current_level->coins[i].sprite->alive = FALSE;
		}
	}

	// If we have taken all the coins, make the goal reachable.
	if (!current_level->coins[0].sprite->alive && !current_level->coins[1].sprite->alive)
	{
		all_taken = TRUE;
	}

	// If all of the coins are taken and we are in collision with the flower, start the mosaic effects.
	if (all_taken && sprite_collide(ball, &sprite_handler[3]))
	{
		enter_state(STATE_GAMEOVER);
	}

	handle_camera(ball);
}

void handle_camera(SPRITE_HANDLER *ball)
{	
	// The balls locations on the screen
	int ballsx = GETX(TO_INTEGER(ball->x));
	int ballsy = GETY(TO_INTEGER(ball->y));

	// If the ball is outside the limit on the left side
	// match the cameras horizontal speed with the balls speed
	if (ballsx < CAMERA_LIMIT && hoff0 > 0)
	{
		hoff0 -= ABS(ball->horizontal_speed);

		// Make sure that we can't move the ball outside the map
		if (hoff0 < 0)
		{
			hoff0 = 0;
		}
		else
		{
			hoff1 -= ABS(ball->horizontal_speed) * 0.3;
		}
	}
	// If the ball is outside the limit on the right side
	// match the cameras horizontal speed with the balls speed
	else if (ballsx + 32 > SCREEN_WIDTH - 32 && TO_INTEGER(hoff0) < game_width - SCREEN_WIDTH)
	{
		hoff0 += ABS(ball->horizontal_speed);

		// Make sure that we can't move the ball outside the map
		if (TO_INTEGER(hoff0) > game_width - SCREEN_WIDTH)
		{
			hoff0 = TO_FLOAT(game_width - SCREEN_WIDTH);
		}
		else
		{
			hoff1 += ABS(ball->horizontal_speed) * 0.3;
		}
	}
	// If the ball is outside the limit on the upper side
	// match the cameras vertical speed with the balls speed
	if (ballsy < CAMERA_LIMIT && voff0 > 0)
	{
		voff0 -= ABS(ball->vertical_speed);

		// Make sure that we can't move the ball outside the map
		if (voff0 < 0)
		{
			voff0 = 0;
		}
		else
		{
			voff1 -= ABS(ball->vertical_speed) * 0.3;
		}
	}
	// If the ball is outside the limit on the right side
	// match the cameras vertical speed with the balls speed
	else if (ballsy + 32 > SCREEN_HEIGHT - 32 && TO_INTEGER(hoff0) < game_height - SCREEN_HEIGHT)
	{
		voff0 += ABS(ball->vertical_speed);

		// Make sure that we can't move the ball outside the map
		if (TO_INTEGER(voff0) > game_height - SCREEN_HEIGHT)
		{
			voff0 = TO_FLOAT(game_height - SCREEN_HEIGHT);
		}
		else
		{
			voff1 += ABS(ball->vertical_speed) * 0.3;
		}
	}
}

void load_level_background(u16 *palatte, u16 *tiles, u16 *map, int tiles_length)
{
	int sb_size = 1024;
	dma_transfer(PALETTE, palatte, 256, DMA_ON);
	dma_transfer(&CHARBLOCKBASE[0], tiles, tiles_length, DMA_ON);
	dma_transfer(&SCREENBLOCKBASE[27], map, 4 * sb_size , DMA_ON);
	dma_transfer(&SCREENBLOCKBASE[23], map + 4 * sb_size, 4 * sb_size, DMA_ON);

	// Set the background register
	REG_BG0CNT = BG_8BPP | BG_REG_64x64 | CBB(0) | SBB(23) | BG_MOSAIC;
	REG_BG1CNT = BG_8BPP | BG_REG_64x64 | CBB(0) | SBB(27) | WRAPAROUND | BG_MOSAIC;
}

void enter_state(int state)
{
	switch (state)
	{
	case STATE_SPLASH: enter_splashscreen(); break;
	case STATE_LEVEL1: enter_gamestate(&level1); break;
	case STATE_LEVEL2: enter_gamestate(&level2); break;
	case STATE_GAMEOVER: enter_gameover(); break;
	case STATE_GAMEWON: enter_gamewon(); break;
	}

	current_state = state;
}

void enter_gamewon()
{
	hide_sprites();

	dma_transfer(PALETTE, victoryscreenPal, 256, DMA_ON);
	dma_transfer(&CHARBLOCKBASE[0], victoryscreenTiles, victoryscreenTilesLen >> 1, DMA_ON);
	dma_transfer(&SCREENBLOCKBASE[27], victoryscreenMap, victoryscreenMapLen >> 1, DMA_ON);

	REG_BG0CNT = BG_8BPP | BG_REG_32x32 | CBB(0) | SBB(27);
	REG_BG0HOFS = 0;
	REG_BG0VOFS = 0;
}

void enter_splashscreen()
{
	// Hide all of the sprites
	hide_sprites();

	// Set the background
	dma_transfer(PALETTE, splashscreenPal, 256, DMA_ON);
	dma_transfer(&CHARBLOCKBASE[0], splashscreenTiles, splashscreenTilesLen >> 1, DMA_ON);
	dma_transfer(&SCREENBLOCKBASE[27], splashscreenMap, splashscreenMapLen >> 1, DMA_ON);

	// Set the background registers
	REG_BG0CNT = BG_8BPP | BG_REG_32x32 | CBB(0) | SBB(27);
	REG_BG0HOFS = 0;
	REG_BG0VOFS = 0;
}

void splashscreen_state()
{
	if (BUTTON_PRESSED(BUTTON_START))
	{
		enter_state(STATE_LEVEL1);
	}
}

void gamewon_state()
{
	if (BUTTON_PRESSED(BUTTON_START))
	{
		enter_state(STATE_SPLASH);
	}
}

void enter_gameover()
{
	mosaic_level = 0;
	mosaic_counter = 0;
}

void enter_gamestate(LEVEL *level)
{
	// Load the sounds
	stop_channel(CHANNEL1);
	stop_channel(CHANNEL1);

	initialize_sound(CHANNEL1, &background_music);
	initialize_sound(CHANNEL2, &bouncing_sound);

	// Intialize the sprites and set the background
	initialize_sprites(spritemap_rawPal, spritemap_rawTiles, spritemap_rawPalLen >> 1, spritemap_rawTilesLen >> 1);

	sprite_handler[0].alive = 1;
	sprite_handler[0].x = TO_FLOAT((240 >> 1) - 16);
	sprite_handler[0].y = TO_FLOAT(512 - 40);
	sprite_handler[0].angle = 0;
	sprite_handler[0].size = 32;
	sprite_handler[0].scale = 1;
	sprite_handler[0].delta = 5.0;
	sprite_handler[0].horizontal_speed = 0;
	sprite_handler[0].vertical_speed = 0;

	if (level == &level1)
	{
		level1.coins[0].current_image = COIN_IMAGE1;
		level1.coins[1].current_image = COIN_IMAGE1;

		level1.coins[0].sprite = &sprite_handler[1];
		level1.coins[1].sprite = &sprite_handler[2];

		level1.coins[0].sprite->alive = TRUE;
		level1.coins[0].sprite->colormode = ATTR0_8BPP;
		level1.coins[0].sprite->size = 16;
		level1.coins[0].sprite->trans = ATTR0_SQUARE;

		level1.coins[0].sprite->x = TO_FLOAT(400);
		level1.coins[0].sprite->y = TO_FLOAT(32);
		level1.coins[0].sprite->tile_index = COIN_IMAGE1;
		level1.coins[0].image_counter = 0;

		level1.coins[1].sprite->alive = TRUE;
		level1.coins[1].sprite->colormode = ATTR0_8BPP;
		level1.coins[1].sprite->size = 16;
		level1.coins[1].sprite->trans = ATTR0_SQUARE;

		level1.coins[1].sprite->x = TO_FLOAT(55);
		level1.coins[1].sprite->y = TO_FLOAT(25);
		level1.coins[1].sprite->tile_index = COIN_IMAGE1;
		level1.coins[1].image_counter = 0;

		level1.goal.current_image = GOAL_IMAGE;
		level1.goal.sprite = &sprite_handler[3];
			
		level1.goal.sprite->alive = TRUE;
		level1.goal.sprite->colormode = ATTR0_8BPP;
		level1.goal.sprite->size = 16;
		level1.goal.sprite->trans = ATTR0_SQUARE;

		level1.goal.sprite->x = TO_FLOAT(40);
		level1.goal.sprite->y = TO_FLOAT(512 - 32);
		level1.goal.sprite->angle = 0;
		level1.goal.sprite->tile_index = GOAL_IMAGE;
		level1.goal.image_counter = 0;
	}
	else
	{
		level2.coins[0].current_image = COIN_IMAGE1;
		level2.coins[1].current_image = COIN_IMAGE1;

		level2.coins[0].sprite = &sprite_handler[1];
		level2.coins[1].sprite = &sprite_handler[2];

		level2.coins[0].sprite->alive = TRUE;
		level2.coins[0].sprite->colormode = ATTR0_8BPP;
		level2.coins[0].sprite->size = 16;
		level2.coins[0].sprite->trans = ATTR0_SQUARE;

		level2.coins[0].sprite->x = TO_FLOAT(400);
		level2.coins[0].sprite->y = TO_FLOAT(32);
		level2.coins[0].sprite->tile_index = COIN_IMAGE1;
		level2.coins[0].image_counter = 0;

		level2.coins[1].sprite->alive = TRUE;
		level2.coins[1].sprite->colormode = ATTR0_8BPP;
		level2.coins[1].sprite->size = 16;
		level2.coins[1].sprite->trans = ATTR0_SQUARE;

		level2.coins[1].sprite->x = TO_FLOAT(55);
		level2.coins[1].sprite->y = TO_FLOAT(25);
		level2.coins[1].sprite->tile_index = COIN_IMAGE1;
		level2.coins[1].image_counter = 0;

		level2.goal.current_image = GOAL_IMAGE;
		level2.goal.sprite = &sprite_handler[3];
			
		level2.goal.sprite->alive = TRUE;
		level2.goal.sprite->colormode = ATTR0_8BPP;
		level2.goal.sprite->size = 16;
		level2.goal.sprite->trans = ATTR0_SQUARE;

		level2.goal.sprite->x = TO_FLOAT(40);
		level2.goal.sprite->y = TO_FLOAT(512 - 32);
		level2.goal.sprite->angle = 0;
		level2.goal.sprite->tile_index = GOAL_IMAGE;
		level2.goal.image_counter = 0;
	}

	all_taken = FALSE;

	// Start the sound
	start_channel(CHANNEL1);

	// Load level background
	load_level_background(level->palatte, level->tiles, level->map, level->tiles_length);

	// Update the cameras
	REG_BG0HOFS = TO_INTEGER(hoff0);
	REG_BG0VOFS = TO_INTEGER(voff0);
	REG_BG1HOFS = TO_INTEGER(hoff1);
	REG_BG1VOFS = TO_INTEGER(voff1);

	// The collision map
	collision_map = level->collision_map;
	collision_map_size = level->collision_map_size;
	current_level = level;

	// Enable push and jump
	can_vbounce = TRUE;
	can_hbounce = TRUE;

	all_taken = FALSE;
	level->coins[0].sprite->alive = TRUE;
	level->coins[1].sprite->alive = TRUE;

	// Hide all the sprites
	hide_sprites();
}


void gameover_state()
{
	REG_MOSAIC = (mosaic_level << 12) | (mosaic_level << 8) | (mosaic_level << 4) | mosaic_level;
	
	mosaic_counter++;
	if (mosaic_counter == 150 && mosaic_level < 120)
	{
		mosaic_level++;
		mosaic_counter = 0;
	}
	if (mosaic_level == 120)
	{
		if (current_level == &level1)
		{
			enter_state(STATE_LEVEL2);
		}
		else
		{
			enter_state(STATE_GAMEWON);
		}
	}
}

void game_buttons()
{
	if(BUTTON_PRESSED(BUTTON_LEFT) && (can_hbounce ||sprite_handler[0].horizontal_speed > 0))
	{
		sprite_handler[0].horizontal_speed -= PUSH_STRENGTH_SIDES;
		can_hbounce = FALSE;
	}
	if(BUTTON_PRESSED(BUTTON_RIGHT) && (can_hbounce || sprite_handler[0].horizontal_speed < 0))
	{
		sprite_handler[0].horizontal_speed += PUSH_STRENGTH_SIDES;
		can_hbounce = FALSE;
	}
	if(BUTTON_PRESSED(BUTTON_UP) && can_vbounce)
	{
		sprite_handler[0].vertical_speed -= PUSH_STRENGTH_UP;
		can_vbounce = FALSE;
	}
	if(BUTTON_PRESSED(BUTTON_DOWN) && (can_vbounce || sprite_handler[0].vertical_speed != 0))
	{
		sprite_handler[0].vertical_speed += PUSH_STRENGTH_UP;
		can_vbounce = FALSE;
	}
	if (BUTTON_PRESSED(BUTTON_L))
	{
		cheat_nocol = (cheat_nocol) ? FALSE : TRUE;
	}
	if (BUTTON_PRESSED(BUTTON_R))
	{
		cheat_nogravity = (cheat_nogravity) ? FALSE : TRUE;
	}
}

void game_state()
{
	if (mosaic_level > 0)
	{
		mosaic_level = 0;
		REG_MOSAIC = (mosaic_level << 12) | (mosaic_level << 8) | (mosaic_level << 4) | mosaic_level;
	}

	// Get the x and y position for the ball on the screen.
	int y = GETY(TO_INTEGER(sprite_handler[0].y));
	int x = GETX(TO_INTEGER(sprite_handler[0].x));

	OAM[0].attribute0 = (MASK & y) | ATTR0_8BPP | ATTR0_SQUARE | ATTR0_AFFINE | ATTR0_MOSAIC;
	OAM[0].attribute1 = (MASK & x) | ATTR1_SIZE32 | ATTR1_AFFINEINDEX(0);
	OAM[0].attribute2 = 0;

	// Get the x and y position on the screen for the goal
	x = GETX(TO_INTEGER(sprite_handler[3].x));
	y = GETY(TO_INTEGER(sprite_handler[3].y));

	// Only update the oam if the goal is visible on the screen
	if (x + 32 >= 0 && x <= SCREEN_WIDTH && y + 32 >= 0 && y <= SCREEN_HEIGHT)
	{
		OAM[3].attribute0 = (MASK & y) | ATTR0_8BPP | ATTR0_SQUARE | ATTR0_AFFINE | ATTR0_MOSAIC;
		OAM[3].attribute1 = (MASK & x) | ATTR1_SIZE32 | ATTR1_AFFINEINDEX(3);
		OAM[3].attribute2 = GOAL_IMAGE;
		setAffineMatrix(3, sprite_handler[3].angle, INT2FIX(1), INT2FIX(1));
	}
	else
	{
		OAM[3].attribute0 = ATTR0_HIDE;
	}

	// Rotate the ball
	setAffineMatrix(0, sprite_handler[0].angle, INT2FIX(1), INT2FIX(1));

	// Update the sprites that contains the coins
	transfer_coins();

	// Handle the buttons
	game_buttons();

	// Update the balls position and speeds
	handle_ball(&sprite_handler[0]);

	if (sprite_handler[0].horizontal_speed > 0 || sprite_handler[0].horizontal_speed < 0)
	{
		rotate_sprite(0, -(sprite_handler[0].horizontal_speed / 10));
	}

	if (all_taken)
	{
		rotate_sprite(3, 5);
	}

	// Wait for a vertical blank before updating
	wait_for_vb();

	// Update the cameras
	REG_BG0HOFS = TO_INTEGER(hoff0);
	REG_BG0VOFS = TO_INTEGER(voff0);
	REG_BG1HOFS = TO_INTEGER(hoff1);
	REG_BG1VOFS = TO_INTEGER(voff1);
}


void transfer_coins()
{
	int i;
	int coin;
	for (i = 1; i < 3; i++)
	{
		 coin = i - 1;

		// if the coin is taken hide it and continue with the next coin
		if (!current_level->coins[coin].sprite->alive)
		{
			OAM[i].attribute0 = ATTR0_HIDE;
			continue;
		}

		// The x and y position of the coin on the screen
		int y = GETY(TO_INTEGER(current_level->coins[coin].sprite->y));
		int x = GETX(TO_INTEGER(current_level->coins[coin].sprite->x));

		// Only transfer the coint to the sprite memory if it is on the screen
		if (x + 16 >= 0 && x <= SCREEN_WIDTH && y + 16 >= 0 && y <= SCREEN_HEIGHT)
		{
			// Set the memory
			OAM[i].attribute0 = (MASK & y) | ATTR0_8BPP | ATTR0_SQUARE;
			OAM[i].attribute1 = (MASK & x) | ATTR1_SIZE16;
			OAM[i].attribute2 = current_level->coins[coin].current_image;

			// Update the counter
			current_level->coins[coin].image_counter += 1;

			// If the counter have reached it's limit, reset the counter
			if (current_level->coins[coin].image_counter == COUNTER_LIMIT)
			{
				// Switch to the next image
				switch (current_level->coins[coin].current_image)
				{
				case COIN_IMAGE1: current_level->coins[coin].current_image = COIN_IMAGE2; break;
				case COIN_IMAGE2: current_level->coins[coin].current_image = COIN_IMAGE3; break;
				case COIN_IMAGE3: current_level->coins[coin].current_image = COIN_IMAGE4; break;
				case COIN_IMAGE4: current_level->coins[coin].current_image = COIN_IMAGE1; break;
				}

				// Reset the counter.
				current_level->coins[coin].image_counter = 0;
			}
		}
		else
		{
			OAM[i].attribute0 = ATTR0_HIDE;
		}
	}
}
