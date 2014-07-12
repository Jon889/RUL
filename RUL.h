#ifndef RUL_H
#define RUL_H
#include "typedefs.h"
typedef int NetworkFlag;
extern const NetworkFlag NetworkFlag_None;
extern const NetworkFlag NetworkFlag_Left_45;
extern const NetworkFlag NetworkFlag_Straight;
extern const NetworkFlag NetworkFlag_Right_45;
extern const NetworkFlag NetworkFlag_Shared_Median;
extern const NetworkFlag NetworkFlag_Left_Blend;
extern const NetworkFlag NetworkFlag_Right_Blend;
typedef enum {West, North, East, South} Direction;
typedef int TGIID;
typedef TGIID InstanceID;
typedef int Rotation;
extern const Rotation Rotation_0;
extern const Rotation Rotation_90;
extern const Rotation Rotation_180;
extern const Rotation Rotation_270;
typedef int Flip;
extern const Flip Flip_None;
extern const Flip Flip_Mirrored;
typedef struct {
    Rotation rotation;
    Flip flip;
} Transformation;


char * seekAfter(char **str, char chr);
#endif
