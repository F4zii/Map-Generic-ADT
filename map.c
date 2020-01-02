//
// Created by Iddo Soreq on 12/4/19.
//

#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define INITIAL_MAP_SIZE 10
#define EXPAND_RATE 2
#define ELEMENT_NOT_FOUND -1

struct Map_t {
    MapKeyElement *keys;
    MapDataElement *data;
    copyMapDataElements copyDataElement; // a function pointer to copy data elements
    copyMapKeyElements copyKeyElement; // a function pointer to copy key elements
    freeMapDataElements freeDataElement; // a function pointer to free data elements
    freeMapKeyElements freeKeyElement; // a function pointer to free key elements
    compareMapKeyElements compareKeyElements; // a function pointer to compare key elements
    int iterator;
    int size;
    int maxSize;
};




/**
*	mapFind: Returns the index of existing key in the map.
*			Iterator status unchanged
*
* @param map - The map for which to get the key index from.
* @param keyElement - The key element which need to be found and whose index
* we want to get.
* @return
*  -1 if not found, else we return the index of the keyElement in map
*/
static int mapFind(Map map, MapKeyElement keyElement);


/**
*	mapInitialize:
*			Iterator status changed to 0 (init / default iterator value)
* @param map - The map for which to get the key index from.
* *
* @param copyDataElement - Function pointer to be used for copying data elements into
*  	the map or when copying the map.
* @param copyKeyElement - Function pointer to be used for copying key elements into
*  	the map or when copying the map.
* @param freeDataElement - Function pointer to be used for removing data elements from
* 		the map
* @param freeKeyElement - Function pointer to be used for removing key elements from
* 		the map
* @param compareKeyElements - Function pointer to be used for comparing key elements
* 		inside the map. Used to check if new elements already exist in the map.
*/

static void mapInitialize(Map new_map,
                          copyMapDataElements copyDataElement,
                          copyMapKeyElements copyKeyElement,
                          freeMapDataElements freeDataElement,
                          freeMapKeyElements freeKeyElement,
                          compareMapKeyElements compareKeyElements);

/**
*	mapExpand: Allocates more memory for the map arrays (data,keys)
*			Iterator status unchanged
*
* @param map - The map we want to expand.
* @return
*  MAP_SUCCESS if we successed to allocate more memory for map, else we return MAP_OUT_OF_MEMORY
*/
static MapResult mapExpand(Map map);


/*
* mapUpdateAfterRemove: Updates the data and keys after a pair removal
* Iterator status unchanged
* @param map - The map we want to update.
* @param index - the index of the pair that was removed
 */
static void mapUpdateAfterRemove(Map map, int index);

// returns the index of the inserted key
static int mapInsertSorted(Map map, MapKeyElement key, MapDataElement data);

/*
static MapKeyElement copyInt(MapKeyElement n) {
    if (!n) {
        return NULL;
    }
    int *copy = malloc(sizeof(*copy));
    if (!copy) {
        return NULL;
    }
    *copy = *(int *) n;
    return copy;
}

static void freeInt(MapDataElement e) {
    free(e);
}

static int compareInts(MapKeyElement int1, MapKeyElement int2) {
    return (*(int *) int1 - *(int *) int2);
}

int main(int argc, char* argv[]) {
    // TODO Testing
    int n = 5, n1;
    Map map = mapCreate(copyInt, copyInt, freeInt, freeInt, compareInts);
    MapKeyElement k = map->copyKeyElement((void*)&n);
    for (int i=0; i < 10; i++) {
        n1 = 9-i;
        mapPut(map, &i, &n1);
    }
    Map copied_map = mapCopy(map);
    MAP_FOREACH(MapKeyElement, elem, copied_map) {
        printf("CM Key: %d ,Data: %d\n", *(int*)elem, *(int*)mapGet(map,elem));
    }
    mapRemove(copied_map, k);
    printf("%d\n", *(int*)map->keys[4]);
    // printf("CM Key: %d ,Data: %d\n", *(int*)k, *(int*)mapGet(copied_map, k));
    printf("CM Size: %d\n", copied_map->size);
    mapRemove(map,k);
    printf("Map Size: %d\n", map->size);
    //printf("%d \n", *(int*)mapGet(map,k));
    return 0;
}


*/



static int mapFind(Map map, MapKeyElement keyElement) {
    for (int i=0;i<map->size;i++) {
        if ((map->compareKeyElements(keyElement, map->keys[i])) == 0) {
            return i;
        }
    }
    return ELEMENT_NOT_FOUND;
}




MapDataElement mapGet(Map map, MapKeyElement keyElement) {
    assert(map != NULL);
    if (keyElement == NULL) {
        return NULL;
    }
    int index = mapFind(map, keyElement);
    if (index == ELEMENT_NOT_FOUND) {
        return NULL; // we didn't find any matching key, no data to return.
    }
    return map->data[index];

}

int mapGetSize(Map map) {
    if (map == NULL) {
        return -1; // magic number, but following the map.h rules
    }
    assert(map->size > -1);
    return map->size;
}



MapKeyElement mapGetFirst(Map map) {
    if (map == NULL || map->size <= 0) {
        return NULL;
    }
    map->iterator = 0;
    return map->keys[0];
}


MapKeyElement mapGetNext(Map map) {
    assert(map != NULL);
    if (map->iterator >= map->size-1) {
        return NULL;
    }
    map->iterator++;
    return map->keys[map->iterator];
}


static MapResult mapExpand(Map map) {
    assert(map != NULL);
    map->keys = realloc(map->keys, map->maxSize * EXPAND_RATE * sizeof(void*));
    map->data = realloc(map->data, map->maxSize * EXPAND_RATE * sizeof(void*));
    if (map->keys == NULL || map->data == NULL) {
        return MAP_OUT_OF_MEMORY;
    }
    map->maxSize*= EXPAND_RATE;
    return MAP_SUCCESS;

}



MapResult mapClear(Map map) {
    if (map == NULL) {
        return MAP_NULL_ARGUMENT;
    }
    int original_size = map->size;
    for (int i=map->size-1;i>-1;i--) {

        mapRemove(map, map->keys[i]);
        //free(map->keys[i]);
        //free(map->data[i]);
        //map->freeKeyElement(map->keys[i]);
        //map->freeDataElement(map->data[i]);
    }
    map->size = original_size;
    // map->iterator = 0; optional, but already done in mapRemove
    return MAP_SUCCESS;
}






void mapDestroy(Map map) {
    if (map == NULL) return;
    for (int i=0;i<map->size;i++) {
        map->freeKeyElement(map->keys[i]);
        map->freeDataElement(map->data[i]);
    }
    /*
    while(map->size > 0) {
        mapRemove(map, mapGetFirst(map));
    }*/
    free(map->keys);
    free(map->data);
    free(map);
}


MapResult mapRemove(Map map, MapKeyElement keyElement) {
    if (keyElement == NULL || map == NULL) {
        return MAP_NULL_ARGUMENT;
    }
    int index = mapFind(map, keyElement);
    if (index == ELEMENT_NOT_FOUND) {
        return MAP_ITEM_DOES_NOT_EXIST;
    }
    //map->keys[index] = map->keys[map->size-1];
    //map->data[index] = map->data[map->size-1];
    map->freeKeyElement(map->keys[index]);
    map->freeDataElement(map->data[index]);
    mapUpdateAfterRemove(map, index);
    map->size--;
    map->iterator = 0; // update the iterator , it can be out of bounds after the size change
    return MAP_SUCCESS;
}


MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement) {
    assert(map != NULL);
    if (keyElement == NULL || dataElement==NULL) return MAP_NULL_ARGUMENT;
    MapKeyElement copiedKeyElement = map->copyKeyElement(keyElement);
    MapDataElement copiedDataElement = map->copyDataElement(dataElement);
    int index = mapFind(map, keyElement);
    if (index == ELEMENT_NOT_FOUND) { // key does not exist in the map, we need to create one
        if (map->size == map->maxSize && mapExpand(map) == MAP_OUT_OF_MEMORY) {
            return MAP_OUT_OF_MEMORY;
        }
        index = mapInsertSorted(map, copiedKeyElement, copiedDataElement);
        if (index == ELEMENT_NOT_FOUND) return MAP_OUT_OF_MEMORY;
        //map->data[index] = copiedDataElement;
        map->size++; // update the map size, since we added a key
        return MAP_SUCCESS;
    }
    // the case for already existing key in the map:
    map->freeDataElement(map->data[index]);
    map->data[index] = copiedDataElement; // insert the data for the existing key
    return MAP_SUCCESS;
}



static void mapInitialize(Map new_map,
                          copyMapDataElements copyDataElement,
                          copyMapKeyElements copyKeyElement,
                          freeMapDataElements freeDataElement,
                          freeMapKeyElements freeKeyElement,
                          compareMapKeyElements compareKeyElements) {
    assert(new_map != NULL); // unnecessary , but still, we should check it
    new_map->size = 0;
    new_map->iterator = 0;
    new_map->maxSize = INITIAL_MAP_SIZE;
    new_map->copyKeyElement = copyKeyElement;
    new_map->copyDataElement = copyDataElement;
    new_map->freeKeyElement = freeKeyElement;
    new_map->freeDataElement = freeDataElement;
    new_map->compareKeyElements = compareKeyElements;
}



bool mapContains(Map map, MapKeyElement element) {
    int index = mapFind(map, element);
    if (index == ELEMENT_NOT_FOUND) {
        return false;
    }
    return true;
}


Map mapCreate(copyMapDataElements copyDataElement, copyMapKeyElements copyKeyElement, freeMapDataElements freeDataElement, freeMapKeyElements freeKeyElement, compareMapKeyElements compareKeyElements) {
    assert(copyDataElement != NULL && copyKeyElement != NULL && freeDataElement != NULL && freeKeyElement != NULL && compareKeyElements != NULL);
    Map new_map = malloc(sizeof(*new_map));
    if (new_map == NULL) {
        return NULL;
    }
    new_map->keys = malloc(INITIAL_MAP_SIZE * sizeof(MapKeyElement)); // array of void pointers in the size of INITIAL_MAP_SIZE
    new_map->data = malloc(INITIAL_MAP_SIZE * sizeof(MapDataElement)); // array of void pointers in the size of INITIAL_MAP_SIZE
    // if one of the allocations failed, we should stop creating the map and free allocated memory and return NULL
    if (new_map->keys == NULL || new_map->data == NULL) {
        if (new_map->data != NULL) {
            free(new_map->data);
        }
        if (new_map->keys != NULL) {
            free(new_map->keys);
        }
        free(new_map);
        return NULL;
    }
    // initialize the map fields, pass in the users functions and map size,maxSize and iterator
    mapInitialize(new_map, copyDataElement, copyKeyElement, freeDataElement, freeKeyElement, compareKeyElements);
    return new_map;
}




Map mapCopy(Map map) {
    if (map == NULL) return NULL;
    Map new_map = mapCreate(map->copyDataElement,map->copyKeyElement, map->freeDataElement, map->freeKeyElement,map->compareKeyElements);
    if (new_map == NULL) return NULL;
    for (int i = 0; i < map->size; i++) {
        if (mapPut(new_map, map->keys[i], map->data[i]) == MAP_OUT_OF_MEMORY) {
            mapDestroy(new_map);
            return NULL;
        }
    }
    new_map->size = map->size;
    new_map->maxSize = map->maxSize;
    new_map->iterator = 0; // ex1.pdf orders

    return new_map;
}

static int mapInsertSorted(Map map, MapKeyElement key, MapDataElement data) {
    if (map->size >= map->maxSize) // just to make sure, this condition is non-likely to happen
        return ELEMENT_NOT_FOUND;

    int i;
    for (i = map->size - 1;
         (i >= 0 && map->compareKeyElements(map->keys[i], key) > 0);i--) {
        map->keys[i + 1] = map->keys[i];
        map->data[i+1] = map->data[i];
    }

    map->keys[i + 1] = key;
    map->data[i + 1] = data;
    return i+1;

}

static void mapUpdateAfterRemove(Map map, int index) {
    int i;
    for (i=index ; i < map->size-1 ; i++) {
        map->keys[i] = map->keys[i+1];
        map->data[i] = map->data[i+1];
    }
}
// todo mapClear size check
