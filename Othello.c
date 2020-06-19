#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>

/*
下記工程に従う(詳細はアウトライン参照)

1.盤面生成…行、もしくは列の個数分だけ、決められた文字列を出力し、盤面に見立てる
2.マス指定…生成された盤面のどこに置くかを矢印キーで指定する
3.盤面保存…指定されたマスが駒を置けるマスか確認し、また置けるマスの候補を計算する
4.駒をひっくり返す…駒が置かれたマスに応じて盤面を処理する
5.勝利判定…コマ数に応じて先攻、後攻どちらが勝利か決定する

オセロの盤面を下記の値を一覧を用いて表す
0…まだ駒が置かれていないマス
1…先攻の駒(〇)が置かれているマス
-1…後攻の駒(●)が置かれているマス
2…先攻の駒を置くことができるマス
-2…後攻の駒を置くことができるマス
※通常先攻が黒駒(●)となるが、cmdの表示色上白駒(〇)で表示する

*/

/////////////////配列の要素、要素数を表す定数定義//////////////////////
#define ChoiceNum (3)//駒の配置を確認する際表示する選択肢の個数
#define OffsetFlgElementNum (4)//プログラムを通じて必要な変数配列の要素数

///////////////// 盤面を表す配列に格納する定数定義 /////////////////
#define EmpPieceNumber (0) //駒が置かれていないマス[を示す値]
#define WhitePieceNumber (-1) //後攻の駒が置かれているマス[を示す値]
#define BlackPieceNumber (1) //先攻の駒が置かれているマス[を示す値]
#define PutAbleNumber (2) //駒を置くことができるマス[を示す値]
#define PutCandidateNumber (3)//駒を後で戻すマス[を示す値]
#define EdgeNumber (99)//ひっくり返す際終端を表すマス[を示す値]

///////////////// 盤面のマスの数を規定する定数定義 /////////////////
#define ColNum (8) //盤面の行数[を示す値]
#define ColRange (2 * ColNum) //盤面生成用の行数[を示す値]
#define BoardFlgListNum (ColNum + 2) //オセロのフラグ配列の長さ[を示す値]

#define CheckLoopNum (8)//配置した駒からループを繰り返す回数[を示す値]

////////////////////////駒の周囲でループする際必要な配列及び定数/////////////////////////////
int boardFlgList[BoardFlgListNum][BoardFlgListNum];//オセロのフラグ配列

//各マスから行、列に対して座標をずらす値
const int LoopByLoopArrange[2][CheckLoopNum] = {
	{-1, 0, 1, 1, 1, 0, -1, -1},//行
	{1, 1, 1, 0, -1, -1, -1, 0}//列
};

///////////////////////////盤面上の駒数を数えるための構造体////////////////////////////////
struct i_putAbleArrange {//駒を置くことができるマスの座標。初期の駒が置かれているマスの数だけ行、列それぞれ値を引いておく
	int pos_x[ColNum * ColNum - 2*2];//x座標
	int pos_y[ColNum * ColNum - 2*2];//y座標
	int pos_Num;//座標の個数
};

struct checkBothPieceNumber {//盤面の白駒、黒駒の個数を格納する構造体
	int count_b;//黒駒の個数
	int count_w;//白駒の個数
};

///////////////////////////プログラム全体を通じて必要な配列の初期値//////////////////////////////////
unsigned int ini_offset_x = 1;//カーソルのx座標
unsigned int ini_offset_y = 1;//カーソルのy座標
int keyflg = 0;//0:矢印、1:Enterキー、(2:盤面確認)
int turn = 0;//盤面におけるマスがあるとき1、なければ0に設定。main関数の最初に初期化し、盤面をひっくり返す処理の後に判定。

/////////////////////////////////////////////プログラム内の関数一覧///////////////////////////////////////////////////////////////
void makeInitBoard(int boardFlgList[BoardFlgListNum][BoardFlgListNum]);//オセロの初期盤面配列生成関数
struct i_putAbleArrange checkPlace_x(int boardFlgList[BoardFlgListNum][BoardFlgListNum], int turn, int pieceNumber);//駒設置前における場所を確認
void printBoard(int boardFlgList[BoardFlgListNum][BoardFlgListNum], int offset_x, int offset_y);//盤面生成用関数
void getArrow(int boardFlgList[BoardFlgListNum][BoardFlgListNum], int offset_flg_turn[OffsetFlgElementNum]);//キー入力判定関数
int checkchoice(int boardFlgList[BoardFlgListNum][BoardFlgListNum], int offset_flg_turn[OffsetFlgElementNum], int putAbleArrangeNum, int n_putAbleCount);//駒の配置可否判断関数
struct i_putAbleArrange reverseBoard_x(int boardFlgList[BoardFlgListNum][BoardFlgListNum], int offset_flg_turn[OffsetFlgElementNum]);//盤面上の駒をひっくり返す関数
void judgeVicotryPlayer(int boardFlgList[BoardFlgListNum][BoardFlgListNum]);//勝利プレイヤー判定フラグ
void backBoard(int boardFlgList[BoardFlgListNum][BoardFlgListNum], struct i_putAbleArrange putAbleArrange, int pieceNuber);//「確認用のマス」、「置ける候補のマス」を元に戻す
struct checkBothPieceNumber checkBothPieceNum(int boardFlgList[BoardFlgListNum][BoardFlgListNum]);//盤面のそれぞれの駒数を数える

int passNum = 0;//パスが起こった回数。全体のターン数を調整する。

int main(void)
{
	int n_putAbleCount = 0;//盤面に駒が置けない場合値がインクリメント。2になった場合強制的に勝利プレイヤーを判定。
	int offset_flg_turn[OffsetFlgElementNum] = { ini_offset_x, ini_offset_y, keyflg, turn};//カーソルのx座標、カーソルのy座標、入力キー、現在の操作プレイヤー
	struct i_putAbleArrange putAbleArrange, reversedBoardArrange;//(左):その時点で駒を置くことができるマス、(右):駒をひっくり返したマスの一覧
	int turnNum = 4;//全体のターン数。パスの際カウントを1マイナスする。

	//オセロのフラグ配列生成
	makeInitBoard(boardFlgList);

	//空いてるマス分ループ
	while((turnNum - passNum) < ColNum * ColNum)
	{
		///////////////////////////盤面の駒が一色の場合、その時点でゲーム終了//////////////////////////////

		struct checkBothPieceNumber piecePairNum;

		piecePairNum = checkBothPieceNum(boardFlgList);

		if ((piecePairNum.count_b == 0) || (piecePairNum.count_w == 0))
			break;
		
		///////////////////////////////////////////////////////////////////////////////////////////////////

		///////////////////////////////マス選択のための前処理//////////////////////////////////////////////
		
		//先攻、後攻のフラグを決定
		if (turnNum % 2)
			offset_flg_turn[3] = WhitePieceNumber;
		else
			offset_flg_turn[3] = BlackPieceNumber;
		
		//駒を置くことができるマスを計算
		putAbleArrange = checkPlace_x(boardFlgList, offset_flg_turn[3], PutAbleNumber);

		///////////////////////////////////////////////////////////////////////////////////////////////////

		
		////////////////両プレイヤーが駒を置けなかった場合、その時点でゲーム終了///////////////////////////
		
		if (n_putAbleCount == 2)
			break;
		else if (putAbleArrange.pos_Num == 0)
			n_putAbleCount += 1;
		
		///////////////////////////////////////////////////////////////////////////////////////////////////

		///////////////////////////マス選択画面。確認時Enterが押されるまでループ///////////////////////////
		do {
			system("cls");

			//先攻、後攻を判定し、今の操作プレイヤーを表示
			if (offset_flg_turn[3] == BlackPieceNumber)
				printf("現在先攻がマスを選択しています。\n\n");
			else if (offset_flg_turn[3] == WhitePieceNumber)
				printf("現在後攻がマスを選択しています。\n\n");

			printf("%d\n", turnNum);//デバッグ用

			//盤面の状態、カーソルの座標を更新
			printBoard(boardFlgList, offset_flg_turn[0], offset_flg_turn[1]);

			//キー入力を受付け、結果を盤面に反映
			getArrow(boardFlgList, offset_flg_turn);

			//プレイヤーに駒を配置するマスを確認
			//またこの時点で、置いたと仮定した場合に次のプレイヤーが配置できるマスの個数を計算
			n_putAbleCount = checkchoice(boardFlgList, offset_flg_turn, putAbleArrange.pos_Num, n_putAbleCount);
			} while (offset_flg_turn[2] != 1);
		
		//////////////////////////////////////////////////////////////////////////////////////////////////

		
		///////////////////////////////////駒をひっくり返す///////////////////////////////////////////////
		
		//駒を置くことができるマスを計算、対象マスの座標を全て取得する
		putAbleArrange = checkPlace_x(boardFlgList, offset_flg_turn[3], PutAbleNumber);

		//駒を置くことができるマスを一度リセット
		backBoard(boardFlgList, putAbleArrange, EmpPieceNumber);

		//駒をひっくり返す(戻り値で出てきた構造体は本ループでは使わない)
		reversedBoardArrange = reverseBoard_x(boardFlgList, offset_flg_turn);
		
		//////////////////////////////////////////////////////////////////////////////////////////////////
		
		
		////////////////////////////////次ループのための後処理////////////////////////////////////////////
		
		//getArrow関数で、Enterキーを押したことで建てたフラグを元に戻す
		offset_flg_turn[2] = 0;

		//経過ターン数をインクリメント
		turnNum++;

		//////////////////////////////////////////////////////////////////////////////////////////////////
	}


	/////////////////////////////////盤面を更新し、勝利プレイヤーを判定///////////////////////////////////
	system("cls");

	//カーソルを除いた盤面を表示
	printBoard(boardFlgList, 0, 0);

	//勝利プレイヤー判定
	judgeVicotryPlayer(boardFlgList);

	//////////////////////////////////////////////////////////////////////////////////////////////////////

	return 0;
}

//盤面を表す配列を生成
void makeInitBoard(int boardFlgList[BoardFlgListNum][BoardFlgListNum])
{
	//盤面を表す配列の端をすべてEdgeNumberに、それ以外を0に設定
	for (int col = 0; col < BoardFlgListNum; col++)
	{
		for (int raw = 0; raw < BoardFlgListNum; raw++)
		{
			if ((col == 0) | (col == BoardFlgListNum - 1) | (raw == 0) | (raw == BoardFlgListNum - 1))
				boardFlgList[col][raw] = EdgeNumber;
			else
				boardFlgList[col][raw] = EmpPieceNumber;
		}
	}

	//オセロの初期設置駒
	boardFlgList[ColNum / 2][ColNum / 2] = WhitePieceNumber;
	boardFlgList[ColNum / 2 + 1][ColNum / 2 + 1] = WhitePieceNumber;
	boardFlgList[ColNum / 2 + 1][ColNum / 2] = BlackPieceNumber;
	boardFlgList[ColNum / 2][ColNum / 2 + 1] = BlackPieceNumber;
}

//盤面生成
void printBoard(int boardFlgList[BoardFlgListNum][BoardFlgListNum], int offset_x, int offset_y)
{
	for (int col = 0; col < ColRange + 1; col++)//行ループ
	{
		for (int raw = 0; raw < ColRange + 1; raw++)//列ループ
		{
			if (col % 2)
			{
				if (((float)offset_x == ((float)(col + 1) / 2)) && ((float)offset_y == ((float)(raw + 1) / 2)))//現在のカーソルの座標
					printf(" ■ ");
				else if ((raw % 2) & (boardFlgList[(col + 1) / 2][(raw + 1) / 2] == BlackPieceNumber))//先攻の駒の座標
					printf(" 〇 ");
				else if ((raw % 2) & (boardFlgList[(col + 1) / 2][(raw + 1) / 2] == WhitePieceNumber))//後攻の駒の座標
					printf(" ● ");
				else if ((raw % 2) & (boardFlgList[(col + 1) / 2][(raw + 1) / 2] == PutAbleNumber))//その時のプレイヤーが駒を置くことができるマス
					printf(" × ");
				else if (raw % 2)//駒が置かれていないマス
					printf(" 　 ");
				else
					printf("┃ ");
			}
			else if(col == 0)//以降盤面の外枠
			{
				if (raw == 0)
					printf("┏  ");
				else if (raw == ColRange)
					printf("┓");
				else if (raw % 2)
					printf("━  ");
				else
					printf("┳  ");
			}
			else if (col == ColRange) 
			{
				if (raw == 0)
					printf("┗  ");
				else if (raw == ColRange)
					printf("┛");
				else if (raw % 2)
					printf("━  ");
				else
					printf("┻  ");
			}
			else
			{
				if (raw == 0)
					printf("┣  ");
				else if (raw == ColRange)
					printf("┫");
				else if (raw % 2)
					printf("━  ");
				else
					printf("╋  ");
			}
		}

		printf("\n\n");
	}
}

//キー入力判定
void getArrow(int boardFlgList[BoardFlgListNum][BoardFlgListNum], int offset_flg_turn[OffsetFlgElementNum]) {

	int flag = 1;//矢印キー入力感知フラグ
	int Arrow;
	
	//矢印キー、もしくはEnterキーが押されるまでループ
	while(flag)
	{
		//単に入力待ちだけならkbhit関数はいらない？
		//将来的に時間制限機能を付与する際用いることができるかも？
		//if (_kbhit())
		//{
			Arrow = _getch();//入力待ち

			if (Arrow == 0x0d)//入力されたキーがEnterの場合ループを抜ける
				flag = 0;
			else if (Arrow == 0xe0)//入力されたキーが矢印キーの場合、もう一度値を読み取ってループを抜ける
			{
				Arrow = _getch();
				flag = 0;
			}
		//}
			/*
			if(_kbhit())
			{
				if (((Arrow == 0x00 || Arrow == 0xffffffe0) || Arrow == 0x0d) ? _getch() : Arrow)
				{
					Arrow = _getch();
					flag = 0;
				}
			}
			*/
	}
		switch (Arrow)//入力されたキーに応じてカーソルの挙動を変化
		{
		case 0x48://上矢印
			offset_flg_turn[0] -= 1;
			if (boardFlgList[offset_flg_turn[0]][offset_flg_turn[1]] == EdgeNumber)
				offset_flg_turn[0] += ColNum;
			break;
		case 0x50://下矢印
			offset_flg_turn[0] += 1;
			if (boardFlgList[offset_flg_turn[0]][offset_flg_turn[1]] == EdgeNumber)
				offset_flg_turn[0] -= ColNum;
			break;
		case 0x4B://左矢印
			offset_flg_turn[1] -= 1;
			if (boardFlgList[offset_flg_turn[0]][offset_flg_turn[1]] == EdgeNumber)
				offset_flg_turn[1] += ColNum;
			break;
		case 0x4D://右矢印
			offset_flg_turn[1] += 1;
			if (boardFlgList[offset_flg_turn[0]][offset_flg_turn[1]] == EdgeNumber)
				offset_flg_turn[1] -= ColNum;
			break;
		case 0x0d://Enterキー
			offset_flg_turn[2] = 1;
			break;
		}
}

//プレイヤーに配置するマスの確認を促す
int checkchoice(int boardFlgList[BoardFlgListNum][BoardFlgListNum], int offset_flg_turn[OffsetFlgElementNum], int putAbleArrangeNum, int n_putAbleCount)
{
	int temp_boardFlgList[BoardFlgListNum][BoardFlgListNum];//現在の盤面のコピー
	
	struct i_putAbleArrange putAbleArrange;//確認用盤面の表示のために、現在の
	
	//[0]:Enterキー、もしくは矢印キーを押すまで待機
	//[1]:確認時、下記選択肢でEnterキーを押すまでループ
	int LoopFlag[2];
	int Arrow;//入力されたキーの値

	char choice[ChoiceNum][10] = { "はい", "いいえ", "確認" };//確認時の選択肢
	int choiceArrow = 0;//選択肢のカーソルの座標
	
	if (!(putAbleArrangeNum))//駒をひっくり返すことができるマスが存在しない場合
	{
		printf("ひっくり返すことができる駒がありません。パスします。");
		offset_flg_turn[2] = 1;
		passNum++;

		while (1)
		{
			if (_kbhit())
				break;
		}
	}
	else if (((boardFlgList[offset_flg_turn[0]][offset_flg_turn[1]] == BlackPieceNumber) | (boardFlgList[offset_flg_turn[0]][offset_flg_turn[1]] == WhitePieceNumber)) & (offset_flg_turn[2] == 1))//駒の配置を決定したマスにすでに駒が置いてある場合
	{
		printf("すでに駒が置かれています。他のマスを指定してください。\n");
		offset_flg_turn[2] = 0;
		n_putAbleCount = 0;

		while (1)
		{
			if (_kbhit())
				break;
		}
	}
	else if ((boardFlgList[offset_flg_turn[0]][offset_flg_turn[1]] == EmpPieceNumber) & (offset_flg_turn[2] == 1))//駒の配置を決定したマスが候補外のマスだった場合
	{
		n_putAbleCount = 0;
		offset_flg_turn[2] = 0;
		printf("そのマスには駒を置くことができません。他のマスを指定してください。\n");

		while (1)
		{
			if (_kbhit())
				break;
		}
	}
	else if ((boardFlgList[offset_flg_turn[0]][offset_flg_turn[1]] == PutAbleNumber) & (offset_flg_turn[2] == 1)) {
		n_putAbleCount = 0;
		do {
			LoopFlag[0] = 1;//Enterキー、もしくは矢印キーを押すまで待機
			LoopFlag[1] = 1;//確認時、下記選択肢でEnterキーを押すまでループ

			//盤面更新
			system("cls");
			printBoard(boardFlgList, offset_flg_turn[0], offset_flg_turn[1]);
			printf("\nそのマスに駒を配置してよろしいですか？\n");

			//選択肢表示
			for (int t_loop = 0; t_loop < ChoiceNum; t_loop++)
			{
				if (t_loop == 0)
					printf("　　");
				else
					printf("　　　");

				if (choiceArrow == t_loop)
					printf("\b\b■");

				for (int t_str = 0; t_str < sizeof(choice[t_loop]) / 2; t_str++) {
					printf("%c%c", choice[t_loop][2 * t_str], choice[t_loop][2 * t_str + 1]);
				}
			}

			//Enterキー、矢印キーが押されるまでループ
			while (LoopFlag[0])
			{
				Arrow = _getch();

				if (Arrow == 0x0d)
					LoopFlag[0] = 0;
				else if (Arrow == 0xe0)
				{
					Arrow = _getch();
					LoopFlag[0] = 0;
				}
				//if (_kbhit())
				//{
					/*
					if ((Arrow == 0x00 || Arrow == 0xffffffe0) ? _getch() : Arrow)
					{
						Arrow = _getch();
						LoopFlag[0] = 0;
					}
					*/
				//}
			}

			//Enerキーが押されるまで選択肢を表示し続ける
			switch (Arrow)
			{
			case 0x4B://左矢印
				choiceArrow -= 1;
				if (choiceArrow == -1)
					choiceArrow += ChoiceNum;
				break;
			case 0x4D://右矢印
				choiceArrow += 1;
				if (choiceArrow == ChoiceNum)
					choiceArrow -= ChoiceNum;
				break;
			case 0x0d://Enterキー
				LoopFlag[1] = 0;
				break;
			}
		} while (LoopFlag[1] == 1);

		if (choiceArrow == 1)//「いいえ」選択時
			offset_flg_turn[2] = 0;
		else if (choiceArrow == 2)//「確認」選択時
		{
			system("cls");

			//盤面のコピー
			//memcpy(temp_boardFlgList, boardFlgList, sizeof(boardFlgList[BoardFlgListNum][BoardFlgListNum]));//下記を配列全体のサイズからmemcpyを使いたい
			memcpy(temp_boardFlgList, boardFlgList, sizeof(int) * BoardFlgListNum * BoardFlgListNum);

			//現在の盤面情報を一時消去
			putAbleArrange = checkPlace_x(temp_boardFlgList, offset_flg_turn[3], PutAbleNumber);//仮にカーソルの座標に駒を配置した際の盤面を計算、その時変更した座標を全て保存
			backBoard(temp_boardFlgList, putAbleArrange, EmpPieceNumber);//仮の盤面を元の盤面に戻す
			
			//ひっくり返した後の盤面を表示
			putAbleArrange = reverseBoard_x(temp_boardFlgList, offset_flg_turn);
			putAbleArrange = checkPlace_x(temp_boardFlgList, (-1)*offset_flg_turn[3], PutAbleNumber);
			printBoard(temp_boardFlgList, 0, 0);//カーソルを除いた盤面を表示

			system("pause");

			offset_flg_turn[2] = 0;
		}
	}

	return n_putAbleCount;
}

//盤面のそれぞれの駒数を数える
struct checkBothPieceNumber checkBothPieceNum(int boardFlgList[BoardFlgListNum][BoardFlgListNum])
{
	//int count_b = 0;
	//int count_w = 0;

	struct checkBothPieceNumber piecePairNum;

	piecePairNum.count_b = 0;//黒駒
	piecePairNum.count_w = 0;//白駒

	//&(piecePairNum[0]) = 0;
	
	/*
	printf("%p, %p, %p, %p\n", &count_b, &count_w, piecePairNum[0], piecePairNum[1]);

	piecePairNum[0] = &count_b;
	piecePairNum[1] = &count_w;

	printf("%p, %p, %p, %p\n", &count_b, &count_w, piecePairNum[0], piecePairNum[1]);
	*/

	for (int col = 0; col < BoardFlgListNum; col++)
	{
		for (int raw = 0; raw < BoardFlgListNum; raw++)
		{
			//駒が配置されていないマスを飛ばす
			if ((col == 0) | (col == BoardFlgListNum - 1) | (raw == 0) | (raw == BoardFlgListNum - 1))
				continue;
			else if (boardFlgList[col][raw] == EmpPieceNumber)
				continue;

			//それぞれの駒のカウントをインクリメント
			if (boardFlgList[col][raw] == 1)
				piecePairNum.count_b += 1;
			else if (boardFlgList[col][raw] == -1)
				piecePairNum.count_w += 1;
		}
	}

	return piecePairNum;
}

//盤面全体の駒の個数を白、黒別に合計し、多い方のプレイヤーを勝利にする、
void judgeVicotryPlayer(int boardFlgList[BoardFlgListNum][BoardFlgListNum])
{
	struct checkBothPieceNumber piecePairNum;
	
	piecePairNum = checkBothPieceNum(boardFlgList);//各プレイヤーの駒の総数をカウント

	printf("\n\n\n");

	printf("黒駒：%d個\n", piecePairNum.count_b);
	printf("白駒：%d個\n\n", piecePairNum.count_w);

	if (piecePairNum.count_b > piecePairNum.count_w)
		printf("先攻の勝利！\n\n");
	else if (piecePairNum.count_b == piecePairNum.count_w)
		printf("引き分け！\n\n");
	else if (piecePairNum.count_b < piecePairNum.count_w)
		printf("後攻の勝利！\n\n");
}

//盤面上の駒をひっくり返す関数
struct i_putAbleArrange reverseBoard_x(int boardFlgList[BoardFlgListNum][BoardFlgListNum], int offset_flg_turn[OffsetFlgElementNum])
{
	struct i_putAbleArrange reversedBoardArrange;

	reversedBoardArrange.pos_Num = 0;

	//上下左右斜め、計八方向を確認
	for (int tempLoopNum = 0; tempLoopNum < CheckLoopNum; tempLoopNum++)
	{
		int check_flg = 0;//確認するマスから、ある方向に相手の駒が置かれている時1にする
		int count = 0;//駒をひっくり返す際、確認中の座標と確認元の差表が一致したとき1にする
		int ini_temp_offset[2] = { offset_flg_turn[0], offset_flg_turn[1] };//確認するマスの初期座標
		int temp_offset[2] = { ini_temp_offset[0], ini_temp_offset[1] };//初期座標からずらした後の座標

		//特定方向に座標をずらす
		temp_offset[0] += LoopByLoopArrange[0][tempLoopNum];
		temp_offset[1] += LoopByLoopArrange[1][tempLoopNum];

		//printf("%d\n", boardFlgList[temp_offset[0]][temp_offset[1]]);

		////ずらした先に相手の駒が配置してある場合
		if (boardFlgList[temp_offset[0]][temp_offset[1]] == (-1) * offset_flg_turn[3])
		{
			//相手の駒が続く限りループ
			do
			{
				temp_offset[0] += LoopByLoopArrange[0][tempLoopNum];
				temp_offset[1] += LoopByLoopArrange[1][tempLoopNum];
				check_flg = 1;
				count++;
			} while (boardFlgList[temp_offset[0]][temp_offset[1]] == (-1) * offset_flg_turn[3]);

			//ループが途絶えたマスが自分の駒以外であった場合ループを飛ばす
			if (boardFlgList[temp_offset[0]][temp_offset[1]] != offset_flg_turn[3])
				continue;

			//ループが途絶えたマスに自分の駒が置かれている場合、そのループ内で確認したマスをすべて自分の駒にする
			if ((boardFlgList[temp_offset[0]][temp_offset[1]] == offset_flg_turn[3]) & (check_flg == 1))
			{
				do {
					temp_offset[0] -= LoopByLoopArrange[0][tempLoopNum];
					temp_offset[1] -= LoopByLoopArrange[1][tempLoopNum];
					boardFlgList[temp_offset[0]][temp_offset[1]] = offset_flg_turn[3];

					reversedBoardArrange.pos_x[reversedBoardArrange.pos_Num] = temp_offset[0];
					reversedBoardArrange.pos_y[reversedBoardArrange.pos_Num] = temp_offset[1];
					reversedBoardArrange.pos_Num++;

					count--;
				} while (count != -1);
			}
		}
	}

	return reversedBoardArrange;
}


//駒配置時、ひっくり返すことができるマスを格納
struct i_putAbleArrange checkPlace_x(int boardFlgList[BoardFlgListNum][BoardFlgListNum], int turn,int pieceNumber)
{
	struct i_putAbleArrange putAbleArrange;

	putAbleArrange.pos_Num = 0;

	//盤面の全マスを確認
	for (int col = 0; col < BoardFlgListNum; col++)
	{
		for (int raw = 0; raw < BoardFlgListNum; raw++)
		{
			//printf("%d\n", boardFlgList[col][raw]);
			//端のマス、または駒がすでに配置されているマスは確認せず飛ばす
			if ((boardFlgList[col][raw] == EdgeNumber) | (boardFlgList[col][raw] == BlackPieceNumber) | (boardFlgList[col][raw] == WhitePieceNumber))
				continue;

			//上下左右斜め、計八方向を確認
			for (int tempLoopNum = 0; tempLoopNum < CheckLoopNum; tempLoopNum++)
			{
				int flg = 0;//確認するマスから、ある方向に相手の駒が置かれている時1にする
				int temp_offset[2] = { col, raw };//確認するマスの初期座標

				//特定方向に座標をずらす
				temp_offset[0] += LoopByLoopArrange[0][tempLoopNum];
				temp_offset[1] += LoopByLoopArrange[1][tempLoopNum];

				//printf("%d, %d, %d\n", temp_offset[0], temp_offset[1], boardFlgList[temp_offset[0]][temp_offset[1]]);

				////ずらした先に相手の駒が配置してある場合
				if (boardFlgList[temp_offset[0]][temp_offset[1]] == (-1) * turn)
				{
					//相手の駒が続く限りループ
					do
					{
						temp_offset[0] += LoopByLoopArrange[0][tempLoopNum];
						temp_offset[1] += LoopByLoopArrange[1][tempLoopNum];
						flg = 1;
						//printf("%d\n", boardFlgList[temp_offset[0]][temp_offset[1]]);
					} while (boardFlgList[temp_offset[0]][temp_offset[1]] == (-1) * turn);

					//ループが途絶えたマスが端、もしくは自分の駒の場合ループを飛ばす
					if (boardFlgList[temp_offset[0]][temp_offset[1]] != turn)
						continue;

					//ループが途絶えたマスに自分の駒が置かれており、その先に駒が置かれていないマスが存在する場合、そのマスを設置可能な場所とす
					if ((boardFlgList[temp_offset[0]][temp_offset[1]] == turn) & (flg == 1))
					{
						//printf("%d\n", boardFlgList[col][raw]);
						boardFlgList[col][raw] = pieceNumber;
						putAbleArrange.pos_x[putAbleArrange.pos_Num] = col;
						putAbleArrange.pos_y[putAbleArrange.pos_Num] = raw;
						putAbleArrange.pos_Num++;
					}
				}
			}
		}
	}

	return putAbleArrange;
}

//「確認用のマス」、「置ける候補のマス」を元に戻す
void backBoard(int boardFlgList[BoardFlgListNum][BoardFlgListNum], struct i_putAbleArrange putAbleArrange, int pieceNumber)
{
	int papl_x;//対象座標のx座標(putAblePlace_x)
	int papl_y;//対象座標のy座標(putAblePlace_y)
	int papl_Num = putAbleArrange.pos_Num;//対象座標の個数

	for (int pieceN = 0; pieceN < papl_Num; pieceN++)
	{
		papl_x = putAbleArrange.pos_x[pieceN];
		papl_y = putAbleArrange.pos_y[pieceN];

		boardFlgList[papl_x][papl_y] = pieceNumber;
	}
}