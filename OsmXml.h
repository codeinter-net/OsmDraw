
#define TEXT_SIZE 1000000L	// Valeur arbitraire à déterminer plus finement


typedef unsigned long ulong;

typedef struct
{
	long	LineCount;
	long	NodeCount;
	long	EndNodeCount;
	long	TagCount;
	long	EndTagCount;
	long	WayCount;
	long	EndWayCount;
	long	NdCount;
	long	EndNdCount;
	long	RelationCount;
	long	EndRelationCount;
	long	MemberCount;
	long	EndMemberCount;
} OSM_STAT_LIST;

typedef struct
{
	long	LatMin;
	long	LatMax;
	long	LonMin;
	long	LonMax;
} OSM_INFO;

typedef struct
{
	long	id;
	long	lat;
	long	lon;
	long	tag;
} OSM_NODE;

typedef struct
{
	ulong	key;
	ulong	val;
	long	tag;
} OSM_TAG;

typedef struct
{
	long	id;
	long	tag;
	long	nd;
} OSM_WAY;

typedef struct
{
	long	ref;
	long	nd;
} OSM_ND;

typedef struct
{
	OSM_NODE	*Node;
	long	NodeNum;
	OSM_TAG	*Tag;
	long	TagNum;
	OSM_WAY	*Way;
	long	WayNum;
	OSM_ND	*Nd;
	long	NdNum;
	char	*Text;
	ulong	TextLen;
} OSM_DATA_LIST;

typedef struct
{
	long LatMin;
	long LatMax;
	long LatScale;
	long LonMin;
	long LonMax;
	long LonScale;
} OSM_DRAW_PARAM;

// ----------------------------------------------------------------

extern char KeyList[][10];

typedef struct
{
	int	Tag;
	int	Idx;
} OSM_LIST_MAP;

/*
typedef struct
{
float	R;
float	G;
float	B;
} OSM_DRAW_RGB;

typedef struct
{
//	OSM_DRAW_RGB	rgb;
	float	R;
	float	G;
	float	B;
	float	width;
	float	dash;
	float offset; // Décalage du tracé par rapport à l'axe : négatif -> vers la gauche / positif -> vers la droite
} OSM_DRAW_LINE;

typedef struct
{
char*	Tag;
char*	Value;
OSM_DRAW_LINE	Line[];	// Plusieurs lignes à cumuler pour obtenir un tracé complet
} OSM_DRAW_OBJECT;
*/

// ----------------------------------------------------------------

typedef struct
{
	int	type;
	double	r;
	double	g;
	double	b;
} OSM_FILL_STYLE;

typedef struct
{
	int	type;
	double	width;
	double	r;
	double	g;
	double	b;
	int	dot;
} OSM_DRAW_STYLE;

typedef union
{
	OSM_FILL_STYLE	fill;
	OSM_DRAW_STYLE	draw;
} OSM_STYLE;
