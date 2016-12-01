#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define _POS_INF_     0x3f3f3f3f
#define _MAX_NOD_NUM_    6
#define _MAX_NOD_DST_    15

typedef struct nod_info {
    char    next;   // Next hop destination
    int     real;   // Real distance for connected routers
    int     dist;   // Minimal distance
    bool    isAdj;  // Adjacent flag
} rtnode;

static int       rtnum = 5; // Number of routers
static rtnode    rttable[_MAX_NOD_NUM_][_MAX_NOD_NUM_]; // Router table
// static rtnode    nwtable[_MAX_NOD_NUM_][_MAX_NOD_NUM_];

static void printTable();
static void addConn(int, int, int, bool);
static void delConn(int, int);
static void initTable();
static void initInput();
static int  dvUpdate();
// static void dvDuplic(rtnode *, rtnode *);

int
main(int argc, char *argv[]) {
    int is_converge, dvruntime = 1;

    /* Exp 1 */
    initTable();
    initInput();
    printTable();
    printf("Running Distance-Vector Updating:\nRun %d:\n", dvruntime);
    do {
        is_converge = dvUpdate();
        printTable();
        dvruntime++;
        printf("Run %d:\n", dvruntime);
    }while(is_converge == 0);
    printf("DV Converged.\n");

    /* Exp 2 */
    // addConn(0, 1, 3, 1);
    // dvruntime = 1;
    // printf("Distance from A to B changed to 3. Rerun DV:\n");
    // do {
    //     is_converge = dvUpdate();
    //     printTable();
    //     dvruntime++;
    //     printf("Run %d:\n", dvruntime);
    // }while(is_converge == 0);
    // printf("DV Converged.\n");

    /* Exp 3 - PROBLEM */
    // delConn(1, 2);
    // printTable();
    // dvruntime = 1;
    // printf("Connection between B and C closed. Rerun DV:\n");
    // do {
    //     is_converge = dvUpdate();
    //     printTable();
    //     dvruntime++;
    //     printf("Run %d:\n", dvruntime);
    // }while(is_converge == 0);
    // printf("DV Converged.\n");

    return 0;
}

int
dvUpdate() {
    /* Use DV algorithm to update router info */
    int src, trg, rec, convergeFlag = 1;
    //TODO: Change rttable later, not immediately.
    for (src = 0; src < rtnum; src++) {
        for (trg = 0; trg < rtnum; trg++) {
            if (rttable[src][trg].isAdj == true) {
                for (rec = 0; rec < rtnum; rec++) {
                    if(rttable[src][rec].dist > (rttable[src][trg].dist + rttable[trg][rec].dist)) {
                        rttable[src][rec].next = rttable[src][trg].next;
                        rttable[src][rec].dist = rttable[src][trg].dist + rttable[trg][rec].dist;
                        convergeFlag = 0;
                    }
                }
            }
        }
    }
    return convergeFlag;
}

void
delConn(int dnod, int nnod) {
    /* Delete connection between two routers, update info */
    rttable[dnod][nnod].dist = _POS_INF_;
    rttable[dnod][nnod].real = _POS_INF_;
    rttable[dnod][nnod].next = dnod;
    rttable[dnod][nnod].isAdj = false;
    rttable[nnod][dnod].dist = _POS_INF_;
    rttable[nnod][dnod].real = _POS_INF_;
    rttable[nnod][dnod].next = nnod;
    rttable[nnod][dnod].isAdj = false;
}

void
addConn(int dnod, int nnod, int dist, bool adj) {
    /* Add connection between two routers, update info */

    if (dist < 0) {
        /* Given negative distance, exit */
        printf("[ERROR] negative dist");
        exit(1);
    }
    rttable[dnod][nnod].dist = dist;
    rttable[dnod][nnod].real = dist;
    rttable[dnod][nnod].next = nnod;
    rttable[dnod][nnod].isAdj = adj;
    rttable[nnod][dnod].dist = dist;
    rttable[nnod][dnod].real = dist;
    rttable[nnod][dnod].next = dnod;
    rttable[nnod][dnod].isAdj = adj;
}

// void
// dvBackup() {
//     int src, rec;
//     for (src = 0; src < rtnum; src++) {
//         for (rec = 0; rec < rtnum; rec++) {
//             nwtable[src][rec].dist = rttable[src][rec].dist;
//             nwtable[src][rec].isAdj = rttable[src][rec].isAdj;
//             nwtable[src][rec].next = rttable[src][rec].next;
//             }
//         }
// }

// void
// dvDuplic(rtnode *src, rtnode *trg) {
//     int i;
//     for (i = 0; i < rtnum * rtnum; i++) {
//         (trg+i)->dist = (src+i)->dist;
//         (trg+i)->real = (src+i)->real;
//         (trg+i)->isAdj = (src+i)->isAdj;
//         (trg+i)->next = (src+i)->next;
//     }
// }

void
initInput() {
    /* Construct initial router table from input */
    char from, to;
    int dist;

    // Read router topology info
    printf("Input router info: (Format like \"AE8\", enter 0 to end input)\n");
    scanf("%c", &from);
    // Deal with input
    while(from != '0') {
        scanf("%c%d", &to, &dist);
        from = toupper(from);
        to = toupper(to);
        from -= 'A';
        to -= 'A';
        if (from >= rtnum || to >= rtnum || from < 0 || to < 0) {
            printf("[ERROR] wrong router input\n");
            exit(1);
        } else {
            addConn(from, to, dist, true);
        }
        scanf("%c", &from);
        while(from == '\n') {
            scanf("%c", &from);
        }
    }
}

void
initTable() {
    int fr, to;
    /* Initialize router table */
    for(fr = 0; fr < rtnum; fr++) {
        for(to = 0; to < rtnum; to++) {
            if(fr == to) {
                // Connection to self
                addConn(fr, to, 0, false);
            } else {
                // Connection to other routers
                addConn(fr, to, _POS_INF_, false);
            }
        }
    }
}

void
printTable() {
    /* Print router table */
    int i, nodenum;

    // Print table for each router
    for(nodenum = 0; nodenum < rtnum; nodenum ++) {
        // Print header
        printf("\n       Router %c\n", nodenum + 'A');
        printf("Desc    Next    Dist\n");
        // Print router info
        for(i = 0; i < rtnum; i++) {
            printf(" %c       %c       ", i + 'A', rttable[nodenum][i].next + 'A');
            if(rttable[nodenum][i].dist < _MAX_NOD_DST_) {
                printf("%d\n", rttable[nodenum][i].dist);
            } else {
                // Print - for distance larger than _MAX_NOD_DST_ 
                printf("-\n");
            }
        }
    }        
}
