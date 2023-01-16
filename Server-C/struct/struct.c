#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "struct.h"

Room* rooms;
int totalRoom = 0;

void initRoom(){
    rooms = (Room*)malloc(sizeof(Room) * 64);
}

void addRoom(Room* room) {
    rooms[totalRoom++] = *room; 
}

void printRooms(){
    for(int i = 0; i < totalRoom; ++i){
        printf("(%d): %s \n", i, rooms[i].name);
    }
}