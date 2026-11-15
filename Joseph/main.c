/*
 * 实验一：约瑟夫环（带密码，动态报数上限 m）
 * 规则：
 *   1. n 个人编号 1..n 围成一圈，每人持有一个正整数密码；
 *   2. 初始报数上限为 m，从 1 号开始报 1、2、...，报到 m 的人出列；
 *   3. 出列者的密码成为新的 m，从其下一个人重新从 1 开始报数；
 *   4. 按出列顺序输出每个人的原编号。
 * 说明：如果所有密码都相同，本程序就等价于普通（固定 p）的约瑟夫环。
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* ==================== 1. 定义存储结构 ==================== */
typedef struct Node {
    int id;                 /* 原始编号 1..n */
    int password;           /* 该人出列后使用的新 m */
    struct Node *next;      /* 指向下一个结点 */
} Node;

/* ==================== 2. 创建结点 ==================== */
Node *createNode(int id, int password)
{
    Node *node = (Node *)malloc(sizeof(Node));
    if (node == NULL) {
        fprintf(stderr, "内存分配失败\n");
        exit(EXIT_FAILURE);
    }

    node->id = id;
    node->password = password;
    node->next = NULL;
    return node;
}

/* ==================== 3. 插入操作 ==================== */
/*
 * 在循环链表尾部插入一个新结点，返回新的尾结点。
 * tail == NULL 表示当前是空表。
 */
Node *insertNode(Node *tail, int id, int password)
{
    Node *newNode = createNode(id, password);

    if (tail == NULL) {             /* 第一个结点，自己指向自己 */
        newNode->next = newNode;
        return newNode;
    }

    newNode->next = tail->next;     /* 新结点插在尾结点之后 */
    tail->next = newNode;
    return newNode;                 /* 新结点成为新的尾结点 */
}

/* ==================== 4. 创建循环链表 ==================== */
/*
 * 按 1..n 的顺序建立单向循环链表，返回尾结点。
 * n == 0 时返回 NULL，表示空环。
 */
Node *createCircle(const int passwords[], int n)
{
    Node *tail = NULL;

    for (int i = 0; i < n; ++i) {
        tail = insertNode(tail, i + 1, passwords[i]);
    }

    return tail;
}

/* ==================== 5. 查找操作 ==================== */
/*
 * 从 start 开始向后走 steps 步，返回目标结点。
 * 通过 prev 带回目标结点的前驱，便于执行删除。
 */
Node *findNode(Node *start, int steps, Node **prev)
{
    Node *cur = start;

    for (int i = 0; i < steps; ++i) {
        *prev = cur;
        cur = cur->next;
    }

    return cur;
}

/* ==================== 6. 删除操作 ==================== */
/*
 * 从循环链表中删除 cur，返回 cur 的后继。
 * 如果删除后链表为空，返回 NULL。
 */
Node *deleteNode(Node *prev, Node *cur)
{
    if (cur == NULL) {
        return NULL;
    }

    /* 只剩一个结点：直接释放 */
    if (cur->next == cur) {
        free(cur);
        return NULL;
    }

    Node *next = cur->next;
    prev->next = next;
    free(cur);
    return next;
}

/* ==================== 7. 求解约瑟夫环 ==================== */
void josephus(Node *tail, int n, int m)
{
    if (tail == NULL) {             /* n == 0，无人出列 */
        printf("\n");
        return;
    }

    Node *prev = tail;              /* 1 号结点的前驱是尾结点 */
    Node *cur = tail->next;         /* 从 1 号开始报数 */
    int remaining = n;

    while (remaining > 0) {
        /*
         * 当前 cur 已经报“1”，所以只需再走 m-1 步。
         * 先对 remaining 取模，即使 m 远大于 n 也不会做无用循环。
         */
        int steps = (m - 1) % remaining;
        cur = findNode(cur, steps, &prev);

        printf("%d", cur->id);
        if (remaining > 1) {
            printf(" ");
        }

        m = cur->password;          /* 动态更新报数上限 */
        cur = deleteNode(prev, cur);/* 出列，cur 指向下一个人 */

        --remaining;
    }

    printf("\n");
}

/* ==================== 8. 主函数 ==================== */
int main(void)
{
    int n, m;

    printf("请输入总人数 n 和初始报数上限 m：\n");
    if (scanf("%d %d", &n, &m) != 2) {
        fprintf(stderr, "输入格式错误\n");
        return EXIT_FAILURE;
    }

    if (n < 0) {
        fprintf(stderr, "人数 n 不能为负数\n");
        return EXIT_FAILURE;
    }

    /* 边界：n == 0，空环，给出明确提示后结束 */
    if (n == 0) {
        printf("人数为 0，无人出列。\n");
        return EXIT_SUCCESS;
    }

    if (m <= 0) {
        fprintf(stderr, "初始报数上限 m 必须为正整数\n");
        return EXIT_FAILURE;
    }

    /* 防止 n 过大导致 n * sizeof(int) 溢出 */
    if ((size_t)n > SIZE_MAX / sizeof(int)) {
        fprintf(stderr, "人数过大，无法分配内存\n");
        return EXIT_FAILURE;
    }

    int *passwords = (int *)malloc((size_t)n * sizeof(int));
    if (passwords == NULL) {
        fprintf(stderr, "内存分配失败\n");
        return EXIT_FAILURE;
    }

    printf("请依次输入 %d 个密码（均为正整数）：\n", n);
    for (int i = 0; i < n; ++i) {
        if (scanf("%d", &passwords[i]) != 1) {
            fprintf(stderr, "第 %d 个密码输入格式错误\n", i + 1);
            free(passwords);
            return EXIT_FAILURE;
        }
        if (passwords[i] <= 0) {
            fprintf(stderr, "第 %d 个密码必须为正整数\n", i + 1);
            free(passwords);
            return EXIT_FAILURE;
        }
    }

    Node *tail = createCircle(passwords, n);
    free(passwords);

    printf("出列顺序：");
    josephus(tail, n, m);

    return EXIT_SUCCESS;
}
