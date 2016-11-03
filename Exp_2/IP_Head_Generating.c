#include <stdio.h>
#include <stdlib.h>

static int  frag[4];
static unsigned char iphead4b[5][8] = {{4, 5, 0, 0, 0, 0, 1, 4}, // Init fixed bits of version/head length/total length
						{3, 8, 3, 1}, {15, 15, 0, 1}}; // Init fixed bits of identification/TTL/protocol number

void printIP8b(void);
void fillIPFrag(int *, int);
void calcChecksum(void);

int
main(int argc, char *argv[])
{
	printf("Input source IP(xxx.xxx.xxx.xxx): ");
	scanf("%d.%d.%d.%d", &frag[0], &frag[1], &frag[2], &frag[3]);
	fillIPFrag(frag, 0);
	printf("Input destination IP(xxx.xxx.xxx.xxx): ");
	scanf("%d.%d.%d.%d", &frag[0], &frag[1], &frag[2], &frag[3]);
	fillIPFrag(frag, 1);

	calcChecksum();
	printIP8b();

	return 0;
}

void
printIP8b(void)
{
	char p, c, i, j;

	for(i = 0; i < 5; i++){
		for(j = 0; j < 4; j++){
			for(p = 0; p < 2; p++){
				c = iphead4b[i][j * 2 + p];
				if(c < 10 && c >= 0){
					printf("%c", c + '0');
				}else if(c < 16){
					printf("%c", c - 10 + 'A');
				}else{
					printf("\nwrong header value '%d' at [%d][%d]\n", c, i ,j);
					exit(1);
				}
			}
			printf(" ");
		}
		printf("\n");
	}
}

void
fillIPFrag(int* frag, int mode)
{
	int i,j;
	//Locating row to write in
	if(mode != 0){
		mode = 4;
	}else{
		mode = 3;
	}

	//Write in IP Section
	for(i = 0;i < 4; i++){
		j = *(frag+i);

		*(frag+i) = 900; //Set value as PROCESSED for illegal input check
		if(j > 255 || j < 0){
			printf("illegal input\n");
			exit(1);
		}

		iphead4b[mode][i * 2] = j / 16;
		iphead4b[mode][i * 2 + 1]  = j % 16;	
	}
}

void
calcChecksum(void)
{
	int i, j, sum[4] = {0};
	bool flag;
	printIP8b();
	for(i = 0; i < 5; ++i){
		for (j = 0; j < 4; ++j)
		{
			sum[j] += iphead4b[i][j] + iphead4b[i][j + 4];
		}
	}

	while(1){
		flag = true;
		for(i = 3; i >= 0; --i){
			if(sum[i] > 15){
				flag = false;
				if((j = i - 1) < 0){
					j = 3;
				}
				sum[j] += sum[i] / 16;
				sum[i] %= 16;
			}
		}
		if(flag){
			for(i = 0; i < 4; ++i){
				iphead4b[2][i + 4] = 15 - sum[i];
			}
			break;
		}
	}
}
