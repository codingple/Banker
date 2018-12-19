#include <stdio.h>

//state 구조
typedef struct {
	int resource[4]; //전체 자원 개수
	int available[4]; //사용 가능한 자원 개수
	int claim[6][4]; //프로세스를 끝낼 수 있는 최대 자원 수
	int allocation[6][4]; //사용 가능한 자원 개수
}state;

//suspend queue 구조
typedef struct {
	int queue[10];
	int start;
	int end;
}suspend_queue;

//request 구조
typedef struct {
	int process;
	int need[4];
}requestType;

//finish queue 구조
typedef struct {
	int length;
	int list[6];
}finish_queue;




/* suspend queue에 해당 프로세스를 넣는 함수 */
void insert_suspend ( suspend_queue* suspend, int* k){
	int index = suspend->end;
	suspend->queue[index] = *k;
	suspend->end++;
}

/* suspend queue에서 프로세스를 모두 빼는 함수 */
void pop_suspend ( suspend_queue* suspend ){
	if ( suspend->start != suspend->end )
		suspend->start = suspend->end;
}



/* suspend queue에 해당 프로세스가 있는지 검사하는 함수 */
int search_suspend ( suspend_queue* suspend, int* k ){
	int s = suspend->start;
	int e = suspend->end;
	while ( s < e ){
		if ( *k == suspend->queue[s%10] )
			return 1;
		s++;
	}
	return 0;
}

/* finish queue에 해당 프로세스를 넣는 함수 */
void insert_finish ( finish_queue* finish, int* k){
	int index = finish->length;
	finish->list[index] = *k;
	finish->length++;
}



/* finish queue에 해당 프로세스가 있는지 검사하는 함수 */
int search_finish ( finish_queue* finish, int* k ){
	int index = finish->length;
	int i;
	for(i=0; i<index; i++){
		if ( *k == finish->list[i] )
			return 1;
	}
	return 0;
}


/* 상태 출력 함수 */
void display_state( state *current_state, suspend_queue* suspend, finish_queue* finish){
	//출력에 필요한 변수 초기화
	int i, j;
	int s = suspend->start;
	int e = suspend->end;
	int count = 0;

	printf("---------------현재상태---------------\n");

	//Resource
	printf("|  +Resource+                        |\n");
	printf("|    A    B    C    D                |\n");
	printf("|    ");
	for(i=0; i<4; i++){
		int num = current_state->resource[i];
		if ( num < 10 )
			printf("%d    ", num);
		else
			printf("%d   ", num);
	}
	printf("            |\n");
	printf("--------------------------------------\n");

	//Available
	printf("|  +Available+                       |\n");
	printf("|    A    B    C    D                |\n");
	printf("|    ");
	for(i=0; i<4; i++){
		int num = current_state->available[i];
		if ( num < 10 )
			printf("%d    ", num);
		else
			printf("%d   ", num);
	}
	printf("            |\n");
	printf("--------------------------------------\n");

	//claim
	printf("|  +claim+                           |\n");
	printf("|    A    B    C    D                |\n");
	for (j=0; j<6; j++){
		if ( search_finish( finish, &j ) )
			continue;
		else{
			printf("|P%d  ", j);
			for(i=0; i<4; i++){
				int num = current_state->claim[j][i];
				if ( num < 10 )
					printf("%d    ", num);
				else
					printf("%d   ", num);
			}
			printf("            |\n");
		}
	}
	printf("--------------------------------------\n");

	//allocation
	printf("|  +allocation+                      |\n");
	printf("|    A    B    C    D                |\n");
	for (j=0; j<6; j++){
		if ( search_finish( finish, &j ) )
			continue;
		else{
			printf("|P%d  ", j);
			for(i=0; i<4; i++){
				int num = current_state->allocation[j][i];
				if ( num < 10 )
					printf("%d    ", num);
				else
					printf("%d   ", num);
			}
			printf("            |\n");
		}
	}
	printf("--------------------------------------\n");

	if ( s != e ){
		//suspend queue
		printf("|  +suspend list+                    |\n");
		printf("|  ");
		while ( s < e ){
			printf("P%d  ", suspend->queue[s%10]);
			s++;
			count += 4;
		}
		i = 0;
		while ( i < (34 - count) ){
			printf(" ");
			i++;
		}
		printf("|\n");
		printf("--------------------------------------\n");
	}

}


/* safe 함수 */
int safe (state* s, finish_queue* finish){
	int currentavail[4];
	int process_rest[6];
	int i;
	int count = 0;
	int rest_count = 0;
	int possible = 1;
	int satisfy;

	//currentavail 초기화
	for(i=0; i<4; i++){
		currentavail[i] = s->available[i];
	}

	//process_rest 초기화
	for(i=0; i< 6; i++){
		if( search_finish(finish, &i) )
			continue;
		else{
			process_rest[rest_count] = i;
			rest_count++;
		}
	}

	//process를 탐색하여 sequence를 만들 수 있는지 시도
	count = rest_count;
	while ( possible ){

		//현재 탐색 위치를 표시하는 count가 0이 되면 끝
		if( count == 0 )
			possible = 0;

		else{
			int process = process_rest[count - 1];
			satisfy = 1;
			//claim - allocation > currentavail이면, satisfy = 0
			for(i=0; i<4; i++){
				if ( s->claim[process][i] - s->allocation[process][i] > currentavail[i] )
					satisfy = 0;
			}

			//프로세스를 끝낼 수 있으면 process_rest에서 빼고 rest_count감소
			if( satisfy == 1 ){
				for(i=0; i<4; i++)
					currentavail[i] += s->allocation[process][i];
				process_rest[count - 1] = process_rest[rest_count - 1];
				rest_count--;
				count = rest_count;
			}

			//프로세스를 끝낼 수 없으면 탐색 위치만 앞으로 바꿈
			else
				count--;
		}
	}

	if ( rest_count == 0 )
		return 1;
	else
		return 0;


}




int main (void){

	/* 현재상태 초기화 */
	state current_state = {
		//resource
		{ 15, 6, 9, 10 },
		//available
		{ 6, 3, 5, 4 },
		//claim
		{
			{ 9, 5, 5, 5 },
			{ 2, 2, 3, 3 },
			{ 7, 5, 4, 4 },
			{ 3, 3, 3, 2 },
			{ 5, 2, 2, 1 },
			{ 4, 4, 4, 4 }
		},
			//allocation
		{
			{ 2, 0, 2, 1 },
			{ 0, 1, 1, 1 },
			{ 4, 1, 0, 2 },
			{ 1, 0, 0, 1 },
			{ 1, 1, 0, 0 },
			{ 1, 0, 1, 1 }
		}
	};

	/* new state 초기화 */
	state new_state;

	/* suspend queue 초기화 */
	suspend_queue suspend = {
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		0, 0
	};

	/* finish queue 초기화 */
	finish_queue finish = {
		0,
		{ 0, 0, 0, 0, 0, 0 }
	};

	/* request 초기화 */
	requestType request;

	/* 프로그램 종료 변수 */
	int shutdown = 1;
	int i;
	int check;

	while ( shutdown ){
		/* current_state출력 및 request입력 */
		display_state(&current_state, &suspend, &finish);
		while (1){
			printf("[알림]request를 입력하시오(순서 : 프로세스명, A, B, C, D)\n: ");
			scanf("%d %d %d %d %d", &request.process, &request.need[0], &request.need[1], &request.need[2], &request.need[3]);
			printf("\n\n");

			//프로세스 숫자 범위
			if ( 0 <= request.process && request.process <= 5 ){
				//suspend 여부
				if ( !search_suspend ( &suspend, &request.process ) ){
					//finish 여부
					if ( !search_finish(&finish, &request.process) )
						break;
					else{
						printf("[에러]이미 끝난 프로세스를 입력했습니다.\n");
						fflush(stdin);
						continue;
					}
				}

				else{
					printf("[에러]P%d는 suspend된 프로세스입니다.\n", request.process);
					fflush(stdin);
					continue;
				}
			}
			else{
				printf("[에러]올바른 process를 입력해주세요.\n");
				fflush(stdin);
				continue;
			}
		}

		/* resource alloc algorithm */
		check = 0;

		//total request > claim인지 검사
		for(i=0; i<4; i++){
			if( current_state.allocation[request.process][i] + request.need[i]
		> current_state.claim[request.process][i])
			check = 1;
		}
		if ( check ){
			printf("[에러]allocation + request > claim\n");
			continue;
		}

		//request > available인지 검사
		for(i=0; i<4; i++){
			if( request.need[i] > current_state.available[i]){
				insert_suspend(&suspend, &request.process);
				check = 1;
			}
		}
		if ( check ){
			printf("[알림]request > availalbe 이므로 suspend합니다.\n");
			continue;
		}

		//new_state 정의
		new_state = current_state;
		for(i=0; i<4; i++){
			new_state.allocation[request.process][i] += request.need[i];
			new_state.available[i] -= request.need[i];
		}

		//safe인지 확인하여 처리
		check = 0;
		if( safe( &new_state, &finish ) ){
			//safe면 new_state를 current_state로 변환
			current_state = new_state;
			//자원을 할당한 프로세스가 끝났는지 확인
			for(i=0; i<4; i++){
				if( current_state.claim[request.process][i] != current_state.allocation[request.process][i] )
					check = 1;
			}

			//프로세스가 끝났으면 모든 프로세스가 끝났는지 확인
			if( check == 0 ){
				insert_finish( &finish, &(request.process) );
				//끝난 프로세스 자원을 돌려줌
				for(i=0; i<4; i++)
					current_state.available[i] += current_state.allocation[request.process][i];
				printf("[알림]프로세스 P%d가 끝났습니다.\n", request.process);
				if( finish.length == 6 )
					shutdown = 0;
			}
			pop_suspend( &suspend );
		}

		else{
			insert_suspend( &suspend, &(request.process) );
			printf("[알림]프로세스 P%d가 unsafe하여 suspend합니다.\n", request.process);
		}
	}

	printf("[알림]프로세스가 모두 끝났습니다. 프로그램을 종료합니다.\n");
	return 0;
}
