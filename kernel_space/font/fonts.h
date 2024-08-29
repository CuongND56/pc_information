
#ifndef __FONTS_H
#define __FONTS_H

#include <linux/types.h>

/** @defgroup FONTS_Exported_Types
  * @{
  */ 
typedef struct _tFont
{    
  const uint8_t *table;
  uint16_t Width;
  uint16_t Height;
  
} sFONT;

extern sFONT Font24;
extern sFONT Font20;
extern sFONT Font16;
extern sFONT Font12;
extern sFONT Font8;
/**
  * @}
  */ 

/** @defgroup FONTS_Exported_Constants
  * @{
  */ 

typedef struct { // Data stored PER GLYPH
	uint16_t bitmapOffset;     // Pointer into GFXfont->bitmap
	uint8_t  width, height;    // Bitmap dimensions in pixels
	uint8_t  xAdvance;         // Distance to advance cursor (x axis)
	int8_t   xOffset, yOffset; // Dist from cursor position to UL corner
} GFXglyph;

typedef struct { // Data stored for FONT AS A WHOLE:
	uint8_t  *bitmap;      // Glyph bitmaps, concatenated
	GFXglyph *glyph;       // Glyph array
	uint8_t   first, last; // ASCII extents
	uint8_t   yAdvance;    // Newline distance (y axis)
} GFXfont;

/**
  * @brief  Draw Properties structures definition
  */
typedef struct
{
	uint32_t 	TextColor;
	uint32_t 	BackColor;
	sFONT*    	pFont;
	uint8_t		TextWrap;
} lcdFontPropTypeDef;

  
#endif /* __FONTS_H */
 

