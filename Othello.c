#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>

/*
���L�H���ɏ]��(�ڍׂ̓A�E�g���C���Q��)

1.�Ֆʐ����c�s�A�������͗�̌��������A���߂�ꂽ��������o�͂��A�ՖʂɌ����Ă�
2.�}�X�w��c�������ꂽ�Ֆʂ̂ǂ��ɒu��������L�[�Ŏw�肷��
3.�Ֆʕۑ��c�w�肳�ꂽ�}�X�����u����}�X���m�F���A�܂��u����}�X�̌����v�Z����
4.����Ђ�����Ԃ��c��u���ꂽ�}�X�ɉ����ĔՖʂ���������
5.��������c�R�}���ɉ����Đ�U�A��U�ǂ��炪���������肷��

�I�Z���̔Ֆʂ����L�̒l���ꗗ��p���ĕ\��
0�c�܂���u����Ă��Ȃ��}�X
1�c��U�̋�(�Z)���u����Ă���}�X
-1�c��U�̋�(��)���u����Ă���}�X
2�c��U�̋��u�����Ƃ��ł���}�X
-2�c��U�̋��u�����Ƃ��ł���}�X
���ʏ��U������(��)�ƂȂ邪�Acmd�̕\���F�㔒��(�Z)�ŕ\������

*/

/////////////////�z��̗v�f�A�v�f����\���萔��`//////////////////////
#define ChoiceNum (3)//��̔z�u���m�F����ە\������I�����̌�
#define OffsetFlgElementNum (4)//�v���O������ʂ��ĕK�v�ȕϐ��z��̗v�f��

///////////////// �Ֆʂ�\���z��Ɋi�[����萔��` /////////////////
#define EmpPieceNumber (0) //��u����Ă��Ȃ��}�X[�������l]
#define WhitePieceNumber (-1) //��U�̋�u����Ă���}�X[�������l]
#define BlackPieceNumber (1) //��U�̋�u����Ă���}�X[�������l]
#define PutAbleNumber (2) //���u�����Ƃ��ł���}�X[�������l]
#define PutCandidateNumber (3)//�����Ŗ߂��}�X[�������l]
#define EdgeNumber (99)//�Ђ�����Ԃ��ۏI�[��\���}�X[�������l]

///////////////// �Ֆʂ̃}�X�̐����K�肷��萔��` /////////////////
#define ColNum (8) //�Ֆʂ̍s��[�������l]
#define ColRange (2 * ColNum) //�Ֆʐ����p�̍s��[�������l]
#define BoardFlgListNum (ColNum + 2) //�I�Z���̃t���O�z��̒���[�������l]

#define CheckLoopNum (8)//�z�u������烋�[�v���J��Ԃ���[�������l]

////////////////////////��̎��͂Ń��[�v����ەK�v�Ȕz��y�ђ萔/////////////////////////////
int boardFlgList[BoardFlgListNum][BoardFlgListNum];//�I�Z���̃t���O�z��

//�e�}�X����s�A��ɑ΂��č��W�����炷�l
const int LoopByLoopArrange[2][CheckLoopNum] = {
	{-1, 0, 1, 1, 1, 0, -1, -1},//�s
	{1, 1, 1, 0, -1, -1, -1, 0}//��
};

///////////////////////////�Ֆʏ�̋�𐔂��邽�߂̍\����////////////////////////////////
struct i_putAbleArrange {//���u�����Ƃ��ł���}�X�̍��W�B�����̋�u����Ă���}�X�̐������s�A�񂻂ꂼ��l�������Ă���
	int pos_x[ColNum * ColNum - 2*2];//x���W
	int pos_y[ColNum * ColNum - 2*2];//y���W
	int pos_Num;//���W�̌�
};

struct checkBothPieceNumber {//�Ֆʂ̔���A����̌����i�[����\����
	int count_b;//����̌�
	int count_w;//����̌�
};

///////////////////////////�v���O�����S�̂�ʂ��ĕK�v�Ȕz��̏����l//////////////////////////////////
unsigned int ini_offset_x = 1;//�J�[�\����x���W
unsigned int ini_offset_y = 1;//�J�[�\����y���W
int keyflg = 0;//0:���A1:Enter�L�[�A(2:�Ֆʊm�F)
int turn = 0;//�Ֆʂɂ�����}�X������Ƃ�1�A�Ȃ����0�ɐݒ�Bmain�֐��̍ŏ��ɏ��������A�Ֆʂ��Ђ�����Ԃ������̌�ɔ���B

/////////////////////////////////////////////�v���O�������̊֐��ꗗ///////////////////////////////////////////////////////////////
void makeInitBoard(int boardFlgList[BoardFlgListNum][BoardFlgListNum]);//�I�Z���̏����Ֆʔz�񐶐��֐�
struct i_putAbleArrange checkPlace_x(int boardFlgList[BoardFlgListNum][BoardFlgListNum], int turn, int pieceNumber);//��ݒu�O�ɂ�����ꏊ���m�F
void printBoard(int boardFlgList[BoardFlgListNum][BoardFlgListNum], int offset_x, int offset_y);//�Ֆʐ����p�֐�
void getArrow(int boardFlgList[BoardFlgListNum][BoardFlgListNum], int offset_flg_turn[OffsetFlgElementNum]);//�L�[���͔���֐�
int checkchoice(int boardFlgList[BoardFlgListNum][BoardFlgListNum], int offset_flg_turn[OffsetFlgElementNum], int putAbleArrangeNum, int n_putAbleCount);//��̔z�u�۔��f�֐�
struct i_putAbleArrange reverseBoard_x(int boardFlgList[BoardFlgListNum][BoardFlgListNum], int offset_flg_turn[OffsetFlgElementNum]);//�Ֆʏ�̋���Ђ�����Ԃ��֐�
void judgeVicotryPlayer(int boardFlgList[BoardFlgListNum][BoardFlgListNum]);//�����v���C���[����t���O
void backBoard(int boardFlgList[BoardFlgListNum][BoardFlgListNum], struct i_putAbleArrange putAbleArrange, int pieceNuber);//�u�m�F�p�̃}�X�v�A�u�u������̃}�X�v�����ɖ߂�
struct checkBothPieceNumber checkBothPieceNum(int boardFlgList[BoardFlgListNum][BoardFlgListNum]);//�Ֆʂ̂��ꂼ��̋�𐔂���

int passNum = 0;//�p�X���N�������񐔁B�S�̂̃^�[�����𒲐�����B

int main(void)
{
	int n_putAbleCount = 0;//�Ֆʂɋ�u���Ȃ��ꍇ�l���C���N�������g�B2�ɂȂ����ꍇ�����I�ɏ����v���C���[�𔻒�B
	int offset_flg_turn[OffsetFlgElementNum] = { ini_offset_x, ini_offset_y, keyflg, turn};//�J�[�\����x���W�A�J�[�\����y���W�A���̓L�[�A���݂̑���v���C���[
	struct i_putAbleArrange putAbleArrange, reversedBoardArrange;//(��):���̎��_�ŋ��u�����Ƃ��ł���}�X�A(�E):����Ђ�����Ԃ����}�X�̈ꗗ
	int turnNum = 4;//�S�̂̃^�[�����B�p�X�̍ۃJ�E���g��1�}�C�i�X����B

	//�I�Z���̃t���O�z�񐶐�
	makeInitBoard(boardFlgList);

	//�󂢂Ă�}�X�����[�v
	while((turnNum - passNum) < ColNum * ColNum)
	{
		///////////////////////////�Ֆʂ̋��F�̏ꍇ�A���̎��_�ŃQ�[���I��//////////////////////////////

		struct checkBothPieceNumber piecePairNum;

		piecePairNum = checkBothPieceNum(boardFlgList);

		if ((piecePairNum.count_b == 0) || (piecePairNum.count_w == 0))
			break;
		
		///////////////////////////////////////////////////////////////////////////////////////////////////

		///////////////////////////////�}�X�I���̂��߂̑O����//////////////////////////////////////////////
		
		//��U�A��U�̃t���O������
		if (turnNum % 2)
			offset_flg_turn[3] = WhitePieceNumber;
		else
			offset_flg_turn[3] = BlackPieceNumber;
		
		//���u�����Ƃ��ł���}�X���v�Z
		putAbleArrange = checkPlace_x(boardFlgList, offset_flg_turn[3], PutAbleNumber);

		///////////////////////////////////////////////////////////////////////////////////////////////////

		
		////////////////���v���C���[�����u���Ȃ������ꍇ�A���̎��_�ŃQ�[���I��///////////////////////////
		
		if (n_putAbleCount == 2)
			break;
		else if (putAbleArrange.pos_Num == 0)
			n_putAbleCount += 1;
		
		///////////////////////////////////////////////////////////////////////////////////////////////////

		///////////////////////////�}�X�I����ʁB�m�F��Enter���������܂Ń��[�v///////////////////////////
		do {
			system("cls");

			//��U�A��U�𔻒肵�A���̑���v���C���[��\��
			if (offset_flg_turn[3] == BlackPieceNumber)
				printf("���ݐ�U���}�X��I�����Ă��܂��B\n\n");
			else if (offset_flg_turn[3] == WhitePieceNumber)
				printf("���݌�U���}�X��I�����Ă��܂��B\n\n");

			printf("%d\n", turnNum);//�f�o�b�O�p

			//�Ֆʂ̏�ԁA�J�[�\���̍��W���X�V
			printBoard(boardFlgList, offset_flg_turn[0], offset_flg_turn[1]);

			//�L�[���͂���t���A���ʂ�Ֆʂɔ��f
			getArrow(boardFlgList, offset_flg_turn);

			//�v���C���[�ɋ��z�u����}�X���m�F
			//�܂����̎��_�ŁA�u�����Ɖ��肵���ꍇ�Ɏ��̃v���C���[���z�u�ł���}�X�̌����v�Z
			n_putAbleCount = checkchoice(boardFlgList, offset_flg_turn, putAbleArrange.pos_Num, n_putAbleCount);
			} while (offset_flg_turn[2] != 1);
		
		//////////////////////////////////////////////////////////////////////////////////////////////////

		
		///////////////////////////////////����Ђ�����Ԃ�///////////////////////////////////////////////
		
		//���u�����Ƃ��ł���}�X���v�Z�A�Ώۃ}�X�̍��W��S�Ď擾����
		putAbleArrange = checkPlace_x(boardFlgList, offset_flg_turn[3], PutAbleNumber);

		//���u�����Ƃ��ł���}�X����x���Z�b�g
		backBoard(boardFlgList, putAbleArrange, EmpPieceNumber);

		//����Ђ�����Ԃ�(�߂�l�ŏo�Ă����\���͖̂{���[�v�ł͎g��Ȃ�)
		reversedBoardArrange = reverseBoard_x(boardFlgList, offset_flg_turn);
		
		//////////////////////////////////////////////////////////////////////////////////////////////////
		
		
		////////////////////////////////�����[�v�̂��߂̌㏈��////////////////////////////////////////////
		
		//getArrow�֐��ŁAEnter�L�[�����������ƂŌ��Ă��t���O�����ɖ߂�
		offset_flg_turn[2] = 0;

		//�o�߃^�[�������C���N�������g
		turnNum++;

		//////////////////////////////////////////////////////////////////////////////////////////////////
	}


	/////////////////////////////////�Ֆʂ��X�V���A�����v���C���[�𔻒�///////////////////////////////////
	system("cls");

	//�J�[�\�����������Ֆʂ�\��
	printBoard(boardFlgList, 0, 0);

	//�����v���C���[����
	judgeVicotryPlayer(boardFlgList);

	//////////////////////////////////////////////////////////////////////////////////////////////////////

	return 0;
}

//�Ֆʂ�\���z��𐶐�
void makeInitBoard(int boardFlgList[BoardFlgListNum][BoardFlgListNum])
{
	//�Ֆʂ�\���z��̒[�����ׂ�EdgeNumber�ɁA����ȊO��0�ɐݒ�
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

	//�I�Z���̏����ݒu��
	boardFlgList[ColNum / 2][ColNum / 2] = WhitePieceNumber;
	boardFlgList[ColNum / 2 + 1][ColNum / 2 + 1] = WhitePieceNumber;
	boardFlgList[ColNum / 2 + 1][ColNum / 2] = BlackPieceNumber;
	boardFlgList[ColNum / 2][ColNum / 2 + 1] = BlackPieceNumber;
}

//�Ֆʐ���
void printBoard(int boardFlgList[BoardFlgListNum][BoardFlgListNum], int offset_x, int offset_y)
{
	for (int col = 0; col < ColRange + 1; col++)//�s���[�v
	{
		for (int raw = 0; raw < ColRange + 1; raw++)//�񃋁[�v
		{
			if (col % 2)
			{
				if (((float)offset_x == ((float)(col + 1) / 2)) && ((float)offset_y == ((float)(raw + 1) / 2)))//���݂̃J�[�\���̍��W
					printf(" �� ");
				else if ((raw % 2) & (boardFlgList[(col + 1) / 2][(raw + 1) / 2] == BlackPieceNumber))//��U�̋�̍��W
					printf(" �Z ");
				else if ((raw % 2) & (boardFlgList[(col + 1) / 2][(raw + 1) / 2] == WhitePieceNumber))//��U�̋�̍��W
					printf(" �� ");
				else if ((raw % 2) & (boardFlgList[(col + 1) / 2][(raw + 1) / 2] == PutAbleNumber))//���̎��̃v���C���[�����u�����Ƃ��ł���}�X
					printf(" �~ ");
				else if (raw % 2)//��u����Ă��Ȃ��}�X
					printf(" �@ ");
				else
					printf("�� ");
			}
			else if(col == 0)//�ȍ~�Ֆʂ̊O�g
			{
				if (raw == 0)
					printf("��  ");
				else if (raw == ColRange)
					printf("��");
				else if (raw % 2)
					printf("��  ");
				else
					printf("��  ");
			}
			else if (col == ColRange) 
			{
				if (raw == 0)
					printf("��  ");
				else if (raw == ColRange)
					printf("��");
				else if (raw % 2)
					printf("��  ");
				else
					printf("��  ");
			}
			else
			{
				if (raw == 0)
					printf("��  ");
				else if (raw == ColRange)
					printf("��");
				else if (raw % 2)
					printf("��  ");
				else
					printf("��  ");
			}
		}

		printf("\n\n");
	}
}

//�L�[���͔���
void getArrow(int boardFlgList[BoardFlgListNum][BoardFlgListNum], int offset_flg_turn[OffsetFlgElementNum]) {

	int flag = 1;//���L�[���͊��m�t���O
	int Arrow;
	
	//���L�[�A��������Enter�L�[���������܂Ń��[�v
	while(flag)
	{
		//�P�ɓ��͑҂������Ȃ�kbhit�֐��͂���Ȃ��H
		//�����I�Ɏ��Ԑ����@�\��t�^����ۗp���邱�Ƃ��ł��邩���H
		//if (_kbhit())
		//{
			Arrow = _getch();//���͑҂�

			if (Arrow == 0x0d)//���͂��ꂽ�L�[��Enter�̏ꍇ���[�v�𔲂���
				flag = 0;
			else if (Arrow == 0xe0)//���͂��ꂽ�L�[�����L�[�̏ꍇ�A������x�l��ǂݎ���ă��[�v�𔲂���
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
		switch (Arrow)//���͂��ꂽ�L�[�ɉ����ăJ�[�\���̋�����ω�
		{
		case 0x48://����
			offset_flg_turn[0] -= 1;
			if (boardFlgList[offset_flg_turn[0]][offset_flg_turn[1]] == EdgeNumber)
				offset_flg_turn[0] += ColNum;
			break;
		case 0x50://�����
			offset_flg_turn[0] += 1;
			if (boardFlgList[offset_flg_turn[0]][offset_flg_turn[1]] == EdgeNumber)
				offset_flg_turn[0] -= ColNum;
			break;
		case 0x4B://�����
			offset_flg_turn[1] -= 1;
			if (boardFlgList[offset_flg_turn[0]][offset_flg_turn[1]] == EdgeNumber)
				offset_flg_turn[1] += ColNum;
			break;
		case 0x4D://�E���
			offset_flg_turn[1] += 1;
			if (boardFlgList[offset_flg_turn[0]][offset_flg_turn[1]] == EdgeNumber)
				offset_flg_turn[1] -= ColNum;
			break;
		case 0x0d://Enter�L�[
			offset_flg_turn[2] = 1;
			break;
		}
}

//�v���C���[�ɔz�u����}�X�̊m�F�𑣂�
int checkchoice(int boardFlgList[BoardFlgListNum][BoardFlgListNum], int offset_flg_turn[OffsetFlgElementNum], int putAbleArrangeNum, int n_putAbleCount)
{
	int temp_boardFlgList[BoardFlgListNum][BoardFlgListNum];//���݂̔Ֆʂ̃R�s�[
	
	struct i_putAbleArrange putAbleArrange;//�m�F�p�Ֆʂ̕\���̂��߂ɁA���݂�
	
	//[0]:Enter�L�[�A�������͖��L�[�������܂őҋ@
	//[1]:�m�F���A���L�I������Enter�L�[�������܂Ń��[�v
	int LoopFlag[2];
	int Arrow;//���͂��ꂽ�L�[�̒l

	char choice[ChoiceNum][10] = { "�͂�", "������", "�m�F" };//�m�F���̑I����
	int choiceArrow = 0;//�I�����̃J�[�\���̍��W
	
	if (!(putAbleArrangeNum))//����Ђ�����Ԃ����Ƃ��ł���}�X�����݂��Ȃ��ꍇ
	{
		printf("�Ђ�����Ԃ����Ƃ��ł�������܂���B�p�X���܂��B");
		offset_flg_turn[2] = 1;
		passNum++;

		while (1)
		{
			if (_kbhit())
				break;
		}
	}
	else if (((boardFlgList[offset_flg_turn[0]][offset_flg_turn[1]] == BlackPieceNumber) | (boardFlgList[offset_flg_turn[0]][offset_flg_turn[1]] == WhitePieceNumber)) & (offset_flg_turn[2] == 1))//��̔z�u�����肵���}�X�ɂ��łɋ�u���Ă���ꍇ
	{
		printf("���łɋ�u����Ă��܂��B���̃}�X���w�肵�Ă��������B\n");
		offset_flg_turn[2] = 0;
		n_putAbleCount = 0;

		while (1)
		{
			if (_kbhit())
				break;
		}
	}
	else if ((boardFlgList[offset_flg_turn[0]][offset_flg_turn[1]] == EmpPieceNumber) & (offset_flg_turn[2] == 1))//��̔z�u�����肵���}�X�����O�̃}�X�������ꍇ
	{
		n_putAbleCount = 0;
		offset_flg_turn[2] = 0;
		printf("���̃}�X�ɂ͋��u�����Ƃ��ł��܂���B���̃}�X���w�肵�Ă��������B\n");

		while (1)
		{
			if (_kbhit())
				break;
		}
	}
	else if ((boardFlgList[offset_flg_turn[0]][offset_flg_turn[1]] == PutAbleNumber) & (offset_flg_turn[2] == 1)) {
		n_putAbleCount = 0;
		do {
			LoopFlag[0] = 1;//Enter�L�[�A�������͖��L�[�������܂őҋ@
			LoopFlag[1] = 1;//�m�F���A���L�I������Enter�L�[�������܂Ń��[�v

			//�ՖʍX�V
			system("cls");
			printBoard(boardFlgList, offset_flg_turn[0], offset_flg_turn[1]);
			printf("\n���̃}�X�ɋ��z�u���Ă�낵���ł����H\n");

			//�I�����\��
			for (int t_loop = 0; t_loop < ChoiceNum; t_loop++)
			{
				if (t_loop == 0)
					printf("�@�@");
				else
					printf("�@�@�@");

				if (choiceArrow == t_loop)
					printf("\b\b��");

				for (int t_str = 0; t_str < sizeof(choice[t_loop]) / 2; t_str++) {
					printf("%c%c", choice[t_loop][2 * t_str], choice[t_loop][2 * t_str + 1]);
				}
			}

			//Enter�L�[�A���L�[���������܂Ń��[�v
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

			//Ener�L�[���������܂őI������\����������
			switch (Arrow)
			{
			case 0x4B://�����
				choiceArrow -= 1;
				if (choiceArrow == -1)
					choiceArrow += ChoiceNum;
				break;
			case 0x4D://�E���
				choiceArrow += 1;
				if (choiceArrow == ChoiceNum)
					choiceArrow -= ChoiceNum;
				break;
			case 0x0d://Enter�L�[
				LoopFlag[1] = 0;
				break;
			}
		} while (LoopFlag[1] == 1);

		if (choiceArrow == 1)//�u�������v�I����
			offset_flg_turn[2] = 0;
		else if (choiceArrow == 2)//�u�m�F�v�I����
		{
			system("cls");

			//�Ֆʂ̃R�s�[
			//memcpy(temp_boardFlgList, boardFlgList, sizeof(boardFlgList[BoardFlgListNum][BoardFlgListNum]));//���L��z��S�̂̃T�C�Y����memcpy���g������
			memcpy(temp_boardFlgList, boardFlgList, sizeof(int) * BoardFlgListNum * BoardFlgListNum);

			//���݂̔Ֆʏ����ꎞ����
			putAbleArrange = checkPlace_x(temp_boardFlgList, offset_flg_turn[3], PutAbleNumber);//���ɃJ�[�\���̍��W�ɋ��z�u�����ۂ̔Ֆʂ��v�Z�A���̎��ύX�������W��S�ĕۑ�
			backBoard(temp_boardFlgList, putAbleArrange, EmpPieceNumber);//���̔Ֆʂ����̔Ֆʂɖ߂�
			
			//�Ђ�����Ԃ�����̔Ֆʂ�\��
			putAbleArrange = reverseBoard_x(temp_boardFlgList, offset_flg_turn);
			putAbleArrange = checkPlace_x(temp_boardFlgList, (-1)*offset_flg_turn[3], PutAbleNumber);
			printBoard(temp_boardFlgList, 0, 0);//�J�[�\�����������Ֆʂ�\��

			system("pause");

			offset_flg_turn[2] = 0;
		}
	}

	return n_putAbleCount;
}

//�Ֆʂ̂��ꂼ��̋�𐔂���
struct checkBothPieceNumber checkBothPieceNum(int boardFlgList[BoardFlgListNum][BoardFlgListNum])
{
	//int count_b = 0;
	//int count_w = 0;

	struct checkBothPieceNumber piecePairNum;

	piecePairNum.count_b = 0;//����
	piecePairNum.count_w = 0;//����

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
			//��z�u����Ă��Ȃ��}�X���΂�
			if ((col == 0) | (col == BoardFlgListNum - 1) | (raw == 0) | (raw == BoardFlgListNum - 1))
				continue;
			else if (boardFlgList[col][raw] == EmpPieceNumber)
				continue;

			//���ꂼ��̋�̃J�E���g���C���N�������g
			if (boardFlgList[col][raw] == 1)
				piecePairNum.count_b += 1;
			else if (boardFlgList[col][raw] == -1)
				piecePairNum.count_w += 1;
		}
	}

	return piecePairNum;
}

//�ՖʑS�̂̋�̌��𔒁A���ʂɍ��v���A�������̃v���C���[�������ɂ���A
void judgeVicotryPlayer(int boardFlgList[BoardFlgListNum][BoardFlgListNum])
{
	struct checkBothPieceNumber piecePairNum;
	
	piecePairNum = checkBothPieceNum(boardFlgList);//�e�v���C���[�̋�̑������J�E���g

	printf("\n\n\n");

	printf("����F%d��\n", piecePairNum.count_b);
	printf("����F%d��\n\n", piecePairNum.count_w);

	if (piecePairNum.count_b > piecePairNum.count_w)
		printf("��U�̏����I\n\n");
	else if (piecePairNum.count_b == piecePairNum.count_w)
		printf("���������I\n\n");
	else if (piecePairNum.count_b < piecePairNum.count_w)
		printf("��U�̏����I\n\n");
}

//�Ֆʏ�̋���Ђ�����Ԃ��֐�
struct i_putAbleArrange reverseBoard_x(int boardFlgList[BoardFlgListNum][BoardFlgListNum], int offset_flg_turn[OffsetFlgElementNum])
{
	struct i_putAbleArrange reversedBoardArrange;

	reversedBoardArrange.pos_Num = 0;

	//�㉺���E�΂߁A�v���������m�F
	for (int tempLoopNum = 0; tempLoopNum < CheckLoopNum; tempLoopNum++)
	{
		int check_flg = 0;//�m�F����}�X����A��������ɑ���̋�u����Ă��鎞1�ɂ���
		int count = 0;//����Ђ�����Ԃ��ہA�m�F���̍��W�Ɗm�F���̍��\����v�����Ƃ�1�ɂ���
		int ini_temp_offset[2] = { offset_flg_turn[0], offset_flg_turn[1] };//�m�F����}�X�̏������W
		int temp_offset[2] = { ini_temp_offset[0], ini_temp_offset[1] };//�������W���炸�炵����̍��W

		//��������ɍ��W�����炷
		temp_offset[0] += LoopByLoopArrange[0][tempLoopNum];
		temp_offset[1] += LoopByLoopArrange[1][tempLoopNum];

		//printf("%d\n", boardFlgList[temp_offset[0]][temp_offset[1]]);

		////���炵����ɑ���̋�z�u���Ă���ꍇ
		if (boardFlgList[temp_offset[0]][temp_offset[1]] == (-1) * offset_flg_turn[3])
		{
			//����̋�������胋�[�v
			do
			{
				temp_offset[0] += LoopByLoopArrange[0][tempLoopNum];
				temp_offset[1] += LoopByLoopArrange[1][tempLoopNum];
				check_flg = 1;
				count++;
			} while (boardFlgList[temp_offset[0]][temp_offset[1]] == (-1) * offset_flg_turn[3]);

			//���[�v���r�₦���}�X�������̋�ȊO�ł������ꍇ���[�v���΂�
			if (boardFlgList[temp_offset[0]][temp_offset[1]] != offset_flg_turn[3])
				continue;

			//���[�v���r�₦���}�X�Ɏ����̋�u����Ă���ꍇ�A���̃��[�v���Ŋm�F�����}�X�����ׂĎ����̋�ɂ���
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


//��z�u���A�Ђ�����Ԃ����Ƃ��ł���}�X���i�[
struct i_putAbleArrange checkPlace_x(int boardFlgList[BoardFlgListNum][BoardFlgListNum], int turn,int pieceNumber)
{
	struct i_putAbleArrange putAbleArrange;

	putAbleArrange.pos_Num = 0;

	//�Ֆʂ̑S�}�X���m�F
	for (int col = 0; col < BoardFlgListNum; col++)
	{
		for (int raw = 0; raw < BoardFlgListNum; raw++)
		{
			//printf("%d\n", boardFlgList[col][raw]);
			//�[�̃}�X�A�܂��͋���łɔz�u����Ă���}�X�͊m�F������΂�
			if ((boardFlgList[col][raw] == EdgeNumber) | (boardFlgList[col][raw] == BlackPieceNumber) | (boardFlgList[col][raw] == WhitePieceNumber))
				continue;

			//�㉺���E�΂߁A�v���������m�F
			for (int tempLoopNum = 0; tempLoopNum < CheckLoopNum; tempLoopNum++)
			{
				int flg = 0;//�m�F����}�X����A��������ɑ���̋�u����Ă��鎞1�ɂ���
				int temp_offset[2] = { col, raw };//�m�F����}�X�̏������W

				//��������ɍ��W�����炷
				temp_offset[0] += LoopByLoopArrange[0][tempLoopNum];
				temp_offset[1] += LoopByLoopArrange[1][tempLoopNum];

				//printf("%d, %d, %d\n", temp_offset[0], temp_offset[1], boardFlgList[temp_offset[0]][temp_offset[1]]);

				////���炵����ɑ���̋�z�u���Ă���ꍇ
				if (boardFlgList[temp_offset[0]][temp_offset[1]] == (-1) * turn)
				{
					//����̋�������胋�[�v
					do
					{
						temp_offset[0] += LoopByLoopArrange[0][tempLoopNum];
						temp_offset[1] += LoopByLoopArrange[1][tempLoopNum];
						flg = 1;
						//printf("%d\n", boardFlgList[temp_offset[0]][temp_offset[1]]);
					} while (boardFlgList[temp_offset[0]][temp_offset[1]] == (-1) * turn);

					//���[�v���r�₦���}�X���[�A�������͎����̋�̏ꍇ���[�v���΂�
					if (boardFlgList[temp_offset[0]][temp_offset[1]] != turn)
						continue;

					//���[�v���r�₦���}�X�Ɏ����̋�u����Ă���A���̐�ɋ�u����Ă��Ȃ��}�X�����݂���ꍇ�A���̃}�X��ݒu�\�ȏꏊ�Ƃ�
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

//�u�m�F�p�̃}�X�v�A�u�u������̃}�X�v�����ɖ߂�
void backBoard(int boardFlgList[BoardFlgListNum][BoardFlgListNum], struct i_putAbleArrange putAbleArrange, int pieceNumber)
{
	int papl_x;//�Ώۍ��W��x���W(putAblePlace_x)
	int papl_y;//�Ώۍ��W��y���W(putAblePlace_y)
	int papl_Num = putAbleArrange.pos_Num;//�Ώۍ��W�̌�

	for (int pieceN = 0; pieceN < papl_Num; pieceN++)
	{
		papl_x = putAbleArrange.pos_x[pieceN];
		papl_y = putAbleArrange.pos_y[pieceN];

		boardFlgList[papl_x][papl_y] = pieceNumber;
	}
}