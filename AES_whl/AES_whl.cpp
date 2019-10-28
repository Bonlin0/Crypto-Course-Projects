// AES_whl.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
// Author : Wu Haolin
// Date： 2019.10.23 18:42


#include <stdio.h>
#include <time.h>
#include <stdlib.h>

typedef unsigned char uint8;
typedef unsigned int uint32;

//表明密钥长度为Nk*4，默认是4（密钥128位），会在初始化过程中更改。
uint8 Nk = 4;
//表示迭代次数，由密钥长度确定
uint8 Nr = 10;

/*
 * 由于S盒通过造表可快速实现，此处使用造表
 * S-box transformation table
 */
static uint8 s_box[256] = {
	// 0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f
	0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, // 0
	0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, // 1
	0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, // 2
	0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, // 3
	0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, // 4
	0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, // 5
	0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, // 6
	0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, // 7
	0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, // 8
	0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, // 9
	0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, // a
	0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, // b
	0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, // c
	0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, // d
	0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, // e
	0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };// f

/*
 * Inverse S-box transformation table
 */
static uint8 inv_s_box[256] = {
	// 0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f
	0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb, // 0
	0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb, // 1
	0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e, // 2
	0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25, // 3
	0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92, // 4
	0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84, // 5
	0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06, // 6
	0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b, // 7
	0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73, // 8
	0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e, // 9
	0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b, // a
	0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4, // b
	0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f, // c
	0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef, // d
	0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61, // e
	0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };// f

// 对于128位数据块，AES永远最多只用10个轮常数 可以测试计算 Nk=4 6 8三种取值情况下需要使用到的轮常数，均不超过10.
static const uint32 rcon[] = {
	0x01000000, 0x02000000, 0x04000000, 0x08000000,
	0x10000000, 0x20000000, 0x40000000, 0x80000000,
	0x1B000000, 0x36000000, 
};

void shiftRow(uint8* state)
{
	uint8 tempByte = 0;
	//第一行不移位
	//第二行循环左移1个字节
	tempByte = state[1];
	state[1] = state[5];
	state[5] = state[9];
	state[9] = state[13];
	state[13] = tempByte;
	//第三行循环左移2个字节
	tempByte = state[2];
	state[2] = state[10];
	state[10] = tempByte;
	tempByte = state[6];
	state[6] = state[14];
	state[14] = tempByte;
	//第四行循环左移3个字节
	tempByte = state[3];
	state[3] = state[15];
	state[15] = state[11];
	state[11] = state[7];
	state[7] = tempByte;
}

void invShiftRow(uint8* state)
{
	uint8 tempByte = 0;
	//第一行不移位
	//第二行循环右移1个字节
	tempByte = state[1];
	state[1] = state[13];
	state[13] = state[9];
	state[9] = state[5];
	state[5] = tempByte;
	//第三行循环左移2个字节 == 右移2个字节 不用修改
	tempByte = state[2];
	state[2] = state[10];
	state[10] = tempByte;
	tempByte = state[6];
	state[6] = state[14];
	state[14] = tempByte;
	//第四行循环左移1个字节
	tempByte = state[3];
	state[3] = state[7];
	state[7] = state[11];
	state[11] = state[15];
	state[15] = tempByte;
}

// 在GF（2^8）上的乘法运算
uint8 byteMult(uint8 a, uint8 b) {

	uint8 result = 0, i = 0, hbs = 0;

	// 对b从低到高每一位做处理，由于每高一位都会使得与a整个多项式相乘结果多乘一个x，所以对a进行移位，如果移位使得多项式超过x的八次方，
	// 模一个模多项式，也就是去掉最高位的x的八次方，然后加上0x1b，也就是x的4次方+x的3次+1.
	for (i = 0; i < 8; i++) {
		if (b & 1) {
			result ^= a;
		}
		// 其实hbs就是判断a的最高位是不是1，如果是以要及时做处理（模多项式）
		hbs = a & 0x80;
		a <<= 1;
		if (hbs) a ^= 0x1b; // 0000 0001 0001 1011	
		b >>= 1;
	}

	return (uint8)result;
}

//列混淆，对每个列做混淆，混淆是乘多项式，但是由于多项式固定，可以直接调用byteMult计算结果。
void mixColumn(uint8* state)
{
	//临时变量存储列
	uint8 tempColumn[4];
	//遍历每个列
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			tempColumn[j] = state[4 * i + j];
		// 公式参见武汉大学出版社密码学引论93面最上方 之所以直接用亦或是因为模多项式是x的四次方+1 
		state[4 * i] = byteMult(tempColumn[0], 0x02) ^ byteMult(tempColumn[1], 0x03) ^ byteMult(tempColumn[2], 0x01) ^ byteMult(tempColumn[3], 0x01);
		state[4 * i + 1] = byteMult(tempColumn[0], 0x01) ^ byteMult(tempColumn[1], 0x02) ^ byteMult(tempColumn[2], 0x03) ^ byteMult(tempColumn[3], 0x01);
		state[4 * i + 2] = byteMult(tempColumn[0], 0x01) ^ byteMult(tempColumn[1], 0x01) ^ byteMult(tempColumn[2], 0x02) ^ byteMult(tempColumn[3], 0x03);
		state[4 * i + 3] = byteMult(tempColumn[0], 0x03) ^ byteMult(tempColumn[1], 0x01) ^ byteMult(tempColumn[2], 0x01) ^ byteMult(tempColumn[3], 0x02);
	}
}

//逆列混淆，对每个列做混淆，混淆是乘多项式，但是由于多项式固定，可以直接调用byteMult计算结果。
void invMixColumn(uint8* state)
{
	//临时变量存储列
	uint8 tempColumn[4];
	//遍历每个列
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			tempColumn[j] = state[4 * i + j];
		// 公式参见武汉大学出版社密码学引论93面最上方 之所以直接用亦或是因为模多项式是x的四次方+1 
		state[4 * i] = byteMult(tempColumn[0], 0x0E) ^ byteMult(tempColumn[1], 0x0B) ^ byteMult(tempColumn[2], 0x0D) ^ byteMult(tempColumn[3], 0x09);
		state[4 * i + 1] = byteMult(tempColumn[0], 0x09) ^ byteMult(tempColumn[1], 0x0E) ^ byteMult(tempColumn[2], 0x0B) ^ byteMult(tempColumn[3], 0x0D);
		state[4 * i + 2] = byteMult(tempColumn[0], 0x0D) ^ byteMult(tempColumn[1], 0x09) ^ byteMult(tempColumn[2], 0x0E) ^ byteMult(tempColumn[3], 0x0B);
		state[4 * i + 3] = byteMult(tempColumn[0], 0x0B) ^ byteMult(tempColumn[1], 0x0D) ^ byteMult(tempColumn[2], 0x09) ^ byteMult(tempColumn[3], 0x0E);
	}
}

//  S 盒替换
void ByteSub(uint8* status)
{
	uint8 temp = 0;
	for (int i = 0; i < 16; i++) {
		temp = status[i];
		status[i] = s_box[temp];
	}
}

//  逆 S 盒替换
void invByteSub(uint8* status)
{
	uint8 temp = 0;
	for (int i = 0; i < 16; i++) {
		temp = status[i];
		status[i] = inv_s_box[temp];
	}
}

//密钥扩展 
void keyExpansion(uint32* masterKey, uint32* W)
{
	// 严格按照教材87面下方的伪代码编写
	for (int i = 0; i < Nk; i++)
	{
		W[i] = masterKey[i];
	}
	for (uint8 i = Nk; i < 4 * (Nr + 1); i++)
	{
		uint32 temp = W[i - 1];
		if (i % Nk == 0)
		{
			//这时书上的代码写的很简略，但是由于前面的实现我都是直接编码没有提出函数（比如ByteSub)，所以这里编写就要费工夫了
			// 由于我之前一直再用uint32, 先转回uint8
			uint8 tmpBytes[4] = { 0 };
			tmpBytes[0] = temp % 256;
			tmpBytes[1] = (temp >> 8) % 256;
			tmpBytes[2] = (temp >> 16) % 256;
			tmpBytes[3] = (temp >> 24) % 256;
			// 先做Rotl处理
			uint8 swapTemp = tmpBytes[3];
			tmpBytes[3] = tmpBytes[2];
			tmpBytes[2] = tmpBytes[1];
			tmpBytes[1] = tmpBytes[0];
			tmpBytes[0] = swapTemp;
			// 在做字节替换
			for (int j = 0; j < 4; j++)
			{
				uint8 subTempByte = tmpBytes[j];
				tmpBytes[j] = s_box[subTempByte];
			}
			// 由字节回复出整数 似乎也不用考虑大小端
			uint32 tempIntFromBytes = (uint32)tmpBytes[0] ^ ((uint32)tmpBytes[1]) << 8 ^ ((uint32)tmpBytes[2]) << 16 ^ ((uint32)tmpBytes[3]) << 24;
			temp = tempIntFromBytes ^ rcon[i / Nk - 1];
			
		}
		else
		{
			//Nk = 8 在 i模Nk=4时做处理 复用了上面的代码，只做S盒替换
			if (Nk > 6)
			{
				// 由于我之前一直再用uint32, 先转回uint8
				uint8 tmpBytes[4] = { 0 };
				tmpBytes[0] = temp % 256;
				tmpBytes[1] = (temp >> 8) % 256;
				tmpBytes[2] = (temp >> 16) % 256;
				tmpBytes[3] = (temp >> 24) % 256;
				// 在做字节替换
				for (int j = 0; j < 4; j++)
				{
					uint8 subTempByte = tmpBytes[j];
					tmpBytes[j] = s_box[subTempByte];
				}
				// 由字节回复出整数 似不用考虑大小端
				temp =  (uint32)tmpBytes[0] + ((uint32)tmpBytes[1]) << 8 + ((uint32)tmpBytes[2]) << 16 + ((uint32)tmpBytes[3]) << 24;
			}
		}
		W[i] = W[i - Nk] ^ temp;
	}
	
}

void addRoundKey(uint8* state, uint32* W, uint8 round)
{
	//因为我的扩展密钥是32位为单位存储的，所以要先把W转成字节处理
	uint8 tmpBytes[4] = { 0 };
	
	for (int i = 0; i < 4; i++)
	{
		//不考虑大小端 单独考虑 word的第一个字节就是Most significant byte 所以这里与前面的代码并不相同
		uint32 temp = W[4 * round + i];
		tmpBytes[3] = temp % 256;
		tmpBytes[2] = (temp >> 8) % 256;
		tmpBytes[1] = (temp >> 16) % 256;
		tmpBytes[0] = (temp >> 24) % 256; 

		state[i * 4] = state[i * 4] ^ tmpBytes[0];
		state[i * 4 + 1] = state[i * 4 + 1] ^ tmpBytes[1];
		state[i * 4 + 2] = state[i * 4 + 2] ^ tmpBytes[2];
		state[i * 4 + 3] = state[i * 4 + 3] ^ tmpBytes[3];
	}
}

void AES_block_enc(uint8* state, uint32* expandedKey)
{
	//加密 初始轮密钥加
	addRoundKey(state, expandedKey, 0);
	// Nr-1 轮迭代
	for (int r = 1; r < Nr; r++) {
		ByteSub(state);
		shiftRow(state);
		mixColumn(state);
		addRoundKey(state, expandedKey, r);
	}
	//最后一轮没有mixColumn
	ByteSub(state);
	shiftRow(state);
	addRoundKey(state, expandedKey, Nr);
	//输出测试
	//for (int i = 0; i < 16; i++)
	//{
	//	printf("%x", state[i]);
	//	if (i % 4 == 3)
	//		printf("\n");
	//}
}

void AES_block_dec(uint8* state, uint32* expandedKey)
{
	//解密
	addRoundKey(state, expandedKey, Nr);
	// Nr-1 轮迭代
	for (int r = Nr - 1; r >= 1; r--) {
		invShiftRow(state);
		invByteSub(state);
		addRoundKey(state, expandedKey, r);
		invMixColumn(state);

	}
	//最后一轮没有mixColumn
	invShiftRow(state);
	invByteSub(state);
	addRoundKey(state, expandedKey, 0);

	//for (int i = 0; i < 16; i++)
	//{
	//	printf("%x", state[i]);
	//	if (i % 4 == 3)
	//		printf("\n");
	//}
}

void generateKeyFile(void)
{
	//生成密钥文件
	FILE* keyFile = NULL;
	fopen_s(&keyFile, "AES_keyfile.key", "wb");
	//打开失败直接退出
	if (keyFile == NULL)
		exit(1);
	time_t t;
	srand((unsigned)time(&t));
	printf("Please choose the length of Key.\n 1.128 bits\t2.192 bits\t3.256 bits\n");
	int input = 0;
	scanf_s("%d", &input);
	while (input != 1 && input != 2 && input != 3)
	{
		printf("Your input is invalid. Try again.\n");
		scanf_s("%d", &input);
	}
	int keyBytes = 0;
	switch (input)
	{
	case 1:
		keyBytes = 16;
		break;
	case 2:
		keyBytes = 24;
		break;
	case 3:
		keyBytes = 32;
		break;
	}
	for (int i = 0; i < keyBytes; i++)
	{
		uint8 charToBeWritten = rand() % 256;
		fwrite(&charToBeWritten, 1, 1, keyFile);
	}
	fclose(keyFile);
}

// mode 为 1 加密， mode 为 0解密
void AES_UI(int mode)
{
	if(mode)
		//加密文件
		printf("Please enter the name of the file to be encrypted.(Less than 256 bytes)\n");
	else
		//解密文件
		printf("Please enter the name of the file to be decrypted.(Less than 256 bytes)\n");


	char fileName[256] = "";
	scanf_s("%s", fileName, sizeof(fileName));
	FILE* keyFile = NULL;
	fopen_s(&keyFile, "AES_keyfile.key", "rb");
	FILE* inputFile = NULL;
	
	fopen_s(&inputFile, fileName, "rb");
	FILE* outputFile = NULL;

	if(mode)
		fopen_s(&outputFile, "Cypher.enc", "wb");
	else
		fopen_s(&outputFile, "Plaintext.dec", "wb");

	//如果文件打开失败退出
	if (keyFile == NULL || inputFile == NULL || outputFile == NULL)
	{
		printf("There is something wrong in opening your file.");
		exit(1);
	}
	//临时变量用于存储取出的word
	uint32 tempWord = 0;
	int counter = 0;
	uint32 MasterKey[8] = { 0 };
	while (fread_s(&tempWord, 4, 4, 1, keyFile) == 1)
	{
		//如果Keyfile大于256bits退出
		if (counter == 8)
		{
			printf("There is something wrong about your key file.\n");
			exit(1);
		}
		MasterKey[counter] = tempWord;
		counter++;
	}
	//如果读取出来的字数目不对退出
	if (counter != 4 && counter != 6 && counter != 8)
	{
		printf("There is something wrong about your key file.\n");
		exit(1);
	}
	switch (counter)
	{
	case 4:
		Nk = 4;
		Nr = 10;
		break;
	case 6:
		Nk = 6;
		Nr = 12;
		break;
	case 8:
		Nk = 8;
		Nr = 14;
		break;
	}
	// 配置完毕开始加密
	uint32 expandedKey[60] = { 0 };
	keyExpansion(MasterKey, expandedKey);
	// TODO 先假设都做过padding了
	counter = 0;
	uint8 state[16] = { 0 };
	uint8 tempByte = 0;
	while (fread_s(&tempByte, 1, 1, 1, inputFile) == 1)
	{
		//读够一块
		if (counter == 16)
		{

			if(mode)
				AES_block_enc(state, expandedKey);
			else
				AES_block_dec(state, expandedKey);

			size_t writeNum = fwrite(state, 1, 16, outputFile);
			if (writeNum != 16)
			{
				printf("There is something wrong in writing the output.\n");
				exit(1);
			}
			counter -= 16;
		}
		state[counter] = tempByte;
		counter++;
	}
	// 由于当正好读完16个字节的最后一个字节后上述的循环就会结束 所以要在下面再次判断
	// 注意到 加密时要考虑padding，如果加密到这里 counter=16说明源文件正好是16字节的倍数
	// padding 方案 最后一个字节指示明文字节数  如果正好16个字节 如果是加密检查最后一个字节是否是0x00-0x0f

	if (counter == 16)
	{
		if (mode)
		{ 
			//加密，检查最后一个字节是不是小于等于0x0f
			if (state[15] <= 0x0f)
			{
				// 先加密这个快并且写入，然后新加密一个00...00的块
				AES_block_enc(state, expandedKey);
				size_t writeNum = fwrite(state, 1, 16, outputFile);
				if (writeNum != 16)
				{
					printf("There is something wrong in writing the output.\n");
					exit(1);
				}
				for (int i = 0; i < 16; i++)
				{
					state[i] = 0x00;
				}
			}
			AES_block_enc(state, expandedKey);
			size_t writeNum = fwrite(state, 1, 16, outputFile);
			if (writeNum != 16)
			{
				printf("There is something wrong in writing the output.\n");
				exit(1);
			}
		}
		//解密
		else
		{
			AES_block_dec(state, expandedKey);
			// 判断最后一个字节是否小于等于0f，如果是说明这个分组有padding。
			//声明计数器计算padding数目
			int messageBytesCounter = (int)state[15];
			
			// 如果padding Counter == 0 ，说明整个分组都是填充，所以干脆跳过，避免因为不写东西导致函数崩溃
			if (messageBytesCounter != 0)
			{
				size_t writeNum = fwrite(state, 1, messageBytesCounter, outputFile);
				if (writeNum != messageBytesCounter)
				{
					printf("There is something wrong in writing the output.\n");
					exit(1);
				}
			}

		}

	}
	//说明块数不到16，只有可能是加密，如果是解密要报错。加密要将后面按规则填充。
	else
	{
		// 解密 说明文件有错误
		if (!mode)
		{
			printf("There is something Wrong about your input file. It is not a valid Cypher.");
			exit(1);
		}
		// 加密
		else
		{
			// counter的数字恰好为该分组明文字节数
			state[15] = counter;
			while (counter < 15)
			{
				state[counter] = 0x00;
				counter++;
			}
			AES_block_enc(state, expandedKey);
			// 写入密文
			size_t writeNum = fwrite(state, 1, 16, outputFile);
			if (writeNum != 16)
			{
				printf("There is something wrong in writing the output.\n");
				exit(1);
			}
		}
	}

	//结束
	fclose(keyFile);
	fclose(outputFile);
	fclose(inputFile);
	return;
}

int main()
{
	/*uint32 key[4] = { 0x00012001, 0x710198ae, 0xda791714, 0x60153594 };
	uint8 state[16] = { 0x00, 0x01, 0x00, 0x01, 0x01, 0xa1, 0x98, 0xaf, 0xda, 0x78, 0x17, 0x34, 0x86, 0x15, 0x35, 0x66 };
	uint32 expandedKey[44] = { 0 };
	keyExpansion(key, expandedKey);*/
	printf("This is a simple AES tool implemented by Wu Haolin.\nThe tool will use \"AES_keyfile.key\" as default. \nIf you don't have one now, Please Generate one first.\nPlease enter a number to choose the function you need.\n1. Generate Key randomly.\n2. Encrypt a file.\n3. Decrypt a file.\n4. Exit\n");
	int input = 1;
	int keepLiveFlag = 1;
	//一直循环
	while (keepLiveFlag)
	{
		scanf_s("%d", &input);
		while (input > 4 || input < 1)
		{
			printf("Your input is invalid. Try again.\n");
			scanf_s("%d", &input);
		}
		long begin, end = 0;
		begin = clock();
		switch (input)
		{
		case 1:
			generateKeyFile();
			break;
		case 2:
			AES_UI(1);
			break;
		case 3:
			AES_UI(0);
			break;
		case 4:
			keepLiveFlag = 0;
			break;
		}
		end = clock();
		printf("It takes %lds to finish the work.\n Please enter the next number.\n", (end - begin) / CLOCKS_PER_SEC);
	}
	system("PAUSE");
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
