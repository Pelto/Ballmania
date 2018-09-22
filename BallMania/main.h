// -------------------------------------------------
//	DEFINES
// -------------------------------------------------

// State definitions
#define STATE_SPLASH			0x1		// Splash screen
#define STATE_LEVEL1			0x2		// Level 1
#define STATE_LEVEL2			0x3		// Level 2
#define STATE_GAMEWON			0x4		// High score
#define STATE_GAMEOVER			0x5		// game over

#define NR_OF_COINS				0x2
#define COIN_IMAGE1				0x80
#define COIN_IMAGE2				0xc0
#define COIN_IMAGE3				0x100
#define COIN_IMAGE4				0xc80

#define GOAL_IMAGE				0x180
#define MOSAIC_COUNTER_LIMIT	5
#define MOSAIC_LIMIT			200

// Physics
#define PUSH_STRENGTH_UP		200
#define PUSH_STRENGTH_SIDES		150

// Friction
#define ROLL_FRICTION			10		// Decrease the speed with this value when the ball rolls
#define BOUNCE_FRICTION			50		// Decrease the speed with this value when the ball bounces

#define CAMERA_LIMIT			60

// Macros that will translate a big coordinate to the screens
#define GETX(bigx)	(bigx) - TO_INTEGER(hoff0)
#define GETY(bigy)	(bigy) - TO_INTEGER(voff0)

#define COUNTER_LIMIT 7

typedef struct _ANIMATED
{
	SPRITE_HANDLER *sprite;
	int current_image;
	int image_counter;
} ANIMATED;

typedef struct _LEVEL
{
	const u16 *tiles;		// The tiles being used
	const u16 *map;			// The map being used.
	const u16 *palatte;		// The palatte being used

	// Since we will use 512 * 512 maps for each level
	// with two backgrounds we know how big the map will be
	// and since the palatte is constant we only need to know
	// how many tiles there are.
	int tiles_length;

	// Collision maps
	u16 *collision_map;		// The collision map being used
	int collision_map_size;	// The width of the collision map

	// The coins
	ANIMATED coins[NR_OF_COINS];
	ANIMATED goal;
} LEVEL;

// -------------------------------------------------
//	FUNCTIONS
// -------------------------------------------------
void transfer_coins();

void enter_state(int state);
void enter_splashscreen();
void enter_gamestate(LEVEL *level);
void enter_gamewon();
void enter_gameover();

void game_state();
void gameover_state();
void gamewon_state();
void splashscreen_state();

void rotate_sprite(int sprite, float delta);										// Rotates the sprite
void handle_ball(SPRITE_HANDLER *ball);												// Apply's physics to the sprite
void game_buttons();																// Process the input
void handle_camera(SPRITE_HANDLER *ball);											// Adjust the camera after a sprite													
void load_level_background(u16 *palatte, u16 *tiles, u16 *map, int tiles_length);	// Loads a leve

int valid_move(int x, int y);														// If the x and y coordinate is valid
int sprite_collide(SPRITE_HANDLER *sprite1, SPRITE_HANDLER *sprite2);				// If two sprites are coilliding with each other