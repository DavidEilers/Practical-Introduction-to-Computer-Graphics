#ifndef HELP_H
#define HELP_H


// *** Help-Module, PrakCG Template

#ifdef __EXPORT_HELP

const char *spalte1[] =
{
	"Maus",
	"",
	"linke Taste      Kamerabewegung",
	"mittlere Taste   Zoom",
	"rechte Taste     Kontextmenue",

	NULL
};
const char *spalte2[] =
{
	"Tastatur:",
	"",
	"f,F    - Framerate (An/Aus)",
	"l,L    - Licht global (An/Aus)",
	"h,H,F1 - Hilfe (An/Aus)",
	"w,W    - WireFrame (An/Aus)",
	"k,K    - Koordinatensystem (An/Aus)",
	"c,C    - Backfaceculling (An/Aus)",
	"m,M   - Lichtquelle aendern",
	"z,Z     - Kameraperspektive aendern",
	"",
	"Linke Pfeiltaste - Zug nach vorne bewegen"
	"",
	"ESC    - Beenden",

	NULL
};
#endif


#ifndef	M_PI				// Pi
#define M_PI 3.14159265358979323846
#endif
#ifndef SGN					// Vorzeichen bestimmen
#define SGN(y) (((y) < 0) ? -1 : ((y) > 0))
#endif
#ifndef MIN					// Minimum bestimmen
#define MIN(a,b) ((a) > (b))? (b) : (a)
#endif
#ifndef MAX					// Maximum bestimmen
#define MAX(a,b) ((a) > (b))? (a) : (b)
#endif


class cg_help
{
public:
	static void  toggle ();
	static void  toggleFps ();
	static void  setTitle ( char *t );
	static void  setWireframe ( bool wf );
	static bool  isWireframe ();
	static void  toggleKoordsystem ();
	static bool  isKoordsystem ();
	static float getFps ();
	static void  draw ();
	static void  drawKoordsystem ( GLfloat xmin, GLfloat xmax, GLfloat ymin, GLfloat ymax, GLfloat zmin, GLfloat zmax );
	static void  setFooter ( const char *text );

private:
	static bool  showhelp, showfps, wireframe, koordsystem;
	static int   frames;
	static float fps, bg_size, shadow;
	static const char  *title;
	static const char  *footer;
	static void drawBackground();
	static void printText ( float x, float y, const char *text, void *font = GLUT_BITMAP_HELVETICA_18 );
	static void printText ( float x, float y, const char *text, float r, float g, float b, void *font = GLUT_BITMAP_HELVETICA_18 );
	static void printTextShadow ( float x, float y, const char *text, float r, float g, float b, void *font = GLUT_BITMAP_HELVETICA_18 );
	static void printFps ( float x, float y, void *font = GLUT_BITMAP_HELVETICA_18 );
};


#endif	// HELP_H
