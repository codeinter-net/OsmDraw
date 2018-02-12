
#include <string.h>
#include "OsmXml.h"
#include "Util.h"
#include "Prefs.h"

//char KeyList[][10]={"zero","name","ref","place","amenity","highway","landuse","barrier","leisure",""};

OSM_KEY_LIST	KeyList2[1000];

typedef struct
{
	int	Id;
	char*	Key1;
	char*	Val1;
	char*	Key2;
	char*	Val2;
} OSM_TAG_DEF;

OSM_TAG_DEF TagDef[]=
{
	{TAG_HIGHWAY_PATH,"highway","path",0,0},
/*
	{TAG_HIGHWAY_TRACKTYPE1,"highway","track","tracktype","grade1"},
	{TAG_HIGHWAY_TRACKTYPE2,"highway","track","tracktype","grade2"},
	{TAG_HIGHWAY_TRACKTYPE3,"highway","track","tracktype","grade3"},
	{TAG_HIGHWAY_TRACKTYPE4,"highway","track","tracktype","grade4"},
	{TAG_HIGHWAY_TRACKTYPE5,"highway","track","tracktype","grade5"},
*/
	{TAG_HIGHWAY_TRACK,"highway","track",0,0},
	{TAG_HIGHWAY_SERVICE,"highway","service",0,0},
	{TAG_HIGHWAY_RESIDENTIAL,"highway","residential",0,0},
	{TAG_HIGHWAY_UNCLASSIFIED,"highway","unclassified",0,0},
	{TAG_HIGHWAY_TERTIARY,"highway","tertiary",0,0},
	{TAG_HIGHWAY_SECONDARY,"highway","secondary",0,0},
	{TAG_HIGHWAY_PRIMARY,"highway","primary",0,0},
	{TAG_HIGHWAY_TRUNK,"highway","trunk",0,0},
	{TAG_HIGHWAY_MOTORWAY,"highway","motorway",0,0},
	{TAG_LANDUSE_RESIDENTIAL,"landuse","residential","",""},
	{TAG_LANDUSE_FOREST,"landuse","forest","",""},
};

OSM_FILL_STYLE FillStyle[]=
{
	{TAG_LANDUSE_RESIDENTIAL,0.8,0.8,0.8},
	{TAG_LANDUSE_FOREST,0.4,0.7,0.4},
	{0,0,0,0}
};

OSM_DRAW_STYLE DrawStyle[]=
{
	{TAG_HIGHWAY_TRACK,4,0.6,0.3,0,1},
	{TAG_HIGHWAY_SERVICE,4,0.3,0.3,0.3,0},
	{TAG_HIGHWAY_RESIDENTIAL,4.5,0.5,0,0,0},
	{TAG_HIGHWAY_UNCLASSIFIED,4.5,0.3,0.3,0.3,0},
	{TAG_HIGHWAY_TERTIARY,5.5,0,0,0,0},
	{TAG_HIGHWAY_SECONDARY,6.5,0,0,0,0},
	{TAG_HIGHWAY_PRIMARY,8,0,0,0,0},

	{TAG_HIGHWAY_TRACK,3,1,1,1,0},
	{TAG_HIGHWAY_SERVICE,3,1,1,1,0},
	{TAG_HIGHWAY_RESIDENTIAL,3.5,1,1,1,0},
	{TAG_HIGHWAY_UNCLASSIFIED,3.5,1,1,1,0},
	{TAG_HIGHWAY_TERTIARY,4,1,1,0,0},
	{TAG_HIGHWAY_SECONDARY,5,1,0.6,0,0},
	{TAG_HIGHWAY_PRIMARY,6.5,1,0,0,0},

	{TAG_HIGHWAY_TRUNK,10,1,0,0.3,0},
	{TAG_HIGHWAY_TRUNK,2.5,1,1,1,0},
	{TAG_HIGHWAY_MOTORWAY,10,0,0,1,0},
	{TAG_HIGHWAY_MOTORWAY,2.5,1,1,1,0},
	{0,0,0,0,0,0}
};

int PrefsInit(void)
{
	int KeyListIndex=0;
	for(int i=0; i<sizeof(TagDef)/sizeof(OSM_TAG_DEF); i++)
	{
		int j;
		int h=Hash(TagDef[i].Key1);
		for(j=0; j<KeyListIndex; j++)	// on recherche si l'identifiant est déjà stocké
		{
			if(KeyList2[j].Hash==h)
			{
				if(stricmp(TagDef[i].Key1,KeyList2[j].Name))
					return(1);	// Deux identifiants avec le même hash -> problème
			}
		}
		if(j==KeyListIndex)	// identifiant non trouvé
		{
			KeyList2[KeyListIndex].Id=TagDef[i].Id;
			KeyList2[KeyListIndex].Hash=h;
			KeyList2[KeyListIndex].Name=TagDef[i].Key1;
			KeyList2[KeyListIndex++].Value=TagDef[i].Val1;
		}

		// PRB : Si Id est commun à Key1 et Key2, il n'est pas possible de stocker Key2 en plus de Key1 !

	}

	return(0);
}
