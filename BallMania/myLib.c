#include "myLib.h"

void dma_transfer(void *dst, void *src, u32 cnt, u32 mode)
{
	DMA[3].src = src;
	DMA[3].dst = dst;
	DMA[3].cnt = cnt | mode;
}

// Waits for a vertical blank
void wait_for_vb()
{
	while(SCANLINECOUNTER > 160);
	while(SCANLINECOUNTER < 160);
}

void set_background0(unsigned short *palette, unsigned short *tiles, unsigned short *map, int palette_length, int tiles_length, int map_length)
{
	// Backgrounds
	dma_transfer(&CHARBLOCKBASE[0], tiles, tiles_length, DMA_ON);
	dma_transfer(PALETTE, palette, palette_length, DMA_ON);
	dma_transfer(&SCREENBLOCKBASE[27], map, map_length, DMA_ON);

	// Set the background register
	REG_BG0CNT = BG_8BPP | BG_REG_64x64 | CBB(0) | SBB(27);

	// Set the offsets to zero
	REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;
}

void set_background1(unsigned short *tiles, unsigned short *map, int tiles_length, int map_length)
{
	// Load everything to the memory
	dma_transfer(&CHARBLOCKBASE[1], tiles, tiles_length, DMA_ON);
	dma_transfer(&SCREENBLOCKBASE[29], map, map_length, DMA_ON);

	// Set the background register
	REG_BG1CNT = BG_8BPP | BG_REG_64x64 | CBB(1) | SBB(29);

	REG_BG1HOFS = 0;
	REG_BG1VOFS = 0;
}

void stop_channel(int channel)
{
	if (channel == 1)
	{
		// Reset the timer and the dma
		REG_TM0CNT = 0;
		DMA[1].cnt = 0;

		sound_handler.channel1_playing = FALSE;
		sound_handler.channel2->vb_counter = 0;
	}
	else if (channel == 2)
	{
		// Reset the timer and the dma
		REG_TM1CNT = 0;
		DMA[2].cnt = 0;

		sound_handler.channel2_playing = FALSE;
		sound_handler.channel2->vb_counter = 0;
	}
}

void start_channel(int channel)
{
	if (channel == 1)
	{
		// If the channel is playing, stop the channel
		if (sound_handler.channel1_playing)
		{
			stop_channel(CHANNEL1);
		}

		// Start the timer
		REG_TM0D   = sound_handler.channel1->timer_intervall;       
		REG_TM0CNT = TIMER_ON;

		// Start the DMA
		DMA[1].src = sound_handler.channel1->sound;
		DMA[1].dst = REG_FIFO_A;
		DMA[1].cnt = DMA_ON | START_ON_FIFO_EMPTY |
					 DMA_32 | DMA_REPEAT |
					 DMA_DESTINATION_FIXED;

		sound_handler.channel1_playing = TRUE;	
	}
	else
	{
		// If the channel is playing, stop the channel
		if (sound_handler.channel2_playing)
		{
			stop_channel(CHANNEL2);
		}

		// Start the timer
		REG_TM1D   = sound_handler.channel2->timer_intervall;       
		REG_TM1CNT = TIMER_ON;

		// Start the DMA
		DMA[2].src = sound_handler.channel2->sound;
		DMA[2].dst = REG_FIFO_B;
		DMA[2].cnt = DMA_ON | START_ON_FIFO_EMPTY |
					 DMA_32 | DMA_REPEAT |
					 DMA_DESTINATION_FIXED;
		
		sound_handler.channel2_playing = TRUE;	
	}
}

void enable_sound()
{
	// Enable sound (Master control)
    REG_SOUNDCNT_X = SND_ENABLED;

    // enable and reset Direct Sound channel A & B, at full volume,
	REG_SOUNDCNT_H = SND_OUTPUT_RATIO_100
		| DSA_OUTPUT_RATIO_100
		| DSA_OUTPUT_TO_BOTH
		| DSA_TIMER0
		| DSA_FIFO_RESET
		| DSB_OUTPUT_RATIO_100
		| DSB_OUTPUT_TO_BOTH
		| DSB_TIMER1
		| DSB_FIFO_RESET;

	// Don't touch channels 1-4
    REG_SOUNDCNT_L = 0;
}

void initialize_sound(int channel, SOUND *sound)
{
	// Stop the channel in case it is running
	stop_channel(channel);

	// Initialize the sound
	sound->vb_counter = 0;
	sound->timer_intervall = -((1 << 24) / sound->freqvency);
	sound->start = 0;
	sound->number_of_vb = VBLANKFREQ * sound->lenght / sound->freqvency;
	
	if (channel == 1)
	{
		sound_handler.channel1 = sound;	
	}
	else if (channel == 2)
	{
		sound_handler.channel2 = sound;
	}
}

void enable_interrupts()
{
	REG_IME = FALSE;							// Disable interrupts
	REG_INTERRUPT = (u32)interrupt_handler;		// Set the interrupt handler

	enable_vb_interrupt();						// Enable vertical blank interrupts

	REG_IME = TRUE;								// Enable interrupts
}

void enable_vb_interrupt()
{
	REG_DISPSTAT |= INT_VBLANK_ENABLE;
	REG_IE = REG_IE | INT_VB;
}


void interrupt_handler()
{
	// Disable interrupts
	REG_IME = FALSE;

	// Handle the interrupt
	switch (REG_IF)
	{
	case INT_VB: vb_interrupt(); break;
	}

	// Restore and enable interrupts
	REG_IF = REG_IF;
	REG_IME = TRUE;
}

void vb_interrupt()
{
	sound_interrupt_handler();

	last_buttons = buttons;
	buttons = ~(*rawButtons);
}

void sound_interrupt_handler()
{
	if (sound_handler.channel1_playing)
	{
		sound_handler.channel1->vb_counter++;	
		if(sound_handler.channel1->vb_counter == sound_handler.channel1->number_of_vb)
		{
			stop_channel(CHANNEL1);

			// Repeat
			if (sound_handler.channel1->repeat)
			{
				start_channel(CHANNEL1);
			}
		}
	}

	if (sound_handler.channel2_playing)
	{
		sound_handler.channel2->vb_counter++;	
		if(sound_handler.channel2->vb_counter == sound_handler.channel2->number_of_vb)
		{
			stop_channel(CHANNEL2);

			// Repeat
			if (sound_handler.channel2->repeat)
			{
				start_channel(CHANNEL2);
			}
		}
	}
}

void init_lib(int mode)
{
	REG_DISPCTL = mode;
	enable_interrupts();
	enable_sound(); 
	hide_sprites();
}

void initialize_sprites(unsigned short *palette, unsigned short *tiles, unsigned int palette_length, unsigned int tiles_length)
{
	dma_transfer(SPRITE_PALETTE, palette, palette_length, DMA_ON);	// Copy the palette
	dma_transfer(&CHARBLOCKBASE[4], tiles, tiles_length, DMA_ON);	// Copy the tiles
}

void hide_sprites()
{
	int i;
	for (i = 0; i < 128; i++)
	{
		OAM[i].attribute0 |= -10 | ATTR0_HIDE;
		OAM[i].attribute1 |= -10;
	}
}

SPRITE_HANDLER* initialize_sprite(int i, int xf, int yf, int horizontal_speed, int vertical_speed, int size, int colormode, int trans, int affine, int affine_index, float angle, float delta, float scale, int tile_index)
{
	sprite_handler[i].alive = TRUE;
	sprite_handler[i].x = xf;
	sprite_handler[i].y = yf;
	sprite_handler[i].horizontal_speed = horizontal_speed;
	sprite_handler[i].vertical_speed = vertical_speed;

	sprite_handler[i].size = size;
	sprite_handler[i].colormode = colormode;
	sprite_handler[i].trans = trans;

	sprite_handler[i].affine = affine;
	sprite_handler[i].affine_index = affine_index;
	
	sprite_handler[i].angle = angle;
	sprite_handler[i].delta = delta;
	sprite_handler[i].scale = scale;

	sprite_handler[i].tile_index = tile_index;

	return &sprite_handler[i];
}

int get_sprite_size(int size)
{
	int ret = 0;
	switch (size)
	{
	case 16: ret = ATTR1_SIZE16; break;
	case 32: ret = ATTR1_SIZE32; break;
	case 64: ret = ATTR1_SIZE64; break;
	}
	return ret;
}
