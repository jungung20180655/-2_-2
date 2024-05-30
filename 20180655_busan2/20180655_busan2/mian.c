#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// �Ķ���� ����
#define LEN_MIN 15 // ���� ���� �ּҰ�
#define LEN_MAX 50 // ���� ���� �ִ밪
#define STM_MIN 0  // ������ ���׹̳� �ּҰ�
#define STM_MAX 5  // ������ ���׹̳� �ִ밪
#define PROB_MIN 10 // Ȯ�� �ּҰ�
#define PROB_MAX 90 // Ȯ�� �ִ밪
#define AGGRO_MIN 0 // ��׷� �ּҰ�
#define AGGRO_MAX 5 // ��׷� �ִ밪
#define STAGES 3    // �������� ��

// �̵� �ൿ
#define MOVE_LEFT 1
#define MOVE_STAY 0

// ���� ���
#define ATK_NONE 0
#define ATK_CITIZEN 1
#define ATK_DONGSEOK 2

// �ൿ ����
#define ACTION_REST 0
#define ACTION_PROVOKE 1
#define ACTION_PULL 2

// ���� ����
#define STAGE2_VILLAIN 1

// �й��� �̸� ��� �Լ�
void numname() {
    printf("���α׷��� �ǽ� ����#2 - �λ���(2)\n");
    printf("�й�: 20180655 �̸�: ������\n");
}

// ���� ���� ����ü
typedef struct {
    int len;
    int stamina;
    int p;
    int Mp, Zp, Vp, Cp[10]; // ĳ���� ��ġ
    int citizen_count;
    int game_over;
    int aggro;
    int escape_success;
    int zombie_pulled;
    int current_stage;
    int enhanced_zombies[10];
} GameState;

// ���� ���� ��� �Լ�
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

// ����� �Է� �Լ�
void input(int* len, int* stamina, int* p) {
    printf("���� ����(%d~%d)>> ", LEN_MIN, LEN_MAX);
    scanf_s("%d", len);
    while (*len < LEN_MIN || *len > LEN_MAX) {
        printf("���� ����(%d~%d)>> ", LEN_MIN, LEN_MAX);
        scanf_s("%d", len);
    }
    printf("������ ���׹̳�(%d~%d)>> ", STM_MIN, STM_MAX);
    scanf_s("%d", stamina);
    while (*stamina < STM_MIN || *stamina > STM_MAX) {
        printf("������ ���׹̳�(%d~%d)>> ", STM_MIN, STM_MAX);
        scanf_s("%d", stamina);
    }
    printf("Ȯ�� 'p'(%d~%d)>> ", PROB_MIN, PROB_MAX);
    scanf_s("%d", p);
    printf("\n");
    while (*p < PROB_MIN || *p > PROB_MAX) {
        printf("�߸��� Ȯ���Դϴ�. %d�� %d ������ ���� �Է��ϼ���.\n", PROB_MIN, PROB_MAX);
        scanf_s("%d", p);
        printf("\n");
    }
}

// ���� �ʱ�ȭ �Լ�
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

// �ù� �̵� �Լ�
void moveCitizens(GameState* game) {
    for (int i = 0; i < game->citizen_count; i++) {
        if (game->Cp[i] != -1) {
            printf("�ù�%d: %d -> %d (��׷�: %d)\n", i, game->Cp[i], game->Cp[i] - 1, game->aggro);
            game->Cp[i]--;
        }
        else {
            printf("�ù�%d�� �ƹ��͵� ���� ����.\n", i);
        }
        // �ù� Ż�� Ȯ��
        if (game->Cp[i] == -1 || game->Cp[i] <= 0) {
            game->escape_success = 1;
            printf("�ù�%d Ż�� ����!\n", i);
            game->Cp[i] = -1;
        }
    }
}

// ���� �̵� �Լ�
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
            printf("����: %d�� �̵�\n", move);
            game->Vp = move;
        }
        else {
            printf("������ �ƹ��͵� ���� ����.\n");
        }
    }
}

// ���� �̵� �Լ�
void moveZombie(GameState* game) {
    if (game->zombie_pulled) {
        printf("����� �̹� �Ͽ� �̵��� �� ����.\n");
        game->zombie_pulled = 0;
    }
    else {
        printf("����: %d -> %d\n", game->Zp, game->Zp - 1);
        game->Zp--;
    }
}

// ������ �ൿ �Լ�
void madongseokAction(GameState* game, int action) {
    if (action == MOVE_LEFT) {
        printf("������: %d���� %d�� �̵�\n", game->Mp, game->Mp - 1);
        game->Mp--;
    }
    else {
        printf("������: %d�� �ӹ�\n", game->Mp);
    }
}

// ���� ���� �Լ�
void zombieAttack(GameState* game) {
    int attacked = 0;
    if (rand() % 100 < game->p) {
        for (int i = 0; i < game->citizen_count; i++) {
            if (game->Cp[i] == game->Zp + 1 || game->Cp[i] == game->Zp - 1) {
                printf("���� �ù�%d�� ������.\n", i);
                game->citizen_count--;
                game->Cp[i] = -1; // �ù� ���
                attacked = 1;
                break;
            }
        }
        if (game->current_stage == 2 && game->Vp == game->Zp) {
            printf("���� ������ ������.\n");
            attacked = 1;
            // ������ ��ȭ ����� ��ȯ
            for (int i = 0; i < 10; i++) {
                if (game->enhanced_zombies[i] == 0) {
                    game->enhanced_zombies[i] = game->Vp;
                    break;
                }
            }
        }
    }
    if (!attacked) {
        printf("���� �ƹ��� �������� ����.\n");
    }
}

// ���� ���� ��� �Լ�
void printGameState(GameState game) {
    generateTrain(game);
    printf("������: %d (��׷�: %d, ���׹̳�: %d)\n", game.Mp, game.aggro, game.stamina);
}

// ���� �Լ�
int main() {
    srand((unsigned int)time(NULL));

    GameState game;
    int action;

    // �й� �̸� ���
    numname();

    // ����� �Է� �ޱ�
    int len, stamina, p;
    input(&len, &stamina, &p);

    // ���� �ʱ�ȭ
    game.len = len;
    game.stamina = stamina;
    game.p = p;
    game.current_stage = 1;

    initGame(&game);

    // �ʱ� ���� ���
    printGameState(game);

    // ���� ����
    while (!game.game_over) {
        // �̵� ������
        moveCitizens(&game);
        moveVillain(&game);
        moveZombie(&game);

        // ������ �̵� �Է� �� ó��
        printf("������ �̵�(0: �ӹ�, 1: ����): ");
        scanf_s("%d", &action);
        madongseokAction(&game, action);
        printGameState(game);

        // ���� ���� ó��
        zombieAttack(&game);

        // ������ �ൿ �Է� �� ó��
        printf("������ �ൿ(0: ����, 1: ����, 2: ����)>> ");
        scanf_s("%d", &action);

        int prev_aggro = game.aggro;
        int prev_stamina = game.stamina;

        if (action == ACTION_REST) {
            game.stamina++;
            if (game.stamina > STM_MAX) game.stamina = STM_MAX;
            printf("�������� ��\n");
        }
        else if (action == ACTION_PROVOKE) {
            if (game.stamina > 0) {
                game.stamina--;
                game.aggro++;
                printf("�������� ������\n");
            }
            else {
                printf("�����ϱ⿡ ���׹̳� ����\n");
            }
        }
        else if (action == ACTION_PULL) {
            if (game.stamina > 0) {
                game.stamina--;
                game.aggro += 2;
                if (rand() % 100 >= game.p) { // (100 - p)% ���� Ȯ��
                    game.zombie_pulled = 1;
                    printf("�������� ���� ���... ���� �Ͽ��� �̵� �Ұ�\n");
                }
                else {
                    printf("�������� ���� ���� ����\n");
                }
            }
            else {
                printf("���⿡ ���׹̳� ����\n");
            }
        }

        printf("������: %d (��׷�: %d -> %d, ���׹̳�: %d -> %d)\n",
            game.Mp, prev_aggro, game.aggro, prev_stamina, game.stamina);

        // ���� ���� ���
        printGameState(game);

        // ���� ���� ���� üũ
        if (game.citizen_count == 0) {
            printf("���� ����. ��� �ù��� �����.\n");
            game.game_over = 1;
        }

        if (game.Zp <= 0) {
            printf("��� ���� �����. �¸�!\n");
            game.game_over = 1;
        }

        if (game.escape_success) {
            printf("��� �� ���� �ù��� Ż�� ����. �¸�!\n");
            game.game_over = 1;
        }

        // ���� ���������� �̵�
        if (!game.game_over && game.current_stage < STAGES) {
            game.current_stage++;
            initGame(&game);
            printf("�������� %d�� ���� ��...\n", game.current_stage);
        }
    }

    return 0;
}
