#include <stdio.h>

//state ����
typedef struct {
	int resource[4]; //��ü �ڿ� ����
	int available[4]; //��� ������ �ڿ� ����
	int claim[6][4]; //���μ����� ���� �� �ִ� �ִ� �ڿ� ��
	int allocation[6][4]; //��� ������ �ڿ� ����
}state;

//suspend queue ����
typedef struct {
	int queue[10];
	int start;
	int end;
}suspend_queue;

//request ����
typedef struct {
	int process;
	int need[4];
}requestType;

//finish queue ����
typedef struct {
	int length;
	int list[6];
}finish_queue;




/* suspend queue�� �ش� ���μ����� �ִ� �Լ� */
void insert_suspend ( suspend_queue* suspend, int* k){
	int index = suspend->end;
	suspend->queue[index] = *k;
	suspend->end++;
}

/* suspend queue���� ���μ����� ��� ���� �Լ� */
void pop_suspend ( suspend_queue* suspend ){
	if ( suspend->start != suspend->end )
		suspend->start = suspend->end;
}



/* suspend queue�� �ش� ���μ����� �ִ��� �˻��ϴ� �Լ� */
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

/* finish queue�� �ش� ���μ����� �ִ� �Լ� */
void insert_finish ( finish_queue* finish, int* k){
	int index = finish->length;
	finish->list[index] = *k;
	finish->length++;
}



/* finish queue�� �ش� ���μ����� �ִ��� �˻��ϴ� �Լ� */
int search_finish ( finish_queue* finish, int* k ){
	int index = finish->length;
	int i;
	for(i=0; i<index; i++){
		if ( *k == finish->list[i] )
			return 1;
	}
	return 0;
}


/* ���� ��� �Լ� */
void display_state( state *current_state, suspend_queue* suspend, finish_queue* finish){
	//��¿� �ʿ��� ���� �ʱ�ȭ
	int i, j;
	int s = suspend->start;
	int e = suspend->end;
	int count = 0;

	printf("---------------�������---------------\n");

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


/* safe �Լ� */
int safe (state* s, finish_queue* finish){
	int currentavail[4];
	int process_rest[6];
	int i;
	int count = 0;
	int rest_count = 0;
	int possible = 1;
	int satisfy;

	//currentavail �ʱ�ȭ
	for(i=0; i<4; i++){
		currentavail[i] = s->available[i];
	}

	//process_rest �ʱ�ȭ
	for(i=0; i< 6; i++){
		if( search_finish(finish, &i) )
			continue;
		else{
			process_rest[rest_count] = i;
			rest_count++;
		}
	}

	//process�� Ž���Ͽ� sequence�� ���� �� �ִ��� �õ�
	count = rest_count;
	while ( possible ){

		//���� Ž�� ��ġ�� ǥ���ϴ� count�� 0�� �Ǹ� ��
		if( count == 0 )
			possible = 0;

		else{
			int process = process_rest[count - 1];
			satisfy = 1;
			//claim - allocation > currentavail�̸�, satisfy = 0
			for(i=0; i<4; i++){
				if ( s->claim[process][i] - s->allocation[process][i] > currentavail[i] )
					satisfy = 0;
			}

			//���μ����� ���� �� ������ process_rest���� ���� rest_count����
			if( satisfy == 1 ){
				for(i=0; i<4; i++)
					currentavail[i] += s->allocation[process][i];
				process_rest[count - 1] = process_rest[rest_count - 1];
				rest_count--;
				count = rest_count;
			}

			//���μ����� ���� �� ������ Ž�� ��ġ�� ������ �ٲ�
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

	/* ������� �ʱ�ȭ */
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

	/* new state �ʱ�ȭ */
	state new_state;

	/* suspend queue �ʱ�ȭ */
	suspend_queue suspend = {
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		0, 0
	};

	/* finish queue �ʱ�ȭ */
	finish_queue finish = {
		0,
		{ 0, 0, 0, 0, 0, 0 }
	};

	/* request �ʱ�ȭ */
	requestType request;

	/* ���α׷� ���� ���� */
	int shutdown = 1;
	int i;
	int check;

	while ( shutdown ){
		/* current_state��� �� request�Է� */
		display_state(&current_state, &suspend, &finish);
		while (1){
			printf("[�˸�]request�� �Է��Ͻÿ�(���� : ���μ�����, A, B, C, D)\n: ");
			scanf("%d %d %d %d %d", &request.process, &request.need[0], &request.need[1], &request.need[2], &request.need[3]);
			printf("\n\n");

			//���μ��� ���� ����
			if ( 0 <= request.process && request.process <= 5 ){
				//suspend ����
				if ( !search_suspend ( &suspend, &request.process ) ){
					//finish ����
					if ( !search_finish(&finish, &request.process) )
						break;
					else{
						printf("[����]�̹� ���� ���μ����� �Է��߽��ϴ�.\n");
						fflush(stdin);
						continue;
					}
				}

				else{
					printf("[����]P%d�� suspend�� ���μ����Դϴ�.\n", request.process);
					fflush(stdin);
					continue;
				}
			}
			else{
				printf("[����]�ùٸ� process�� �Է����ּ���.\n");
				fflush(stdin);
				continue;
			}
		}

		/* resource alloc algorithm */
		check = 0;

		//total request > claim���� �˻�
		for(i=0; i<4; i++){
			if( current_state.allocation[request.process][i] + request.need[i]
		> current_state.claim[request.process][i])
			check = 1;
		}
		if ( check ){
			printf("[����]allocation + request > claim\n");
			continue;
		}

		//request > available���� �˻�
		for(i=0; i<4; i++){
			if( request.need[i] > current_state.available[i]){
				insert_suspend(&suspend, &request.process);
				check = 1;
			}
		}
		if ( check ){
			printf("[�˸�]request > availalbe �̹Ƿ� suspend�մϴ�.\n");
			continue;
		}

		//new_state ����
		new_state = current_state;
		for(i=0; i<4; i++){
			new_state.allocation[request.process][i] += request.need[i];
			new_state.available[i] -= request.need[i];
		}

		//safe���� Ȯ���Ͽ� ó��
		check = 0;
		if( safe( &new_state, &finish ) ){
			//safe�� new_state�� current_state�� ��ȯ
			current_state = new_state;
			//�ڿ��� �Ҵ��� ���μ����� �������� Ȯ��
			for(i=0; i<4; i++){
				if( current_state.claim[request.process][i] != current_state.allocation[request.process][i] )
					check = 1;
			}

			//���μ����� �������� ��� ���μ����� �������� Ȯ��
			if( check == 0 ){
				insert_finish( &finish, &(request.process) );
				//���� ���μ��� �ڿ��� ������
				for(i=0; i<4; i++)
					current_state.available[i] += current_state.allocation[request.process][i];
				printf("[�˸�]���μ��� P%d�� �������ϴ�.\n", request.process);
				if( finish.length == 6 )
					shutdown = 0;
			}
			pop_suspend( &suspend );
		}

		else{
			insert_suspend( &suspend, &(request.process) );
			printf("[�˸�]���μ��� P%d�� unsafe�Ͽ� suspend�մϴ�.\n", request.process);
		}
	}

	printf("[�˸�]���μ����� ��� �������ϴ�. ���α׷��� �����մϴ�.\n");
	return 0;
}
