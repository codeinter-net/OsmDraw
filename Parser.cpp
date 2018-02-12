
#include <stdio.h>
#include <string.h>
#include "OsmXml.h"
#include "Util.h"
#include "Prefs.h"


// ----------------------------------------------------------------
// Affichage d'infos texte
// ----------------------------------------------------------------

void DisplayTag(OSM_DATA_LIST *OsmDataList, ulong Index)
{
	for(;;)
	{
		if((Index<0)||(Index>=OsmDataList->TagNum)) return;
		printf("  # %s = %s\n",GetKey(OsmDataList->Tag[Index].key),OsmDataList->Text+OsmDataList->Tag[Index].val);
		Index=OsmDataList->Tag[Index].tag;
	}
}

void DisplayNode(OSM_DATA_LIST *OsmDataList, ulong Begin, ulong End)
{
	if(End>OsmDataList->NodeNum) End=OsmDataList->NodeNum;
	for(ulong i=Begin; i<End; i++)
	{
		printf(". : %lu %ld %ld %ld\n",OsmDataList->Node[i].id,OsmDataList->Node[i].lat,OsmDataList->Node[i].lon,OsmDataList->Node[i].tag);
		if(OsmDataList->Node[i].tag) DisplayTag(OsmDataList,OsmDataList->Node[i].tag);
	}
}

void DisplayNd(OSM_DATA_LIST *OsmDataList, ulong Index)
{
	for(;;)
	{
		if((Index<0)||(Index>=OsmDataList->NdNum)) return;
		int NodeIndex=GetNodeIndex(OsmDataList,OsmDataList->Nd[Index].ref);

		printf("  * %d = %d %d\n",OsmDataList->Nd[Index].ref,OsmDataList->Node[NodeIndex].lat,OsmDataList->Node[NodeIndex].lon);
		Index=OsmDataList->Nd[Index].nd;
	}
}

void DisplayWay(OSM_DATA_LIST *OsmDataList, ulong Begin, ulong End)
{
	if(End>OsmDataList->WayNum) End=OsmDataList->WayNum;
	for(ulong i=Begin; i<End; i++)
	{
		printf("- : %lu\n",OsmDataList->Way[i].id);
		if(OsmDataList->Way[i].tag) DisplayTag(OsmDataList,OsmDataList->Way[i].tag);
		if(OsmDataList->Way[i].nd) DisplayNd(OsmDataList,OsmDataList->Way[i].nd);
	}
}

// ----------------------------------------------------------------
// Sous-fonctions de ParseXml
// ----------------------------------------------------------------

long* ScanNode(char *Data, OSM_DATA_LIST *OsmDataList, OSM_INFO *OsmInfo)
{
char	*Ptr,*Id,*Lat,*Lon;
ulong	ScanId;
double	ScanLat,ScanLon;
OSM_NODE	*ScanNode;

	Ptr=strstr(Data,"id=\"");
	if(!Ptr) return(0);
	Id=Ptr+4;
	Ptr=strchr(Id,'"');
	if(!Ptr) return(0);
	*Ptr++=0;

	Ptr=strstr(Ptr,"lat=\"");
	if(!Ptr) return(0);
	Lat=Ptr+5;
	Ptr=strchr(Lat,'"');
	if(!Ptr) return(0);
	*Ptr++=0;

	Ptr=strstr(Ptr,"lon=\"");
	if(!Ptr) return(0);
	Lon=Ptr+5;
	Ptr=strchr(Lon,'"');
	if(!Ptr) return(0);
	*Ptr++=0;

	sscanf(Id,"%lu",&ScanId);
	sscanf(Lat,"%lf",&ScanLat);
	sscanf(Lon,"%lf",&ScanLon);

	ScanNode=&OsmDataList->Node[OsmDataList->NodeNum];
	ScanNode->id=ScanId;
	ScanNode->lat=(long)(ScanLat*10000000);
	if(ScanNode->lat<OsmInfo->LatMin) OsmInfo->LatMin=ScanNode->lat;
	if(ScanNode->lat>OsmInfo->LatMax) OsmInfo->LatMax=ScanNode->lat;
	ScanNode->lon=(long)(ScanLon*10000000);
	if(ScanNode->lon<OsmInfo->LonMin) OsmInfo->LonMin=ScanNode->lon;
	if(ScanNode->lon>OsmInfo->LonMax) OsmInfo->LonMax=ScanNode->lon;
	ScanNode->tag=-1L;
	OsmDataList->NodeNum++;
	return(&ScanNode->tag);
}

long* ScanNd(char *Data, OSM_DATA_LIST *OsmDataList,long *RefNd)
{
char	*Ptr,*Ref;
ulong	ScanRef;
OSM_ND	*ScanNd;

	Ptr=strstr(Data,"ref=\"");
	if(!Ptr) return(RefNd);
	Ref=Ptr+5;
	Ptr=strchr(Ref,'"');
	if(!Ptr) return(RefNd);
	*Ptr++=0;

	sscanf(Ref,"%lu",&ScanRef);

	ScanNd=&OsmDataList->Nd[OsmDataList->NdNum];
	ScanNd->ref=ScanRef;

	ScanNd->nd=-1L;
	if(RefNd) *RefNd=OsmDataList->NdNum;
	OsmDataList->NdNum++;
	return(&ScanNd->nd);
}

long* ScanWay(char *Data, OSM_DATA_LIST *OsmDataList)
{
char	*Ptr,*Id;
ulong	ScanId;
OSM_WAY	*ScanWay;

	Ptr=strstr(Data,"id=\"");
	if(!Ptr) return(0);
	Id=Ptr+4;
	Ptr=strchr(Id,'"');
	if(!Ptr) return(0);
	*Ptr++=0;

	sscanf(Id,"%lu",&ScanId);

	ScanWay=&OsmDataList->Way[OsmDataList->WayNum];
	ScanWay->id=ScanId;
	ScanWay->tag=-1L;
	ScanWay->nd=-1L;
	OsmDataList->WayNum++;
	return(&ScanWay->tag);
}

long* ScanTag(char *Data, OSM_DATA_LIST *OsmDataList,long *Ref)
{
int	TextSize;
char	*Ptr,*Key,*Val;
OSM_TAG	*ScanTag;
ulong	KeyNum;

	Ptr=strstr(Data,"k=\"");
	if(!Ptr) return(Ref);
	Key=Ptr+3;
	Ptr=strchr(Key,'"');
	if(!Ptr) return(Ref);
	*Ptr++=0;

// TODO : Gérer les clés seules ainsi que les couples clés-valeur dans certains cas.

	KeyNum=ScanKey(Key);
	if(KeyNum==-1L) return(Ref);	// Clé non prise en charge

	Ptr=strstr(Ptr,"v=\"");
	if(!Ptr) return(Ref);
	Val=Ptr+3;
	Ptr=strchr(Val,'"');
	if(!Ptr) return(Ref);
	*Ptr++=0;

	ScanTag=&OsmDataList->Tag[OsmDataList->TagNum];
	ScanTag->key=KeyNum;

	TextSize=strlen(Val);
	if(OsmDataList->TextLen+TextSize+1>TEXT_SIZE) return(Ref);	// Dépassement de buffer
	strcpy(OsmDataList->Text+OsmDataList->TextLen,Val);
	ScanTag->val=OsmDataList->TextLen;
	OsmDataList->TextLen+=TextSize+1;

	ScanTag->tag=-1L;
	if(Ref) *Ref=OsmDataList->TagNum;
	OsmDataList->TagNum++;
	return(&ScanTag->tag);
}

// ----------------------------------------------------------------
// ParseXml : Lit les données en XML et stocke ce qui est utile en binaire
// FileName		Fichier à lire
// Mode			0 = Compte le nombre de lignes du fichier ; 1 = Compte les élements présents ; 2 = Lit le contenu des éléments
// ----------------------------------------------------------------

void ParseXml(char *FileName, int Mode,OSM_STAT_LIST *StatList, OSM_DATA_LIST *OsmDataList, OSM_INFO *OsmInfo)
{
	char	Data[4096];
	StatList->LineCount=0;
	StatList->NodeCount=0;
	StatList->EndNodeCount=0;
	StatList->TagCount=0;
	StatList->EndTagCount=0;
	StatList->WayCount=0;
	StatList->EndWayCount=0;
	StatList->NdCount=0;
	StatList->EndNdCount=0;
	StatList->RelationCount=0;
	StatList->EndRelationCount=0;
	StatList->MemberCount=0;
	StatList->EndMemberCount=0;

	FILE*	Fh=fopen(FileName,"r");
	if(Fh)
	{
		long	*TagRef=0;
		long	*NdRef=0;
		fseek(Fh,0,SEEK_END);
		unsigned long long FileSize=ftell(Fh);
		fseek(Fh,0,SEEK_SET);
		while(!feof(Fh))
		{
			char	*Ptr;

			fgets(Data,sizeof(Data),Fh);
			StatList->LineCount++;
			if(!(StatList->LineCount%100000))
				printf("Line %ld (%lu %%)\n",StatList->LineCount,(unsigned long)((100*(long long)ftell(Fh))/FileSize));
			if(Mode<1) continue;

			Ptr=strchr(Data,'<');
			if(Ptr)
			{
				Ptr++;
				bool End=(*Ptr=='/')?true:false;
				if(End) Ptr++;

				switch(*Ptr++)
				{
				case 'n' :
					switch(*Ptr++)
					{
					case 'd' :	// nd
					{
						if(!End)
						{
							StatList->NdCount++;
							if((Mode>1)&&NdRef) NdRef=ScanNd(Ptr,OsmDataList,NdRef);
						}
						else
							StatList->EndNdCount++;
					}
					case 'o' :
						if((*Ptr++=='d')&&(*Ptr++=='e'))	// node
						{
							if(!End)
							{
								StatList->NodeCount++;
								if(Mode>1) TagRef=ScanNode(Ptr,OsmDataList,OsmInfo);
							}
							else
							{
								StatList->EndNodeCount++;
								TagRef=0;
							}
						}
					}
					break;
				case 't' :
					if((*Ptr++=='a')&&(*Ptr++=='g'))	// tag
					{
						if(!End)
						{
							StatList->TagCount++;
							if((Mode>1)&&TagRef) TagRef=ScanTag(Ptr,OsmDataList,TagRef);
						}
						else
							StatList->EndTagCount++;
					}
					break;
				case 'w' :
					if((*Ptr++=='a')&&(*Ptr++=='y'))	// way
					{
						if(!End)
						{
							StatList->WayCount++;
							if(Mode>1)
							{
								TagRef=ScanWay(Ptr,OsmDataList);
								if(TagRef) NdRef=TagRef+1;	// !!! C hack : OsmWay.nd est juste après OsmWay.tag
							}
						}
						else
						{
							StatList->EndWayCount++;
							TagRef=NdRef=0;
						}
					}
					break;
				case 'm' :
					if((*Ptr++=='e')&&(*Ptr++=='m')&&(*Ptr++=='b')&&(*Ptr++=='e')&&(*Ptr++=='r'))	// member
					{
						if(!End)
							StatList->MemberCount++;
						else
							StatList->EndMemberCount++;
					}
					break;
				case 'r' :
					if((*Ptr++=='e')&&(*Ptr++=='l')&&(*Ptr++=='a')&&(*Ptr++=='t')&&(*Ptr++=='i')&&(*Ptr++=='o')&&(*Ptr++=='n'))	// relation
					{
						if(!End)
							StatList->RelationCount++;
						else
							StatList->EndRelationCount++;
					}
					break;
				}
			}
		}
		fclose(Fh);
	}
	printf("LineCount = %ld\n",StatList->LineCount);
	printf("NodeCount = %ld\n",StatList->NodeCount);
	printf("EndNodeCount = %ld\n",StatList->EndNodeCount);
	printf("TagCount = %ld\n",StatList->TagCount);
	printf("EndTagCount = %ld\n",StatList->EndTagCount);
	printf("WayCount = %ld\n",StatList->WayCount);
	printf("EndWayCount = %ld\n",StatList->EndWayCount);
	printf("NdCount = %ld\n",StatList->NdCount);
	printf("EndNdCount = %ld\n",StatList->EndNdCount);
	printf("RelationCount = %ld\n",StatList->RelationCount);
	printf("EndRelationCount = %ld\n",StatList->EndRelationCount);
	printf("MemberCount = %ld\n",StatList->MemberCount);
	printf("EndMemberCount = %ld\n",StatList->EndMemberCount);
}
