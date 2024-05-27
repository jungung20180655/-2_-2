#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//2-4 까지 구현했습니다
// 파라미터
#define LEN_MIN 15 // 기차 길이
#define LEN_MAX 50
#define STM_MIN 0 // 마동석 체력
#define STM_MAX 5
#define PROB_MIN 10 // 확률
#define PROB_MAX 90
#define AGGRO_MIN 0 // 어그로범위
#define AGGRO_MAX 5

// 마동석 이동 방향
#define MOVE_LEFT 1
#define MOVE_STAY 0

// 좀비의 공격 대상
#define ATK_NONE 0
#define ATK_CITIZEN 1
#define ATK_DONGSEOK 2

// 마동석 행동
#define ACTION_REST 0
#define ACTION_PROVOKE 1
#define ACTION_PULL 2

// 학번 이름
void numname() {
    printf("프로그래밍 실습 프로젝트#2 부산헹2 \n");
    printf("학번: 20180655 이름: 우정웅\n");
}

// 기차 상태 구조체
typedef struct {
    int len;
    int stamina;
    int p;
    int Mp, Zp, Cp[3]; // 각 포지션
    int citizen_count;
    int game_over;
    int aggro;
    int escape_success; // 탈출 성공 여부
    int zombie_pulled; // 좀비가 붙들렸는지 여부
} GameState;

// 기차 생성 함수
void generateTrain(GameState* game) {
    // 기차 초기 배치
    printf("...\n");
    for (int i = 0; i < game->len; i++) {
        printf("#");
    }
    printf("\n");

    printf("#");
    for (int i = 1; i < game->len - 1; i++) {
        if (i == game->Cp[0] || i == game->Cp[1] || i == game->Cp[2]) {
            printf("C");
        }
        else if (i == game->Zp) {
            printf("Z");
        }
        else if (i == game->Mp) {
            printf("M");
        }
        else {
            printf(" ");
        }
    }
    printf("#\n");

    for (int i = 0; i < game->len; i++) {
        printf("#");
    }
    printf("\n...\n");
}

// 각 길이, 스테미나, 확률 입력 함수
void input(int* len, int* stamina, int* p) {
    printf("train length(%d~%d)>> ", LEN_MIN, LEN_MAX);
    scanf_s("%d", len);
    while (*len < LEN_MIN || *len > LEN_MAX) {
        printf("train length(%d~%d)>> ", LEN_MIN, LEN_MAX);
        scanf_s("%d", len);
    }
    printf("madongseok stamina(%d~%d)>> ", STM_MIN, STM_MAX);
    scanf_s("%d", stamina);
    while (*stamina < STM_MIN || *stamina > STM_MAX) {
        printf("madongseok stamina(%d~%d)>> ", STM_MIN, STM_MAX);
        scanf_s("%d", stamina);
    }
    printf("percentile probability 'p'(%d~%d)>> ", PROB_MIN, PROB_MAX);
    scanf_s("%d", p);
    printf("\n");
    while (*p < PROB_MIN || *p > PROB_MAX) {
        printf("Invalid percentile probability. Please enter a value between %d and %d.\n", PROB_MIN, PROB_MAX);
        scanf_s("%d", p);
        printf("\n");
    }
}

// 게임 초기화 함수
void initGame(GameState* game) {
    game->citizen_count = 3;
    game->game_over = 0;
    game->aggro = 0;
    game->escape_success = 0;
    game->zombie_pulled = 0;

    game->Mp = game->len - 3;
    game->Zp = game->len - 4;
    game->Cp[0] = game->len - 5;
    game->Cp[1] = rand() % (game->len - 6);
    game->Cp[2] = rand() % (game->len - 6);
    if (game->Cp[1] == game->Cp[0]) game->Cp[1]++;
    if (game->Cp[2] == game->Cp[0] || game->Cp[2] == game->Cp[1]) game->Cp[2]++;
}

// 시민 이동 함수
void moveCitizens(GameState* game) {
    for (int i = 0; i < 3; i++) {
        if (game->Cp[i] != -1) {
            printf("citizen%d: %d -> %d (aggro: %d)\n", i, game->Cp[i], game->Cp[i] - 1, game->aggro);
            game->Cp[i]--;
        }
        else {
            printf("citizen%d does nothing.\n", i);
        }
        // 시민 탈출 여부 확인
        if (game->Cp[i] == -1 || game->Cp[i] <= 0) {
            game->escape_success = 1;
            printf("citizen%d has escaped successfully!\n", i);
            game->Cp[i] = -1; // 시민이 탈출하면 더 이상 움직이지 않음
        }
    }
}

// 좀비 이동 함수
void moveZombie(GameState* game) {
    if (game->zombie_pulled) {
        printf("zombie cannot move this turn.\n");
        game->zombie_pulled = 0; // 한번 붙들린 후 다음 턴에는 이동 가능
    }
    else {
        printf("zombie: %d -> %d\n", game->Zp, game->Zp - 1);
        game->Zp--;
    }
}

// 마동석 행동 함수
void madongseokAction(GameState* game, int action) {
    if (action == MOVE_LEFT) {
        printf("madongseok: move left from %d to %d\n", game->Mp, game->Mp - 1);
        game->Mp--;
    }
    else {
        printf("madongseok: stay at %d\n", game->Mp);
    }
}

// 좀비 공격 함수
void zombieAttack(GameState* game) {
    int attacked = 0;
    if (rand() % 100 < game->p) {
        for (int i = 0; i < 3; i++) {
            if (game->Cp[i] == game->Zp + 1 || game->Cp[i] == game->Zp - 1) {
                printf("zombie attacked citizen%d.\n", i);
                game->citizen_count--;
                game->Cp[i] = -1; // Citizen is dead
                attacked = 1;
                break;
            }
        }
    }
    if (!attacked) {
        printf("zombie attacked nobody.\n");
    }
}

// 게임 상태 출력 함수
void printGameState(GameState* game) {
    generateTrain(game);
    printf("madongseok: %d (aggro: %d, stamina: %d)\n", game->Mp, game->aggro, game->stamina);
}

// 메인 함수
int main() {
    srand((unsigned int)time(NULL));

    GameState game;
    int action;

    // 학번과 이름 출력
    numname();

    // 사용자 입력 받기
    input(&game.len, &game.stamina, &game.p);

    // 게임 초기화
    initGame(&game);

    // 초기 상태 출력
    printGameState(&game);

    // 게임 루프
    while (!game.game_over) {
        // 이동 페이즈
        moveCitizens(&game);
        moveZombie(&game);

        // 마동석 이동 입력 및 처리
        printf("madongseok move(0: stay, 1: left): ");
        scanf_s("%d", &action);
        madongseokAction(&game, action);
        printGameState(&game);

        // 좀비 공격 처리
        zombieAttack(&game);

        // 마동석 행동 입력 및 처리
        printf("madongseok action(0: rest, 1: provoke, 2: pull)>> ");
        scanf_s("%d", &action);

        int prev_aggro = game.aggro;
        int prev_stamina = game.stamina;

        if (action == ACTION_REST) {
            game.stamina++;
            if (game.stamina > STM_MAX) game.stamina = STM_MAX;
            printf("madongseok rests\n");
        }
        else if (action == ACTION_PROVOKE) {
            if (game.stamina > 0) {
                game.stamina--;
                game.aggro++;
                printf("madongseok provokes\n");
            }
            else {
                printf("Not enough stamina to provoke.\n");
            }
        }
        else if (action == ACTION_PULL) {
            if (game.stamina > 0) {
                game.stamina--;
                game.aggro += 2;
                if (rand() % 100 >= game.p) { // (100 - p)% 확률로 성공
                    game.zombie_pulled = 1;
                    printf("madongseok pulled zombie... Next turn, it can't move\n");
                }
                else {
                    printf("madongseok failed to pull zombie\n");
                }
            }
            else {
                printf("Not enough stamina to pull.\n");
            }
        }

        printf("madongseok: %d (aggro: %d -> %d, stamina: %d -> %d)\n",
            game.Mp, prev_aggro, game.aggro, prev_stamina, game.stamina);

        // 게임 상태 출력
        printGameState(&game);

        // 게임 종료 조건 체크
        if (game.citizen_count == 0) {
            printf("Game over. All citizens are dead.\n");
            game.game_over = 1;
        }

        if (game.Zp <= 0) {
            printf("All zombies are dead. You win!\n");
            game.game_over = 1;
        }

        if (game.escape_success) {
            printf("At least one citizen has escaped successfully. You win!\n");
            game.game_over = 1;
        }
    }

    return 0;
}

