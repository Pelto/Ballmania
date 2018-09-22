
//{{BLOCK(splashscreen)

//======================================================================
//
//	splashscreen, 256x256@8, 
//	+ palette 256 entries, not compressed
//	+ 228 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 14592 + 2048 = 17152
//
//	Time-stamp: 2009-11-30, 22:48:23
//	Exported by Cearn's GBA Image Transmogrifier
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef __SPLASHSCREEN__
#define __SPLASHSCREEN__

#define splashscreenTilesLen 14592
extern const unsigned short splashscreenTiles[7296];

#define splashscreenMapLen 2048
extern const unsigned short splashscreenMap[1024];

#define splashscreenPalLen 512
extern const unsigned short splashscreenPal[256];

#endif // __SPLASHSCREEN__

//}}BLOCK(splashscreen)
