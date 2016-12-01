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

static void printTable();
static void addConn(int, int, int, bool);
static void initTable();
static void initInput();
static int  dvUpdate();

int
main(int argc, char *argv[]) {
    int is_converge, dvruntime = 1;

    /* Exp 1 */
    initTable();
    initInput();
    // printTable();
    printf("Running Distance-Vector Updating:\nRun %d:\n", dvruntime);
    do {
        is_converge = dvUpdate();
        printTable();
        dvruntime++;
        printf("Run %d:", dvruntime);
    }while(is_converge == 0);
    printf("DV Converged.\n");

    /* Exp 2 */
    addConn(0, 1, 3, 1);
    dvruntime = 1;
    printf("Distance from A to B changed to 3. Rerun DV:");
    do {
        is_converge = dvUpdate();
        printTable();
        dvruntime++;
        printf("Run %d:", dvruntime);
    }while(is_converge == 0);
    printf("DV Converged.\n");

    return 0;
}

int
dvUpdate() {
    /* Use DV algorithm to update router info */
    int src, trg, rec, convergeFlag = 1;
    for (src = 0; src < rtnum; src++) {
        for (trg = 0; trg < rtnum; trg++) {
            if (rttable[src][trg].isAdj == true) {
                for (rec = 0; rec < rtnum; rec++) {
                    if((rttable[src][rec].dist <= _MAX_NOD_DST_ && rttable[src][rec].next == trg) || (rttable[src][rec].dist > (rttable[src][trg].real + rttable[trg][rec].dist))) {
                        rttable[src][rec].next = rttable[src][trg].next;
                        if(rttable[src][rec].dist != rttable[src][trg].real + rttable[trg][rec].dist) {
                            rttable[src][rec].dist = rttable[src][trg].real + rttable[trg][rec].dist;
                            convergeFlag = 0;
                        }       
                    }
                }
            }
        }
    }
    return convergeFlag;
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
