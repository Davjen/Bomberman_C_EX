#include "clove.h"
#include "../bomberman.h"



CLOVE_TEST(MovableFixPositionX)
{
    level_t level;
    int32_t map[] = {0, 0, 0, 0};
    level_init(&level, 4, 4, 8, map);
    movable_t movable = { 1, 0, 4, 4};
    move_on_level(&level, &movable, -10, 0);
    CLOVE_FLOAT_EQ(movable.x, 0);
}

CLOVE_TEST(MovableFixPositionY)
{
    level_t level;
    int32_t map[] = {0, 0, 0, 0};
    level_init(&level, 4, 4, 8, map);
    movable_t movable = { 1, 0, 4, 4};
    move_on_level(&level, &movable, 0, -30);
    CLOVE_FLOAT_EQ(movable.y, 0);
}

CLOVE_TEST(MovableFixPositionWidth)
{
    level_t level;
    int32_t map[] = {0, 0, 0, 0};
    level_init(&level, 4, 4, 8, map);
    movable_t movable = { 1, 0, 4, 4};
    move_on_level(&level, &movable, 50, 0);
    CLOVE_FLOAT_EQ(movable.x, 28);
}

CLOVE_TEST(MovableFixPositionHeight)
{
    level_t level;
    int32_t map[] = {0, 0, 0, 0};
    level_init(&level, 4, 4, 8, map);
    movable_t movable = { 1, 0, 4, 4};
    move_on_level(&level, &movable, 0, 100);
    CLOVE_FLOAT_EQ(movable.y, 28);
}

CLOVE_TEST(MovableWallHit)
{
    level_t level;
    int32_t map[] = {0, 0x101, 0, 0};
    level_init(&level, 2, 2, 64, map);
    movable_t movable = { 1, 1, 32, 32};
    move_on_level(&level, &movable, 80, 0);
    CLOVE_FLOAT_EQ(movable.x, 32);
}

CLOVE_TEST(MovableWallOnHeadRight){
    level_t level;
    int32_t map[] = {0, 0x101, 0, 0};
    level_init(&level, 2, 2, 64, map);
    movable_t movable = { 1, 50, 32, 32};
    move_on_level(&level, &movable, 80, 0);
    CLOVE_FLOAT_EQ(movable.x, 32);
}
CLOVE_TEST(MovableWallOnFeatRight){
    level_t level;
    int32_t map[] = {0, 0, 0, 0x101};
    level_init(&level, 2, 2, 64, map);
    movable_t movable = { 1, 50, 32, 32};
    move_on_level(&level, &movable, 80, 0);
    CLOVE_FLOAT_EQ(movable.x, 32);
}
CLOVE_TEST(MovableWallOnHeadLeft){
    level_t level;
    int32_t map[] = {0x101, 0, 0, 0};
    level_init(&level, 2, 2, 64, map);
    movable_t movable = { 70, 50, 32, 32};
    move_on_level(&level, &movable, -30, 0);
    CLOVE_FLOAT_EQ(movable.x, 65);
}
CLOVE_TEST(MovableWallOnFeetLeft){
    level_t level;
    int32_t map[] = {0, 0, 0x101, 0};
    level_init(&level, 2, 2, 64, map);
    movable_t movable = { 70, 50, 32, 32};
    move_on_level(&level, &movable, -30, 0);
    CLOVE_FLOAT_EQ(movable.x, 65);
}

CLOVE_SUITE(MovableSuite)
{
    CLOVE_SUITE_TESTS(
        MovableFixPositionX,
        MovableFixPositionY,
        MovableFixPositionWidth,
        MovableFixPositionHeight,
        MovableWallHit,
        MovableWallOnHeadRight,
        MovableWallOnFeatRight,
        MovableWallOnHeadLeft,
        MovableWallOnFeetLeft
    );
}