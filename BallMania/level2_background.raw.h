
//{{BLOCK(level2_background)

//======================================================================
//
//	level2_background, 512x1024@8, 
//	+ palette 256 entries, not compressed
//	+ 12 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 64x128 
//	Total size: 512 + 768 + 16384 = 17664
//
//	Time-stamp: 2009-11-24, 12:49:00
//	Exported by Cearn's GBA Image Transmogrifier
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef __LEVEL2_BACKGROUND__
#define __LEVEL2_BACKGROUND__

#define level2_backgroundTilesLen 768
extern const unsigned short level2_backgroundTiles[384];

#define level2_backgroundMapLen 16384
extern const unsigned short level2_backgroundMap[8192];

#define level2_backgroundPalLen 512
extern const unsigned short level2_backgroundPal[256];

#endif // __LEVEL2_BACKGROUND__

//}}BLOCK(level2_background)
