
#include <stdio.h>
#include <string.h>
#include "cairo.h"

#include "OsmXml.h"
#include "Prefs.h"
#include "Util.h"

double DotTest[]={14,6};

// ----------------------------------------------------------------
//	Sous-fonctions de DrawMap
// ----------------------------------------------------------------

void DrawNd(OSM_DATA_LIST *OsmDataList, ulong Index,cairo_t *cr,OSM_DRAW_PARAM *OsmDrawParam)
{
	int	x,y;
	bool First=true;
	for(;;)
	{
		if((Index<0)||(Index>=OsmDataList->NdNum)) break;
		int NodeIndex=GetNodeIndex(OsmDataList,OsmDataList->Nd[Index].ref);
		y=(OsmDrawParam->LatMax-OsmDataList->Node[NodeIndex].lat)/OsmDrawParam->LatScale;
		x=(OsmDataList->Node[NodeIndex].lon-OsmDrawParam->LonMin)/OsmDrawParam->LonScale;

//printf("%d %lu %d %d %ld %ld\n",First,Index,x,y,OsmDataList->Node[NodeIndex].lon,OsmDataList->Node[NodeIndex].lat);
		if(First)
			cairo_move_to(cr,x,y);
		else
			cairo_line_to(cr,x,y);

		First=false;
		Index=OsmDataList->Nd[Index].nd;
	}
}

void DrawWay(OSM_DATA_LIST *OsmDataList, OSM_LIST_MAP* OsmListMap, long TagValue, ulong ListCount, cairo_t *cr, OSM_DRAW_PARAM *OsmDrawParam)
{
	for(ulong i=0; i<ListCount; i++,OsmListMap++)
	{
		if(!(i%100)) printf("DRAW %lu\n",i);
//printf("%ld : %ld\n",i,OsmListMap->Tag);

		if(OsmListMap->Tag==TagValue)
		{
//printf("%ld\n",OsmDataList->Way[OsmListMap->Idx].nd);
			DrawNd(OsmDataList,OsmDataList->Way[OsmListMap->Idx].nd,cr,OsmDrawParam);
			cairo_stroke(cr);
		}
	}
}

void FillWay(OSM_DATA_LIST *OsmDataList, OSM_LIST_MAP* OsmListMap, long TagValue, ulong ListCount, cairo_t *cr, OSM_DRAW_PARAM *OsmDrawParam)
{
	for(ulong i=0; i<ListCount; i++,OsmListMap++)
	{
		if(!(i%100)) printf("FILL %lu\n",i);
//printf("%ld : %ld\n",i,OsmListMap->Tag);

		if(OsmListMap->Tag==TagValue)
		{
//printf("%ld\n",OsmDataList->Way[OsmListMap->Idx].nd);
			DrawNd(OsmDataList,OsmDataList->Way[OsmListMap->Idx].nd,cr,OsmDrawParam);
			cairo_fill(cr);
		}
	}
}

// ----------------------------------------------------------------
// DrawMap : Dessine la carte
// ----------------------------------------------------------------

void DrawMap(OSM_DATA_LIST* OsmDataList, OSM_LIST_MAP* OsmListMap, ulong ListCount, OSM_DRAW_PARAM *OsmDrawParam, char* FileName)
{
	long ImageWidth=(OsmDrawParam->LonMax-OsmDrawParam->LonMin)/OsmDrawParam->LonScale;
	long ImageHeight=(OsmDrawParam->LatMax-OsmDrawParam->LatMin)/OsmDrawParam->LatScale;

	cairo_surface_t *surface=cairo_image_surface_create(CAIRO_FORMAT_ARGB32, ImageWidth, ImageHeight);
	cairo_t *cr=cairo_create(surface);
	cairo_set_source_rgb(cr,1,1,1);
	cairo_paint(cr);
	cairo_set_line_cap(cr,CAIRO_LINE_CAP_ROUND);

	for(int i=0;i<10000;i++)	// Tracé des surfaces
	{
		if(!FillStyle[i].type) break;
		printf("FILL STYLE %d : %d\n",i,FillStyle[i].type);
		cairo_set_source_rgb(cr,FillStyle[i].r,FillStyle[i].g,FillStyle[i].b);
		FillWay(OsmDataList,OsmListMap,FillStyle[i].type,ListCount,cr,OsmDrawParam);
	}

	for(int i=0;i<10000;i++)	// Tracé des lignes
	{
		if(!DrawStyle[i].type) break;
		printf("DRAW STYLE %d : %d\n",i,DrawStyle[i].type);
		cairo_set_line_width(cr,DrawStyle[i].width);
		cairo_set_source_rgb(cr,DrawStyle[i].r,DrawStyle[i].g,DrawStyle[i].b);
		if(DrawStyle[i].dot)
			cairo_set_dash (cr, DotTest, 2, 0);
		else
			cairo_set_dash (cr, 0, 0, 0);
		DrawWay(OsmDataList,OsmListMap,DrawStyle[i].type,ListCount,cr,OsmDrawParam);
	}

	cairo_destroy(cr);
	cairo_surface_write_to_png(surface, FileName);
	cairo_surface_destroy(surface);
}

