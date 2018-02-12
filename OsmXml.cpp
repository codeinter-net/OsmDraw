// OsmXml.cpp - PBA - 26/02/2014

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "OsmXml.h"
#include "Prefs.h"
#include "Util.h"
#include "Parser.h"
#include "Draw.h"

//char	InputFile[]="D:\\dt\\Perso\\Cartes\\poitou-charentes-latest.osm";
char	InputFile[]="D:\\dt\\Perso\\Cartes\\Auge.osm";
char	DataFile[]="osm-data.obd";
char	OutputFile[]="osm.png";

// ----------------------------------------------------------------
// Code pour les mesures de perfs
// ----------------------------------------------------------------

// time_t	StartTime,EndTime;
//	time(&StartTime);
//	time(&EndTime);
//	printf("TIME = %ld\n",(long)difftime(EndTime,StartTime));

// ----------------------------------------------------------------
//	SortData : Trie les données à tracer
// ----------------------------------------------------------------

ulong _SortData(OSM_DATA_LIST *OsmDataList, OSM_LIST_MAP* OsmListMap, ulong Begin, ulong End)
{
ulong	Count=0;
ulong	Key;

	if(End>OsmDataList->WayNum) End=OsmDataList->WayNum;
	for(ulong i=Begin; i<End; i++)
	{
		if(!(i%10000)) printf("SORT %lu\n",i);

		Key=ScanKey("highway");
		char* TagValue=SearchTag(OsmDataList,OsmDataList->Way[i].tag,Key);
//		char* TagValue=SearchTag(OsmDataList,OsmDataList->Way[i].tag,KEY_HIGHWAY);
//		if(TagValue&&OsmDataList->Way[i].nd)
		if(TagValue)
		{
			if(!strcmp(TagValue,"motorway"))
			{
				OsmListMap->Tag=TAG_HIGHWAY_MOTORWAY;
				OsmListMap->Idx=i;
				OsmListMap++;
				Count++;
			}
			else if(!strcmp(TagValue,"trunk"))
			{
				OsmListMap->Tag=TAG_HIGHWAY_TRUNK;
				OsmListMap->Idx=i;
				OsmListMap++;
				Count++;
			}
			else if(!strcmp(TagValue,"primary"))
			{
				OsmListMap->Tag=TAG_HIGHWAY_PRIMARY;
				OsmListMap->Idx=i;
				OsmListMap++;
				Count++;
			}
			else if(!strcmp(TagValue,"secondary"))
			{
				OsmListMap->Tag=TAG_HIGHWAY_SECONDARY;
				OsmListMap->Idx=i;
				OsmListMap++;
				Count++;
			}
			else if(!strcmp(TagValue,"tertiary"))
			{
				OsmListMap->Tag=TAG_HIGHWAY_TERTIARY;
				OsmListMap->Idx=i;
				OsmListMap++;
				Count++;
			}
			else if(!strcmp(TagValue,"residential"))
			{
				OsmListMap->Tag=TAG_HIGHWAY_RESIDENTIAL;
				OsmListMap->Idx=i;
				OsmListMap++;
				Count++;
			}
			else if(!strcmp(TagValue,"unclassified"))
			{
				OsmListMap->Tag=TAG_HIGHWAY_UNCLASSIFIED;
				OsmListMap->Idx=i;
				OsmListMap++;
				Count++;
			}
			else if(!strcmp(TagValue,"service"))
			{
				OsmListMap->Tag=TAG_HIGHWAY_SERVICE;
				OsmListMap->Idx=i;
				OsmListMap++;
				Count++;
			}
			else if(!strcmp(TagValue,"track"))
			{
				OsmListMap->Tag=TAG_HIGHWAY_TRACK;
				OsmListMap->Idx=i;
				OsmListMap++;
				Count++;
			}
		}

		Key=ScanKey("landuse");
		TagValue=SearchTag(OsmDataList,OsmDataList->Way[i].tag,Key);
//		TagValue=SearchTag(OsmDataList,OsmDataList->Way[i].tag,KEY_LANDUSE);
		if(TagValue&&OsmDataList->Way[i].nd)
		{
			if(!strcmp(TagValue,"residential"))
			{
				OsmListMap->Tag=TAG_LANDUSE_RESIDENTIAL;
				OsmListMap->Idx=i;
				OsmListMap++;
				Count++;
			}
			else if(!strcmp(TagValue,"forest"))
			{
				OsmListMap->Tag=TAG_LANDUSE_FOREST;
				OsmListMap->Idx=i;
				OsmListMap++;
				Count++;
			}
		}
	}
	printf("LIST COUNT = %lu\n",Count);
	return(Count);
}

ulong SortData(OSM_DATA_LIST *OsmDataList, OSM_LIST_MAP* OsmListMap, ulong Begin, ulong End)
{
ulong	Count=0;

	if(End>OsmDataList->WayNum) End=OsmDataList->WayNum;
	for(ulong i=Begin; i<End; i++)
	{
		ulong TagIndex=OsmDataList->Way[i].tag;
		if(TagIndex==-1) continue;
		
		ulong KeyIndex=OsmDataList->Tag[TagIndex].key;
		char* TagValue=SearchTag(OsmDataList,OsmDataList->Way[i].tag,KeyIndex);
if(!strcmp(TagValue,"forest"))
{
	printf("*");
}
		
		for(ulong j=0; j<10000L; j++)
		{
			if(!KeyList2[j].Name) break;
			if((KeyList2[j].Id==KeyIndex)&&!strcmp(TagValue,KeyList2[j].Value))
			{

// Problème : on ne trouve que le premier éléments du correspondant au nom -> Changer les listes

				OsmListMap->Tag=KeyList2[j].Id;
				OsmListMap->Idx=i;
				OsmListMap++;
				Count++;
				break;
			}
			else if((KeyList2[j].Id==KeyIndex))
			{
//				printf(".");
			}
		}
	}
	printf("LIST COUNT = %lu\n",Count);
	return(Count);
}

// ----------------------------------------------------------------
// Chargement / Sauvegarde / Allocation des données binaires
// ----------------------------------------------------------------

int AllocData(OSM_STAT_LIST *StatList, OSM_DATA_LIST* OsmDataList)
{
	OsmDataList->Node=(OSM_NODE*)malloc(sizeof(OSM_NODE)*StatList->NodeCount);
	if(!OsmDataList->Node) return(1);
	OsmDataList->Tag=(OSM_TAG*)malloc(sizeof(OSM_TAG)*StatList->TagCount);
	if(!OsmDataList->Tag) return(2);
	OsmDataList->Way=(OSM_WAY*)malloc(sizeof(OSM_WAY)*StatList->WayCount);
	if(!OsmDataList->Way) return(3);
	OsmDataList->Nd=(OSM_ND*)malloc(sizeof(OSM_ND)*StatList->NdCount);
	if(!OsmDataList->Nd) return(4);
	OsmDataList->Text=(char*)malloc(TEXT_SIZE);	// TODO : taille réelle
	if(!OsmDataList->Text) return(5);
	return(0);
}

int SaveData(char* DataFile, OSM_STAT_LIST *StatList, OSM_DATA_LIST* OsmDataList)
{
FILE*	Fh;

	Fh=fopen(DataFile,"wb");
	if(!Fh) return(1);
	fwrite(StatList,sizeof(OSM_STAT_LIST),1,Fh);
	fwrite(OsmDataList,sizeof(OSM_DATA_LIST),1,Fh);
	fwrite(OsmDataList->Node,sizeof(OSM_NODE),StatList->NodeCount,Fh);
	fwrite(OsmDataList->Tag,sizeof(OSM_TAG),StatList->TagCount,Fh);
	fwrite(OsmDataList->Way,sizeof(OSM_WAY),StatList->WayCount,Fh);
	fwrite(OsmDataList->Nd,sizeof(OSM_ND),StatList->NdCount,Fh);
	fwrite(OsmDataList->Text,1,TEXT_SIZE,Fh);	// TODO : taille réelle
	fclose(Fh);
	return(0);
}

int LoadData(char* DataFile, OSM_STAT_LIST *StatList, OSM_DATA_LIST* OsmDataList)
{
int	Ret;
FILE*	Fh;

	Fh=fopen(DataFile,"rb");
	if(!Fh) return(1);
	fread(StatList,sizeof(OSM_STAT_LIST),1,Fh);
	fread(OsmDataList,sizeof(OSM_DATA_LIST),1,Fh);
	Ret=AllocData(StatList,OsmDataList);
	if(Ret) return(Ret);
	fread(OsmDataList->Node,sizeof(OSM_NODE),StatList->NodeCount,Fh);
	fread(OsmDataList->Tag,sizeof(OSM_TAG),StatList->TagCount,Fh);
	fread(OsmDataList->Way,sizeof(OSM_WAY),StatList->WayCount,Fh);
	fread(OsmDataList->Nd,sizeof(OSM_ND),StatList->NdCount,Fh);
	fread(OsmDataList->Text,1,TEXT_SIZE,Fh);	// TODO : taille réelle
	fclose(Fh);
	return(0);
}


int ParseFile(char* InputFile, OSM_STAT_LIST *StatList, OSM_DATA_LIST* OsmDataList, OSM_INFO* OsmInfo)
{
int	Ret;

	ParseXml(InputFile,1,StatList,OsmDataList,OsmInfo);	// Première passe : Comptage des objets avant l'allocation mémoire
	memset(OsmDataList,0,sizeof(OSM_DATA_LIST));
	Ret=AllocData(StatList,OsmDataList);
	if(Ret) return(Ret);
	ParseXml(InputFile,2,StatList,OsmDataList,OsmInfo);	// Deuxième passe : Lecture et stockage du contenu utile
	return(0);
}

// ----------------------------------------------------------------

OSM_LIST_MAP	OsmListMap[100000];

int main(int argc, char* argv[])
{
int	Ret;
OSM_DATA_LIST OsmDataList;
OSM_STAT_LIST StatList;
OSM_INFO OsmInfo;
OSM_DRAW_PARAM OsmDrawParam;

	PrefsInit();

	OsmInfo.LatMin=0x7FFFFFFF;
	OsmInfo.LatMax=0x80000000;
	OsmInfo.LonMin=0x7FFFFFFF;
	OsmInfo.LonMax=0x80000000;

//	OsmDrawParam.LatMin=461589973;
//	OsmDrawParam.LatMax=467974590;
//	OsmDrawParam.LonMin=-7787872;
//	OsmDrawParam.LonMax=-2091946;

	OsmDrawParam.LatMin=463512000;
	OsmDrawParam.LatMax=465350000;	// 1838000
	OsmDrawParam.LonMin=-4120000;
	OsmDrawParam.LonMax=-2000000;	// 2120000
//	OsmDrawParam.LatScale=690;		// cos(latMoy)
//	OsmDrawParam.LonScale=1000;

	OsmDrawParam.LatScale=690/2;		// cos(latMoy)
	OsmDrawParam.LonScale=1000/2;

//	Lecture du fichier XML, conversion en OBD (Osm Binary Data) et stockage en fichier
//	Ret=ParseFile(InputFile,&StatList,&OsmDataList,&OsmInfo);
//	if(Ret) return(Ret);
//	SaveData(DataFile,&StatList,&OsmDataList);

//	Lecture directe du fichier OBD préalablement créé
	LoadData(DataFile,&StatList,&OsmDataList);

//	printf("LAT MIN = %ld\n",OsmInfo.LatMin);
//	printf("LAT MAX = %ld\n",OsmInfo.LatMax);
//	printf("LON MIN = %ld\n",OsmInfo.LonMin);
//	printf("LON MAX = %ld\n",OsmInfo.LonMax);

//	DisplayNode(&OsmDataList,0,200);
//	DisplayWay(&OsmDataList,11,12);

	ulong	ListCount=SortData(&OsmDataList,OsmListMap,0,100000);
	DrawMap(&OsmDataList,OsmListMap,ListCount,&OsmDrawParam,OutputFile);

	free(OsmDataList.Node);
	return(0);
}
