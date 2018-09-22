
//{{BLOCK(level1_background)

//======================================================================
//
//	level1_background, 512x1024@8, 
//	+ palette 256 entries, not compressed
//	+ 56 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 64x128 
//	Total size: 512 + 3584 + 16384 = 20480
//
//	Time-stamp: 2009-11-15, 16:49:14
//	Exported by Cearn's GBA Image Transmogrifier
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef __LEVEL1_BACKGROUND__
#define __LEVEL1_BACKGROUND__

#define level1_backgroundTilesLen 3584
extern const unsigned short level1_backgroundTiles[1792];

#define level1_backgroundMapLen 16384
extern const unsigned short level1_backgroundMap[8192];

#define level1_backgroundPalLen 512
extern const unsigned short level1_backgroundPal[256];

#endif // __LEVEL1_BACKGROUND__

//}}BLOCK(level1_background)
