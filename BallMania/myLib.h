typedef unsigned char	u8;
typedef signed char		s8;
typedef unsigned short	u16;
typedef signed short	s16;
typedef unsigned int	u32;
typedef signed int		s32;

// Video defines
#define REG_DISPCTL			*(unsigned short *)0x4000000
#define SCANLINECOUNTER		(*(volatile u16*)0x4000006)
#define VIDEO_BUFFER		(*(unsigned short *)0x6000000)
#define BG2_ENABLE			(1<<10)

#define TRUE				0x1
#define FALSE				0x0
#define WORLD_MODIFIER		5
#define GRAVITY				6
#define SCREEN_WIDTH		240
#define SCREEN_HEIGHT		160
#define PALETTE				((unsigned short *)0x5000000)

// Useful macros
#define OFFSET(r,c,rowlen)			((r) * (rowlen) + (c))			// Calculates the offset
#define ABS(x)						(((x) < 0) ? -(x) : (x))		// Returns the absolute of the argument
#define SET_MODE(mode)				(REG_DISPCTL = (mode))			// Sets a mode
#define TO_FLOAT(i)					((i) << WORLD_MODIFIER)			// Converts an integer to a "float"
#define TO_INTEGER(f)				((f) >> WORLD_MODIFIER)			// Converts an float to a "integer"
#define DRAW_PIXEL3(x, y, color)	VIDEO_BUFFER[OFFSET(y, x, 240)] = color;

// Buttons
int buttons;
int last_buttons;

static volatile unsigned int* rawButtons = (volatile unsigned int *) 0x4000130;

#define BUTTON_HELD(mask)		((mask & buttons) == mask)
#define BUTTON_PRESSED(mask)	(((last_buttons & (mask)) != (mask)) && BUTTON_HELD(mask))

// Button Definitions
#define BUTTON_A       		0x1
#define BUTTON_B       		0x2
#define BUTTON_SELECT  		0x4
#define BUTTON_START   		0x8
#define BUTTON_RIGHT   		0x10
#define BUTTON_LEFT    		0x20
#define BUTTON_UP      		0x40
#define BUTTON_DOWN    		0x80
#define BUTTON_R       		0x100
#define BUTTON_L       		0x200

void delay(int clicks);
void wait_for_vb();

// DMA
typedef struct
{								
	 const volatile void *src;	
	 volatile void *dst;		
	 volatile u32 cnt;			
								
} DMAREC;

#define DMA							((volatile DMAREC *)0x040000B0)

#define DMA_DESTINATION_INCREMENT	(0 << 21)
#define DMA_DESTINATION_DECREMENT	(1 << 21)
#define DMA_DESTINATION_FIXED		(2 << 21)
#define DMA_DESTINATION_RESET		(3 << 21)

#define DMA_SOURCE_INCREMENT		(0 << 23)
#define DMA_SOURCE_DECREMENT		(1 << 23)
#define DMA_SOURCE_FIXED			(2 << 23)

#define DMA_REPEAT					(1 << 25)

#define DMA_16						(0 << 26)
#define DMA_32						(1 << 26)

#define DMA_NOW						(0 << 28)
#define DMA_AT_VBLANK				(1 << 28)
#define DMA_AT_HBLANK				(2 << 28)
#define DMA_AT_REFRESH				(3 << 28)

#define DMA_IRQ						(1 << 30)
#define DMA_ON						(1 << 31)

#define START_ON_FIFO_EMPTY			0x30000000

// Timers
#define REG_TM0CNT			*(volatile u16*)0x4000102
#define REG_TM1CNT			*(volatile u16*)0x4000106
#define REG_TM2CNT 			*(volatile u16*)0x400010A
#define REG_TM3CNT 			*(volatile u16*)0x400010E

#define REG_TM0D       		*(volatile u16*)0x4000100
#define REG_TM1D       		*(volatile u16*)0x4000104
#define REG_TM2D       		*(volatile u16*)0x4000108
#define REG_TM3D       		*(volatile u16*)0x400010C

// Timer flags
#define TIMER_ON      		(1<<7)  
#define TM_IRQ        		(1<<6)
#define TM_CASCADE    		(1<<2)
#define TM_FREQ_1     		0
#define TM_FREQ_64    		1
#define TM_FREQ_256   		2
#define TM_FREQ_1024  		3

// Time factors to multiply clock ticks to convert to microsec (usec)
// The next line (uncommented) should be in myLib.c
// double time_factors[] = {0.059604, 3.811, 15.259, 59.382};

// Freqs
#define VBLANKFREQ 59.727
extern double time_factors[];

// Sounds
#define REG_SOUNDCNT_X 			(*(u32*)0x04000084)
#define SND_ENABLED    			0x00000080

// Register definitions
#define REG_SOUNDCNT_L			(*(u16*)0x04000080)
#define REG_SOUNDCNT_H			(*(u16*)0x04000082)

// Flags
#define SND_ENABLED           	0x00000080
#define SND_OUTPUT_RATIO_25   	0x0000
#define SND_OUTPUT_RATIO_50   	0x0001
#define SND_OUTPUT_RATIO_100  	0x0002
#define DSA_OUTPUT_RATIO_50   	0x0000
#define DSA_OUTPUT_RATIO_100  	0x0004
#define DSA_OUTPUT_TO_RIGHT   	0x0100
#define DSA_OUTPUT_TO_LEFT    	0x0200
#define DSA_OUTPUT_TO_BOTH    	0x0300
#define DSA_TIMER0            	0x0000
#define DSA_TIMER1            	0x0400
#define DSA_FIFO_RESET        	0x0800
#define DSB_OUTPUT_RATIO_50   	0x0000
#define DSB_OUTPUT_RATIO_100  	0x0008
#define DSB_OUTPUT_TO_RIGHT   	0x1000
#define DSB_OUTPUT_TO_LEFT    	0x2000
#define DSB_OUTPUT_TO_BOTH    	0x3000
#define DSB_TIMER0            	0x0000
#define DSB_TIMER1            	0x4000
#define DSB_FIFO_RESET			0x8000

typedef struct 
{
	unsigned char *sound;
	int lenght;
	int freqvency;
	int repeat;

	int vb_counter;
	int timer_intervall;
	int start;
	int number_of_vb;
} SOUND;

typedef struct 
{
	int channel1_playing;
	int channel2_playing;

	SOUND *channel1;
	SOUND *channel2;
} SOUND_SYSTEM;

#define CHANNEL1 0x1
#define CHANNEL2 0x2

static SOUND_SYSTEM sound_handler;

// Interrutps
#define REG_IME			*(u16*)0x4000208
#define REG_IE			*(u16*)0x4000200
#define REG_IF			*(volatile u16*)0x4000202
#define REG_INTERRUPT	*(u32*)0x3007FFC

// Display Status Control Register 
#define REG_DISPSTAT *(u16*)0x4000004

// Interrupt constants for turning them on
#define INT_VBLANK_ENABLE (1<<3)
#define INT_HBLANK_ENABLE (1<<4)
#define INT_VCOUNT_ENABLE (1<<5)

#define INT_VB     (1<< 0)	// VB – vertical blank interrupt
#define INT_HB     (1<< 1)	// HB – horizontal blank interrupt
#define INT_VC     (1<< 2)	// VC – vertical scanline count interrupt
#define INT_T0     (1<< 3)	// T0 – timer 0 interrupt
#define INT_T1     (1<< 4)	// T1 – timer 1 interrupt
#define INT_T2     (1<< 5)	// T2 – timer 2 interrupt
#define INT_T3     (1<< 6)	// T3 – timer 3 interrupt
#define INT_COM    (1<< 7)	// COM – serial communication interrupt
#define INT_DMA0   (1<< 8)	// DMA0 – DMA0 finished interrupt
#define INT_DMA1   (1<< 9)	// DMA1 – DMA1 finished interrupt
#define INT_DMA2   (1<<10) 	// DMA2 – DMA2 finished interrupt
#define INT_DMA3   (1<<11)	// DMA3 – DMA3 finished interrupt
#define INT_BUTTON (1<<12) 	// BUTTON – button interrupt
#define INT_CART   (1<<13)	// CART – game cartridge 

// FIFO address defines
#define REG_FIFO_A				((void *)0x040000A0)
#define REG_FIFO_B				((void *)0x040000A4)

// Modes
#define MODE0 0
#define MODE3 3
#define MODE4 4

// Backgrounds
#define BG0_ENABLE (1<<8)
#define BG1_ENABLE (1<<9)
#define BG2_ENABLE (1<<10)
#define BG3_ENABLE (1<<11)

//background control registers
#define REG_BG0CNT *(volatile unsigned short*)0x4000008
#define REG_BG1CNT *(volatile unsigned short*)0x400000A
#define REG_BG2CNT *(volatile unsigned short*)0x400000C
#define REG_BG3CNT *(volatile unsigned short*)0x400000E

// *** Tiles =========================================================

typedef struct { u16 tileimg[8192]; } charblock;
typedef struct { u16 tilemap[1024]; } screenblock;

#define CHARBLOCKBASE ((charblock *)0x6000000)
#define SCREENBLOCKBASE ((screenblock *)0x6000000)

#define CharBaseBlock(n)		(((n) * 0x4000) + 0x6000000)
#define ScreenBaseBlock(n)		(((n) * 0x800) + 0x6000000)


//background offset registers
#define REG_BG0HOFS *(volatile unsigned short *)0x04000010
#define REG_BG1HOFS *(volatile unsigned short *)0x04000014
#define REG_BG2HOFS *(volatile unsigned short *)0x04000018
#define REG_BG3HOFS *(volatile unsigned short *)0x0400001C

#define REG_BG0VOFS *(volatile unsigned short *)0x04000012
#define REG_BG1VOFS *(volatile unsigned short *)0x04000016
#define REG_BG2VOFS *(volatile unsigned short *)0x0400001A
#define REG_BG3VOFS *(volatile unsigned short *)0x0400001E

//macros and bit constants for setting the background control register specifics
#define SBB(num)			num << 8
#define CBB(num)			num << 2
#define COLOR256			1 << 7
#define BG_SIZE0			0<<14      // 32 x 32 tiles
#define BG_SIZE1			1<<14      // 64 x 32
#define BG_SIZE2			2<<14      // 32 x 64
#define BG_SIZE3			3<<14      // 64 x 64
#define BG_MOSAIC			(1 << 6)
#define WRAPAROUND			0x1

#define BG_4BPP (0 << 7)
#define BG_8BPP (1 << 7)
#define BG_REG_32x32 (0 << 14)
#define BG_REG_64x32 (1 << 14)
#define BG_REG_32x64 (2 << 14)
#define BG_REG_64x64 (3 << 14)

// Some useful macros
#define CharBaseBlock(n)	(((n) * 0x4000) + 0x6000000)
#define ScreenBaseBlock(n)	(((n) * 0x800) + 0x6000000)

#define SPRITE_MODE_1D (1 << 6)			//put this into REG_DISPCNTL to enable 1D mapping mode
#define SPRITE_ENABLE (1 << 12)			//put this into REG_DISPCNTL to enable sprites

#define ATTR0_REGULAR			(0 << 8)			// Normal Sprites
#define ATTR0_AFFINE			(1 << 8)			// Affine Sprites
#define ATTR0_HIDE				(2 << 8)			// Hide Sprites
#define ATTR0_DOUBLE_AFFINE		(3 << 8)			// Double Affine Sprites (Never used it before so don't ask me -Peter)
#define ATTR0_NORMAL			(0 << 10)			// Normal Rendering
#define ATTR0_BLEND				(1 << 10)			// Enables Alpha Blending.  Don't worry about it.  Unless you want too =D
#define ATTR0_WIN				(2 << 10)			// Object Window mode.  Again, no idea.
#define ATTR0_MOSAIC			(1 << 12)			// Enables the mosaic effect for this sprite.  It's a cool visual effect.  We can talk about it on thursday perhaps
#define ATTR0_4BPP				(0 << 13)			// 16 color Sprites
#define ATTR0_8BPP				(1 << 13)			// 256 color Sprites
#define ATTR0_SQUARE			(0 << 14)			// Square Shape
#define ATTR0_WIDE				(1 << 14)			// Wide Shape
#define ATTR0_TALL				(2 << 14)			// Tall Shape

#define ATTR1_HFLIP				(1 << 12)
#define ATTR1_VFLIP				(1 << 13)
#define ATTR1_SIZE8				(0 << 14)
#define ATTR1_SIZE16			(1 << 14)
#define ATTR1_SIZE32			(2 << 14)
#define ATTR1_SIZE64			(3 << 14)
#define ATTR1_AFFINEINDEX(n)	(n<<9)

#define MASK					(0x1FF)

#define OAM						((SPRITE*)(0x7000000))
#define SPRITE_MEM				((unsigned short*)0x7000000)
#define SPRITE_PALETTE			((unsigned short*)(0x5000200))
#define REG_MOSAIC				*(volatile unsigned short*)0x400004C

#define SPRITEOFFSET16(x,y)		(y) * 16 + (x)
#define ROTDATA(n)				((n) << 9)

typedef struct _SPRITE
{
	u16 attribute0;
	u16 attribute1;
	u16 attribute2;
	u16 attribute3;
} SPRITE;

typedef struct _SPRITE_HANDLER
{
	int alive;
	int x;
	int y;
	int horizontal_speed;
	int vertical_speed;

	int size;
	int colormode;
	int trans;

	int affine;
	int affine_index;
	
	float angle;
	float delta;
	float scale;

	int tile_index;
} SPRITE_HANDLER;

static SPRITE_HANDLER sprite_handler[128];

// Common
void init_lib(int mode);
void dma_transfer(void *dst, void *src, u32 cnt, u32 mode);
void calculate_angles();

// Sound
void stop_channel(int channel);
void start_channel(int channel);
void enable_sound();
void initialize_sound(int channel, SOUND *sound);
void sound_interrupt_handler();

// Interrupts
void enable_interrupts();		// Enables interrupts
void enable_vb_interrupt();		// Enables vb interrupt
void interrupt_handler();		// The interrupt handler used
void vb_interrupt();			// Calls the sound handler

// Sprites
void initialize_sprites(unsigned short *palette, unsigned short *tiles, unsigned int palette_length, unsigned int tiles_length);
void hide_sprites();
SPRITE_HANDLER* initialize_sprite(int i, int xf, int yf, int horizontal_speed, int vertical_speed, int size, int colormode, int trans, int affine, int affine_index, float angle, float delta, float scale, int tile_index);
void update_sprite_pos(int num, int x, int y);
void update_sprites();
int get_sprite_size(int size);

