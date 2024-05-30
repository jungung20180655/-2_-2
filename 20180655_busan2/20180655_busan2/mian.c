#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 파라미터 설정
#define LEN_MIN 15 // 기차 길이 최소값
#define LEN_MAX 50 // 기차 길이 최대값
#define STM_MIN 0  // 마동석 스테미나 최소값
#define STM_MAX 5  // 마동석 스테미나 최대값
#define PROB_MIN 10 // 확률 최소값
#define PROB_MAX 90 // 확률 최대값
#define AGGRO_MIN 0 // 어그로 최소값
#define AGGRO_MAX 5 // 어그로 최대값
#define STAGES 3    // 스테이지 수

// 이동 행동
#define MOVE_LEFT 1
#define MOVE_STAY 0

// 공격 대상
#define ATK_NONE 0
#define ATK_CITIZEN 1
#define ATK_DONGSEOK 2

// 행동 종류
#define ACTION_REST 0
#define ACTION_PROVOKE 1
#define ACTION_PULL 2

// 빌런 관련
#define STAGE2_VILLAIN 1

// 학번과 이름 출력 함수
void numname() {
    printf("프로그래밍 실습 과제#2 - 부산행(2)\n");
    printf("학번: 20180655 이름: 우정웅\n");
}

// 게임 상태 구조체
typedef struct {
    int len;
    int stamina;
    int p;
    int Mp, Zp, Vp, Cp[10]; // 캐릭터 위치
    int citizen_count;
    int game_over;
    int aggro;
    int escape_success;
    int zombie_pulled;
    int current_stage;
    int enhanced_zombies[10];
} GameState;

// 기차 상태 출력 함수
void generateTrain(GameState game) {
    printf("...\n");
    for (int i = 0; i < game.len; i++) {
        printf("#");
    }
    printf("\n");

    printf("#");
    for (int i = 1; i < game.len - 1; i++) {
        if (i == game.Mp) {
            printf("M");
        }
        else if (i == game.Zp) {
            printf("Z");
        }
        else if (game.current_stage == 2 && i == game.Vp) {
            printf("V");
        }
        else {
            int is_citizen = 0;
            for (int j = 0; j < game.citizen_count; j++) {
                if (i == game.Cp[j]) {
                    printf("C");
                    is_citizen = 1;
                    break;
                }
            }
            if (!is_citizen) {
                printf(" ");
            }
        }
    }
    printf("#\n");

    for (int i = 0; i < game.len; i++) {
        printf("#");
    }
    printf("\n...\n");
}

// 사용자 입력 함수
void input(int* len, int* stamina, int* p) {
    printf("기차 길이(%d~%d)>> ", LEN_MIN, LEN_MAX);
    scanf_s("%d", len);
    while (*len < LEN_MIN || *len > LEN_MAX) {
        printf("기차 길이(%d~%d)>> ", LEN_MIN, LEN_MAX);
        scanf_s("%d", len);
    }
    printf("마동석 스테미나(%d~%d)>> ", STM_MIN, STM_MAX);
    scanf_s("%d", stamina);
    while (*stamina < STM_MIN || *stamina > STM_MAX) {
        printf("마동석 스테미나(%d~%d)>> ", STM_MIN, STM_MAX);
        scanf_s("%d", stamina);
    }
    printf("확률 'p'(%d~%d)>> ", PROB_MIN, PROB_MAX);
    scanf_s("%d", p);
    printf("\n");
    while (*p < PROB_MIN || *p > PROB_MAX) {
        printf("잘못된 확률입니다. %d와 %d 사이의 값을 입력하세요.\n", PROB_MIN, PROB_MAX);
        scanf_s("%d", p);
        printf("\n");
    }
}

// 게임 초기화 함수
void initGame(GameState* game) {
    game->citizen_count = game->len / 4 + rand() % ((game->len / 2) - (game->len / 4) + 1);
    game->game_over = 0;
    game->aggro = 0;
    game->escape_success = 0;
    game->zombie_pulled = 0;
    game->current_stage = 1;

    for (int i = 0; i < 10; i++) {
        game->enhanced_zombies[i] = 0;
    }

    game->Mp = game->len - 3;
    game->Zp = game->len - 4;
    game->Cp[0] = game->len - 5;
    for (int i = 1; i < game->citizen_count; i++) {
        game->Cp[i] = rand() % (game->len - 6);
    }
    for (int i = 1; i < game->citizen_count; i++) {
        for (int j = 0; j < i; j++) {
            if (game->Cp[i] == game->Cp[j]) {
                game->Cp[i]++;
                j = -1;
            }
        }
    }

    if (game->current_stage == 2) {
        game->Vp = game->len - 6;
    }
}

// 시민 이동 함수
void moveCitizens(GameState* game) {
    for (int i = 0; i < game->citizen_count; i++) {
        if (game->Cp[i] != -1) {
            printf("시민%d: %d -> %d (어그로: %d)\n", i, game->Cp[i], game->Cp[i] - 1, game->aggro);
            game->Cp[i]--;
        }
        else {
            printf("시민%d은 아무것도 하지 않음.\n", i);
        }
        // 시민 탈출 확인
        if (game->Cp[i] == -1 || game->Cp[i] <= 0) {
            game->escape_success = 1;
            printf("시민%d 탈출 성공!\n", i);
            game->Cp[i] = -1;
        }
    }
}

// 빌런 이동 함수
void moveVillain(GameState* game) {
    if (game->current_stage == 2) {
        int move = 0;
        for (int i = 0; i < game->citizen_count; i++) {
            if (game->Cp[i] != -1) {
                move = game->Cp[i];
                break;
            }
        }
        if (move != 0) {
            printf("빌런: %d로 이동\n", move);
            game->Vp = move;
        }
        else {
            printf("빌런은 아무것도 하지 않음.\n");
        }
    }
}

// 좀비 이동 함수
void moveZombie(GameState* game) {
    if (game->zombie_pulled) {
        printf("좀비는 이번 턴에 이동할 수 없음.\n");
        game->zombie_pulled = 0;
    }
    else {
        printf("좀비: %d -> %d\n", game->Zp, game->Zp - 1);
        game->Zp--;
    }
}

// 마동석 행동 함수
void madongseokAction(GameState* game, int action) {
    if (action == MOVE_LEFT) {
        printf("마동석: %d에서 %d로 이동\n", game->Mp, game->Mp - 1);
        game->Mp--;
    }
    else {
        printf("마동석: %d에 머뭄\n", game->Mp);
    }
}

// 좀비 공격 함수
void zombieAttack(GameState* game) {
    int attacked = 0;
    if (rand() % 100 < game->p) {
        for (int i = 0; i < game->citizen_count; i++) {
            if (game->Cp[i] == game->Zp + 1 || game->Cp[i] == game->Zp - 1) {
                printf("좀비가 시민%d을 공격함.\n", i);
                game->citizen_count--;
                game->Cp[i] = -1; // 시민 사망
                attacked = 1;
                break;
            }
        }
        if (game->current_stage == 2 && game->Vp == game->Zp) {
            printf("좀비가 빌런을 공격함.\n");
            attacked = 1;
            // 빌런을 강화 좀비로 변환
            for (int i = 0; i < 10; i++) {
                if (game->enhanced_zombies[i] == 0) {
                    game->enhanced_zombies[i] = game->Vp;
                    break;
                }
            }
        }
    }
    if (!attacked) {
        printf("좀비가 아무도 공격하지 않음.\n");
    }
}

// 게임 상태 출력 함수
void printGameState(GameState game) {
    generateTrain(game);
    printf("마동석: %d (어그로: %d, 스테미나: %d)\n", game.Mp, game.aggro, game.stamina);
}

// 메인 함수
int main() {
    srand((unsigned int)time(NULL));

    GameState game;
    int action;

    // 학번 이름 출력
    numname();

    // 사용자 입력 받기
    int len, stamina, p;
    input(&len, &stamina, &p);

    // 게임 초기화
    game.len = len;
    game.stamina = stamina;
    game.p = p;
    game.current_stage = 1;

    initGame(&game);

    // 초기 상태 출력
    printGameState(game);

    // 게임 루프
    while (!game.game_over) {
        // 이동 페이즈
        moveCitizens(&game);
        moveVillain(&game);
        moveZombie(&game);

        // 마동석 이동 입력 및 처리
        printf("마동석 이동(0: 머뭄, 1: 왼쪽): ");
        scanf_s("%d", &action);
        madongseokAction(&game, action);
        printGameState(game);

        // 좀비 공격 처리
        zombieAttack(&game);

        // 마동석 행동 입력 및 처리
        printf("마동석 행동(0: 쉬기, 1: 도발, 2: 당기기)>> ");
        scanf_s("%d", &action);

        int prev_aggro = game.aggro;
        int prev_stamina = game.stamina;

        if (action == ACTION_REST) {
            game.stamina++;
            if (game.stamina > STM_MAX) game.stamina = STM_MAX;
            printf("마동석이 쉼\n");
        }
        else if (action == ACTION_PROVOKE) {
            if (game.stamina > 0) {
                game.stamina--;
                game.aggro++;
                printf("마동석이 도발함\n");
            }
            else {
                printf("도발하기에 스테미나 부족\n");
            }
        }
        else if (action == ACTION_PULL) {
            if (game.stamina > 0) {
                game.stamina--;
                game.aggro += 2;
                if (rand() % 100 >= game.p) { // (100 - p)% 성공 확률
                    game.zombie_pulled = 1;
                    printf("마동석이 좀비를 당김... 다음 턴에는 이동 불가\n");
                }
                else {
                    printf("마동석이 좀비 당기기 실패\n");
                }
            }
            else {
                printf("당기기에 스테미나 부족\n");
            }
        }

        printf("마동석: %d (어그로: %d -> %d, 스테미나: %d -> %d)\n",
            game.Mp, prev_aggro, game.aggro, prev_stamina, game.stamina);

        // 게임 상태 출력
        printGameState(game);

        // 게임 종료 조건 체크
        if (game.citizen_count == 0) {
            printf("게임 종료. 모든 시민이 사망함.\n");
            game.game_over = 1;
        }

        if (game.Zp <= 0) {
            printf("모든 좀비가 사망함. 승리!\n");
            game.game_over = 1;
        }

        if (game.escape_success) {
            printf("적어도 한 명의 시민이 탈출 성공. 승리!\n");
            game.game_over = 1;
        }

        // 다음 스테이지로 이동
        if (!game.game_over && game.current_stage < STAGES) {
            game.current_stage++;
            initGame(&game);
            printf("스테이지 %d로 진행 중...\n", game.current_stage);
        }
    }

    return 0;
}
