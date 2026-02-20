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

#define PLAYER_SPEED 6

typedef struct {
    int x;
    int y;
    bool pushable;
} cs_Entity;

void cs_InitEntities();
void cs_DrawEntities();
void cs_MovePlayer();

Texture2D player_texture;
Texture2D block_texture;
Texture2D crate_texture;

cs_Entity map_data[256];
size_t map_data_size = 0;

Rectangle player_rect = {
    .x = ((float)WIDTH - SPRITE_SIZE) / 2,
    .y = ((float)HEIGHT - SPRITE_SIZE) / 2,
    .width = SPRITE_SIZE - 10,
    .height = SPRITE_SIZE - 10,
};

int main() {
    InitWindow(WIDTH, HEIGHT, "Cargoshift");
    SetTargetFPS(60);

    player_texture = LoadTexture("./assets/PNG/Default size/Player/player_01.png");
    block_texture = LoadTexture("./assets/PNG/Default size/Blocks/block_01.png");
    crate_texture = LoadTexture("./assets/PNG/Default size/Crates/crate_02.png");

    cs_InitEntities();

    map_data[map_data_size++] = (cs_Entity){ .x = SPRITE_SIZE*2, .y = SPRITE_SIZE*2 };
    map_data[map_data_size++] = (cs_Entity){ .x = SPRITE_SIZE*2 + 2 * SPRITE_SIZE, .y = SPRITE_SIZE*2 };
    map_data[map_data_size++] = (cs_Entity){ .x = SPRITE_SIZE*2, .y = SPRITE_SIZE*4 };
    map_data[map_data_size++] = (cs_Entity){ .x = SPRITE_SIZE*2 + 2 * SPRITE_SIZE, .y = SPRITE_SIZE*4 };

    map_data[map_data_size++] = (cs_Entity){
        .x = SPRITE_SIZE * 7,
        .y = SPRITE_SIZE * 4,
        .pushable = true,
    };

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_R)) {
            map_data[map_data_size-1].x = SPRITE_SIZE * 7;
            map_data[map_data_size-1].y = SPRITE_SIZE * 4;
        }

        cs_MovePlayer();

        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            cs_DrawEntities();

            DrawRectangleRec(player_rect, LIGHTGRAY);
            DrawTexture(player_texture, player_rect.x - 5, player_rect.y - 5, WHITE);
        }
        EndDrawing();
    }

    UnloadTexture(block_texture);

    CloseWindow();
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

void cs_MovePlayer() {
    Vector2 move_vector = {0};
    if (IsKeyDown(KEY_A)) {
        move_vector.x = -1;
    }
    if (IsKeyDown(KEY_W)) {
        move_vector.y = -1;
    }
    if (IsKeyDown(KEY_S)) {
        move_vector.y = 1;
    }
    if (IsKeyDown(KEY_D)) {
        move_vector.x = 1;
    }

    move_vector = Vector2Normalize(move_vector);
    Rectangle new_rect = player_rect;
    new_rect.x += move_vector.x * PLAYER_SPEED;
    new_rect.y += move_vector.y * PLAYER_SPEED;

    Rectangle entity_rect = { .width = SPRITE_SIZE, .height = SPRITE_SIZE };

    for (size_t i = 0; i < map_data_size; i++) {
        cs_Entity *entity = &map_data[i];
        entity_rect.x = entity->x;
        entity_rect.y = entity->y;
        if (CheckCollisionRecs(new_rect, entity_rect)) {
            if (entity->pushable) {
                entity->x += move_vector.x * PLAYER_SPEED;
                entity->y += move_vector.y * PLAYER_SPEED;
            } else {
                return;
            }
        }
    }

    player_rect = new_rect;
}

void cs_InitEntities() {
    for (int i = 0, x = 0; i < BLOCK_WIDTH_COUNT; i++, x += SPRITE_SIZE) {
        map_data[map_data_size++] = (cs_Entity){ .x = x, .y = 0 };
        map_data[map_data_size++] = (cs_Entity){
            .x = x,
            .y = (BLOCK_HEIGHT_COUNT-1) * SPRITE_SIZE,
        };
    }
    for (int i = 0, y = SPRITE_SIZE; i < BLOCK_HEIGHT_COUNT-2; i++, y += SPRITE_SIZE) {
        map_data[map_data_size++] = (cs_Entity){ .x = 0, .y = y };
        map_data[map_data_size++] = (cs_Entity){
            .x = (BLOCK_WIDTH_COUNT-1) * SPRITE_SIZE,
            .y = y,
        };
    }
}
