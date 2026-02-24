#include <assert.h>
#include <stdio.h>
#include <raylib.h>
#include <stdbool.h>
#include <raymath.h>

#define BLOCK_WIDTH_COUNT  13
#define BLOCK_HEIGHT_COUNT 10
#define SPRITE_SIZE 64

#define WIDTH      (BLOCK_WIDTH_COUNT) * (SPRITE_SIZE)
#define HEIGHT     (BLOCK_HEIGHT_COUNT) * (SPRITE_SIZE)
#define FONT_SIZE  32

#define ENTITY_CAPACITY 256

#define PLAYER_SPEED 250

typedef struct {
    int x;
    int y;
    bool pushable;
} cs_Entity;

Vector2 cs_GetPlayerInput(float dt);
void cs_MovePlayer(Vector2 dir);
cs_Entity *cs_GetCollidedEntity(cs_Entity *target, Vector2 dir);
void cs_TryMoveCrate(cs_Entity *crate, Vector2 dir);
void cs_InitEntities(void);
void cs_DrawEntities(void);

Texture2D player_texture;
Texture2D block_texture;
Texture2D crate_texture;

cs_Entity map_data[256];
size_t map_data_size = 0;
size_t crate_index;

Rectangle player_rect = {
    .x = ((float)WIDTH - SPRITE_SIZE) / 2,
    .y = ((float)HEIGHT - SPRITE_SIZE) / 2,
    .width = SPRITE_SIZE - 28,
    .height = SPRITE_SIZE - 20,
};

int main() {
    InitWindow(WIDTH, HEIGHT, "Cargoshift");
    SetTargetFPS(60);

    player_texture = LoadTexture("./assets/PNG/Default size/Player/player_01.png");
    block_texture = LoadTexture("./assets/PNG/Default size/Blocks/block_01.png");
    crate_texture = LoadTexture("./assets/PNG/Default size/Crates/crate_02.png");

    cs_InitEntities();

    map_data[map_data_size++] = (cs_Entity){
        .x = SPRITE_SIZE*2.5,
        .y = SPRITE_SIZE*2.5
    };
    map_data[map_data_size++] = (cs_Entity){
        .x = SPRITE_SIZE*2.5 + 2.5*SPRITE_SIZE,
        .y = SPRITE_SIZE*2.5
    };
    map_data[map_data_size++] = (cs_Entity){
        .x = SPRITE_SIZE*2.5,
        .y = SPRITE_SIZE*5
    };
    map_data[map_data_size++] = (cs_Entity){
        .x = SPRITE_SIZE*2.5 + 2.5*SPRITE_SIZE,
        .y = SPRITE_SIZE*5
    };

    crate_index = map_data_size;
    map_data[map_data_size++] = (cs_Entity){
        .x = SPRITE_SIZE*7,
        .y = SPRITE_SIZE*4,
        .pushable = true
    };

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (IsKeyPressed(KEY_R)) {
            map_data[crate_index].x = SPRITE_SIZE * 7;
            map_data[crate_index].y = SPRITE_SIZE * 4;
        }

        Vector2 player_dir = cs_GetPlayerInput(dt);
        cs_MovePlayer(player_dir);

        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            cs_DrawEntities();

            DrawRectangleRec(player_rect, LIGHTGRAY);
            DrawTexture(player_texture, player_rect.x - 14, player_rect.y - 10, WHITE);
        }
        EndDrawing();
    }

    UnloadTexture(block_texture);

    CloseWindow();
}


void cs_MovePlayer(Vector2 dir) {
    if (dir.x != 0) {
        Vector2 moveVector = { .x = dir.x };
        cs_Entity *entity = cs_GetCollidedEntity(NULL, moveVector);
        if (entity == NULL) player_rect.x += dir.x;
        else if (entity->pushable) cs_TryMoveCrate(entity, moveVector);
    }

    if (dir.y != 0) {
        Vector2 moveVector = { .y = dir.y };
        cs_Entity *entity = cs_GetCollidedEntity(NULL, (Vector2){ .y = dir.y });
        if (entity == NULL) player_rect.y += dir.y;
        else if (entity->pushable) cs_TryMoveCrate(entity, moveVector);
    }
}


Vector2 cs_GetPlayerInput(float dt) {
    Vector2 dir = {
        .x = IsKeyDown(KEY_D) - IsKeyDown(KEY_A),
        .y = IsKeyDown(KEY_S) - IsKeyDown(KEY_W)
    };
    dir = Vector2Normalize(dir);
    dir.x *= PLAYER_SPEED * dt;
    dir.y *= PLAYER_SPEED * dt;
    return dir;
}

/*
 * Returns a pointer to the first entity that would collide with `target`
 * if it moved by `dir`.
 *
 * Returns NULL if no collision occurs.
 *
 * If `target` is NULL, the player is used as the moving entity.
 *
 * Note: Only axis-aligned movement is supported (one component of `dir`
 * must be zero).
 */
cs_Entity *cs_GetCollidedEntity(cs_Entity *target, Vector2 dir) {
    assert((dir.x == 0 || dir.y == 0) && "Only check one axis at a time");

    Rectangle new_rect = { .width = SPRITE_SIZE, .height = SPRITE_SIZE };
    if (target == NULL) {
        new_rect = player_rect;
        new_rect.x += dir.x;
        new_rect.y += dir.y;
    } else {
        new_rect.x = target->x + dir.x;
        new_rect.y = target->y + dir.y;
    }

    Rectangle entity_rect = { .width = SPRITE_SIZE, .height = SPRITE_SIZE };

    for (size_t i = 0; i < map_data_size; i++) {
        cs_Entity *entity = &map_data[i];
        if (target == entity) continue;

        entity_rect.x = entity->x;
        entity_rect.y = entity->y;
        if (CheckCollisionRecs(new_rect, entity_rect)) {
            return entity;
        }
    }

    return NULL;
}

void cs_TryMoveCrate(cs_Entity *crate, Vector2 dir) {
    if (cs_GetCollidedEntity(crate, dir) == NULL) {
        crate->x += dir.x;
        crate->y += dir.y;
    }
}

void cs_DrawEntities() {
    for (size_t i = 0; i < map_data_size; i++) {
        const cs_Entity *entity = &map_data[i];
        if (entity->pushable) {
            DrawTexture(crate_texture, entity->x, entity->y, WHITE);
        } else {
            DrawTexture(block_texture, entity->x, entity->y, WHITE);
        }
    }
}

void cs_InitEntities() {
    for (int i = 0, x = 0; i < BLOCK_WIDTH_COUNT; i++, x += SPRITE_SIZE) {
        map_data[map_data_size++] = (cs_Entity){ .x = x, .y = 0 };
        map_data[map_data_size++] = (cs_Entity){
            .x = x,
            .y = (BLOCK_HEIGHT_COUNT-1) * SPRITE_SIZE
        };
    }
    for (int i = 0, y = SPRITE_SIZE; i < BLOCK_HEIGHT_COUNT-2; i++, y += SPRITE_SIZE) {
        map_data[map_data_size++] = (cs_Entity){ .x = 0, .y = y };
        map_data[map_data_size++] = (cs_Entity){
            .x = (BLOCK_WIDTH_COUNT-1) * SPRITE_SIZE,
            .y = y
        };
    }
}
