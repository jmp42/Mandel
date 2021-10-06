#include <Gamebuino-Meta.h>


// cf https://www.math.univ-toulouse.fr/~cheritat/wiki-draw/index.php/Mandelbrot_set


int LoopCount = 0;

const Color bluePalette[16] = {

 // blue   
  (Color)0x0000,
  (Color)0x0002,
  (Color)0x0004,
  (Color)0x0006,
  (Color)0x0008,
  (Color)0x000A,
  (Color)0x000C,
  (Color)0x000E,
  (Color)0x0010,
  (Color)0x0002,
  (Color)0x0014,
  (Color)0x0016,
  (Color)0x0018,
  (Color)0x001A,
  (Color)0x001C,
//  (Color)0x001F
 (Color)0xFFFF
  
/*
 // orange not working !
(Color)0,
(Color)4160,
(Color)8320,
(Color)12480,
(Color)16640,
(Color)20800,
(Color)24960,
(Color)29120,
(Color)33280,
(Color)37440,
(Color)41600,
(Color)45760,
(Color)49920,
(Color)54080,
(Color)58240,
(Color)62400
*/



};
void setup() {
  // put your setup code here, to run once:
  gb.begin();

  #if ( DISPLAY_MODE  == DISPLAY_MODE_INDEX )
  gb.display.setPalette(bluePalette);
  #endif

}

#if ( DISPLAY_MODE  == DISPLAY_MODE_INDEX )
  #define SCREEN_WIDTH (80*2)
  #define SCREEN_HEIGHT (64*2)
#else
  #define SCREEN_WIDTH (80*1)
  #define SCREEN_HEIGHT (64*1)
#endif

#define MANDELBROT_LOOP 15

struct complex_t {
  float f32_real;
  float f32_imag;
} complex_t;

inline struct complex_t complex_make( float f32_real, float f32_imag )
{
  struct complex_t cmp;
  cmp.f32_real = f32_real;
  cmp.f32_imag = f32_imag;
  return cmp;
}

float complex_squared_modulus( struct complex_t cmp )
{
  return cmp.f32_real * cmp.f32_real + cmp.f32_imag*cmp.f32_imag;
}

inline struct complex_t complex_square( struct complex_t cmp )
{
  struct complex_t temp;
  temp.f32_real = cmp.f32_real*cmp.f32_real - cmp.f32_imag*cmp.f32_imag;
  temp.f32_imag = 2 * cmp.f32_real * cmp.f32_imag;
  return temp;
}

inline struct complex_t complex_sum( struct complex_t cmpa, struct complex_t cmpb )
{
  struct complex_t temp;
  temp.f32_real = cmpa.f32_real + cmpb.f32_real;
  temp.f32_imag = cmpa.f32_imag + cmpb.f32_imag;
  return temp;
}

uint16_t compute_color( uint16_t u16_count )
{
  uint16_t u16_color;
  if (u16_count&1) u16_color|= 0x8000; // red
  if (u16_count&2) u16_color|= 0x0400; // green
  if (u16_count&4) u16_color|= 0x0010; // blue
  if (u16_count&8) u16_color|= 0x4000; // 1/2 red
  if (u16_count&16) u16_color|= 0x0200; // 1/2 green
  if (u16_count&32) u16_color|= 0x0008; // 1/2 blue
  return u16_color;
}

float fscale = 0.1;
float foffset_x = 0.0; // center x
float foffset_y = 0.0; // center y

#if ( DISPLAY_MODE  == DISPLAY_MODE_INDEX )
uint8_t compute_mandelbrot( uint16_t u16_x, uint16_t u16_y )
#else
uint16_t compute_mandelbrot( uint16_t u16_x, uint16_t u16_y )
#endif
{
    struct complex_t c = complex_make( fscale*(u16_x-SCREEN_WIDTH/2) + foffset_x, fscale*(u16_y-SCREEN_HEIGHT/2) + foffset_y );
    struct complex_t z = complex_make( 0, 0 );
    for ( uint16_t u16_count = 0 ; u16_count < MANDELBROT_LOOP ; u16_count++ )
    {
        if ( complex_squared_modulus(z) > 4 )
        #if ( DISPLAY_MODE  == DISPLAY_MODE_INDEX )
          return (uint8_t)u16_count; // color index
        #else
            return compute_color(u16_count); //0x0000; // black
        #endif
        z = complex_square( z );
        z = complex_sum( z, c );
    }
    return 0x0000; // black
//    return 0xFFFF; // white
}


int button_press()
{

    if ( gb.buttons.pressed(BUTTON_DOWN) )  {  foffset_y += fscale*SCREEN_HEIGHT/4; return 1; }
    if ( gb.buttons.pressed(BUTTON_UP) )  {foffset_y -= fscale*SCREEN_HEIGHT/4; return 1; }
    if ( gb.buttons.pressed(BUTTON_LEFT) )  {foffset_x -= fscale*SCREEN_WIDTH/4; return 1; }
    if ( gb.buttons.pressed(BUTTON_RIGHT) )  {foffset_x += fscale*SCREEN_WIDTH/4; return 1; }
    if ( gb.buttons.pressed(BUTTON_A) )   {fscale *= 2.0; return 1; }
    if ( gb.buttons.pressed(BUTTON_B) )   {fscale /= 2.0; return 1; }
  return 0;
}

void loop() {
  while (!gb.update());

  if ( button_press() || (LoopCount++==0) )
  {
    gb.sound.playTick();    // Play a ticking sound
//    gb.display.clear();
  for ( uint16_t u16_y = 0 ; u16_y < SCREEN_HEIGHT ; u16_y++ )
  {
    for ( uint16_t u16_x = 0 ; u16_x < SCREEN_WIDTH ; u16_x++ )
    {
      Color c = (Color)compute_mandelbrot( u16_x, u16_y );
      gb.display.drawPixel( u16_x, u16_y, c );
    }
    gb.update();
  }
    gb.display.setCursor(0,0);
    gb.display.setColor(15);
    gb.display.print(fscale);gb.display.print("\n");
    gb.display.print(foffset_x);gb.display.print(",");
    gb.display.print(foffset_y);gb.display.print("\n");
    gb.display.print( gb.display.width() ); gb.display.print(","); gb.display.print( gb.display.height() );

  }
  
}

