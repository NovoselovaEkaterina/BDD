#pragma comment(lib, "bdd.lib")
#include "bdd.h"
#include <fstream>

using namespace std;

#define N 9 // число объектов
#define M 4 // число свойств
#define LOG_N 4
#define N_VAR N*M*LOG_N // число булевых переменных(16*9*4)

ofstream out;
void fun(char* varset, int size); // функция, используемая для вывода решений

void main(void)
{
	// инициализация
	bdd_init(10000000, 1000000);
	bdd_setvarnum(N_VAR);
	// ->--- вводим функцию p(k, i, j) следующим образом ( кодируем pk[i][j]):
	bdd p1[N][N];
	bdd p2[N][N];
	bdd p3[N][N];
	bdd p4[N][N];
	unsigned I = 0;
	for (unsigned i = 0; i < N; i++)
	{
		for (unsigned j = 0; j < N; j++)
		{
			p1[i][j] = bddtrue;
			for (unsigned k = 0; k < LOG_N; k++) p1[i][j] &= ((j >> k) &
				1) ? bdd_ithvar(I + k) : bdd_nithvar(I + k);
			p2[i][j] = bddtrue;
			for (unsigned k = 0; k < LOG_N; k++) p2[i][j] &= ((j >> k) &
				1) ? bdd_ithvar(I + LOG_N + k) : bdd_nithvar(I + LOG_N + k);
			p3[i][j] = bddtrue;
			for (unsigned k = 0; k < LOG_N; k++) p3[i][j] &= ((j >> k) &
				1) ? bdd_ithvar(I + LOG_N * 2 + k) : bdd_nithvar(I + LOG_N * 2 + k);
			p4[i][j] = bddtrue;
			for (unsigned k = 0; k < LOG_N; k++) p4[i][j] &= ((j >> k) &
				1) ? bdd_ithvar(I + LOG_N * 3 + k) : bdd_nithvar(I + LOG_N * 3 + k);

		}
		I += LOG_N * M;
	}
	// здесь должны быть ограничения
	bdd task = bddtrue;

	//переменные принимают значения только до N
	for (unsigned i = 0; i < N; i++)
	{
		bdd temp1 = bddfalse;
		bdd temp2 = bddfalse;
		bdd temp3 = bddfalse;
		bdd temp4 = bddfalse;
		for (unsigned j = 0; j < N; j++)
		{
			temp1 |= p1[i][j];
			temp2 |= p2[i][j];
			temp3 |= p3[i][j];
			temp4 |= p4[i][j];
		}
		task &= temp1 & temp2 & temp3 & temp4;
	}

	//ограничение типа n7
	for (int i = 1; i < N; i += 2)
		for (int j = 0; j < 9; j++)
			for (int k = 0; k < 9; k++)
				for (int x = 0; x < 9; x++)
					for (int y = 0; y < 9; y++)
						if (j + k + x + y > 15)
							task &= !(p1[i][j] & p2[i][k] & p3[i][x] & p4[i][y]);



	//ограничения типа 1
	task &= p1[0][0];
	task &= p2[3][3];
	task &= p3[1][7];
	task &= p4[4][8];
	task &= p1[8][4];

	//дополнительные ограничения
	task &= p1[3][6];
	task &= p3[8][2];
	task &= p1[2][3];
	task &= p3[0][8];

	//ограничения типа 2
	for (unsigned i = 0; i < N; i++) {
		task &= !(p1[i][3] ^ p2[i][5]);
		task &= !(p1[i][7] ^ p4[i][3]);
		task &= !(p3[i][1] ^ p2[i][0]);
		task &= !(p2[i][6] ^ p3[i][2]);
		task &= !(p3[i][7] ^ p4[i][5]);

		//дополнительные ограничения
		task &= !(p1[i][0] ^ p4[i][0]);
		task &= !(p3[i][4] ^ p4[i][4]);
		task &= !(p3[i][2] ^ p4[i][7]);
		task &= !(p1[i][7] ^ p2[i][7]);
	}

	//ограничения типа 3
	for (unsigned i = 0; i < N - 3; i++) {
		task &= !(p1[i][2] ^ p1[i + 3][4]);
	}
	task &= !p1[0][4] & !p1[1][4] & !p1[2][4] & !p1[8][2] & !p1[7][2] & !p1[6][2];
	for (unsigned i = 0; i < N - 3; i++) {
		task &= !(p3[i][1] ^ p2[i + 3][4]);
	}
	task &= !p2[0][4] & !p2[1][4] & !p2[2][4] & !p3[8][1] & !p3[7][1] & !p3[6][1];
	for (unsigned i = 0; i < N - 1; i++) {
		task &= !(p2[i][0] ^ p3[i + 1][5]);
	}
	task &= !p3[0][5] & !p2[8][0];	
	for (unsigned i = 0; i < N - 1; i++) {
		task &= !(p4[i][8] ^ p2[i + 1][1]);
	}
	task &= !p2[0][1] & !p4[8][8];
	
	//ограничения типа 4
	bdd temp1 = bddtrue;
	bdd temp2 = bddtrue;
	for (unsigned i = 0; i < N - 1; i++) {
		temp1 &= !(p1[i][2] ^ p1[i + 1][5]);
		temp2 &= !(p1[i][5] ^ p1[i + 1][2]);
	}
	task &= (temp1 | temp2);

	temp1 = bddtrue;
	temp2 = bddtrue;
	for (unsigned i = 0; i < N - 3; i++) {
		temp1 &= !(p2[i][0] ^ p4[i + 3][2]);
		temp2 &= !(p4[i][2] ^ p2[i + 3][0]);
	}
	task &= (temp1 | temp2);

	
	// переменные принимают разные значения
	for (unsigned j = 0; j < N; j++)
		for (unsigned i = 0; i < N - 1; i++)
			for (unsigned k = i + 1; k < N; k++)
			{
				task &= p1[i][j] >> !p1[k][j];
				task &= p2[i][j] >> !p2[k][j];
				task &= p3[i][j] >> !p3[k][j];
				task &= p4[i][j] >> !p4[k][j];
			}
			

	// вывод результатов
	out.open("out.txt");
	unsigned satcount = (unsigned)bdd_satcount(task);
	out << satcount << " solutions:\n" << endl;
	if (satcount) bdd_allsat(task, fun);
	out.close();
	bdd_done(); // завершение работы библиотеки
}

// Ниже приведена реализация функций, управляющих выводом результатов.
// Рекомендуется самостоятельно с ними ознакомиться.
// В собственных заданиях следует использовать эти функции
// или придумать собственные.
char var[N_VAR];

void print(void)
{
	for (unsigned i = 0; i < N; i++)
	{
		out << i << ": ";
		for (unsigned j = 0; j < M; j++)
		{
			int J = i * M*LOG_N + j * LOG_N;
			int num = 0;
			for (unsigned k = 0; k < LOG_N; k++) num += (unsigned)(var[J + k] << k);
			out << num << ' ';
		}
		out << endl;
	}
	out << endl;
}

void build(char* varset, unsigned n, unsigned I)
{
	if (I == n - 1)
	{
		if (varset[I] >= 0)
		{
			var[I] = varset[I];
			print();
			return;
		}
		var[I] = 0;
		print();
		var[I] = 1;
		print();
		return;
	}
	if (varset[I] >= 0)
	{
		var[I] = varset[I];
		build(varset, n, I + 1);
		return;
	}
	var[I] = 0;
	build(varset, n, I + 1);
	var[I] = 1;
	build(varset, n, I + 1);
}

void fun(char* varset, int size)
{
	build(varset, size, 0);
}