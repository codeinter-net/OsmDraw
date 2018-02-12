// ----------------------------------------------------------------


#include <string.h>
#include "OsmXml.h"
#include "Prefs.h"

ulong Hash(char *Text)
{
	unsigned long long h=0;
	while(*Text)
	{
		h=(h<<5)|(*Text++&0x1F);
	}
	return(h^(h>>32));
}

ulong ScanKey(char* Key)
{
	for(ulong i=0; i<10000L; i++)
	{
		if(!KeyList2[i].Name) break;
		int h=Hash(Key);
//		if(!strcmp(Key,KeyList2[i].Name)) return(KeyList2[i].Id);	// A optimiser avec des clés de haschage
		if((h==KeyList2[i].Hash)&&!strcmp(Key,KeyList2[i].Name)) return(KeyList2[i].Id);	// A optimiser avec des clés de haschage
//		if(!*KeyList[i]) break;
//		if(!strcmp(Key,KeyList[i])) return(i);	// A optimiser avec des clés de haschage
	}
	return(-1L);
}

char* GetKey(ulong Key)
{
	for(ulong i=0; i<10000L; i++)
	{
		if(!KeyList2[i].Name) break;
		if(KeyList2[i].Id==Key) return(KeyList2[i].Name);
	}
	return(0);
//	return(KeyList[Key]);
}

char* SearchTag(OSM_DATA_LIST *OsmDataList, ulong Index,ulong Key)
{
	for(;;)
	{
		if((Index<0)||(Index>=OsmDataList->TagNum)) return(0);
		if(OsmDataList->Tag[Index].key==Key) return(OsmDataList->Text+OsmDataList->Tag[Index].val);
		Index=OsmDataList->Tag[Index].tag;
	}
	return(0);
}

ulong GetNodeIndex(OSM_DATA_LIST *OsmDataList, ulong Node)
{
	OSM_NODE* OsmNodePtr=OsmDataList->Node;
	for(int i=1+OsmDataList->NodeNum;--i>0;OsmNodePtr++)
		if(OsmNodePtr->id==Node) return(OsmDataList->NodeNum-i);
	return(-1);
}

