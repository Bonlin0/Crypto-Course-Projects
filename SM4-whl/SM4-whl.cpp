// SM4-whl.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
// Author : Wu haolin
// Date: 2019/11/2 19:45 

#include <stdio.h>
#include <time.h>
#include <stdlib.h>


typedef unsigned long long int u64;
typedef unsigned int u32;
typedef unsigned char u8;

static const unsigned char SboxTable[16][16] =
{
	{0xd6, 0x90, 0xe9, 0xfe, 0xcc, 0xe1, 0x3d, 0xb7, 0x16, 0xb6, 0x14, 0xc2, 0x28, 0xfb, 0x2c, 0x05},
	{0x2b, 0x67, 0x9a, 0x76, 0x2a, 0xbe, 0x04, 0xc3, 0xaa, 0x44, 0x13, 0x26, 0x49, 0x86, 0x06, 0x99},
	{0x9c, 0x42, 0x50, 0xf4, 0x91, 0xef, 0x98, 0x7a, 0x33, 0x54, 0x0b, 0x43, 0xed, 0xcf, 0xac, 0x62},
	{0xe4, 0xb3, 0x1c, 0xa9, 0xc9, 0x08, 0xe8, 0x95, 0x80, 0xdf, 0x94, 0xfa, 0x75, 0x8f, 0x3f, 0xa6},
	{0x47, 0x07, 0xa7, 0xfc, 0xf3, 0x73, 0x17, 0xba, 0x83, 0x59, 0x3c, 0x19, 0xe6, 0x85, 0x4f, 0xa8},
	{0x68, 0x6b, 0x81, 0xb2, 0x71, 0x64, 0xda, 0x8b, 0xf8, 0xeb, 0x0f, 0x4b, 0x70, 0x56, 0x9d, 0x35},
	{0x1e, 0x24, 0x0e, 0x5e, 0x63, 0x58, 0xd1, 0xa2, 0x25, 0x22, 0x7c, 0x3b, 0x01, 0x21, 0x78, 0x87},
	{0xd4, 0x00, 0x46, 0x57, 0x9f, 0xd3, 0x27, 0x52, 0x4c, 0x36, 0x02, 0xe7, 0xa0, 0xc4, 0xc8, 0x9e},
	{0xea, 0xbf, 0x8a, 0xd2, 0x40, 0xc7, 0x38, 0xb5, 0xa3, 0xf7, 0xf2, 0xce, 0xf9, 0x61, 0x15, 0xa1},
	{0xe0, 0xae, 0x5d, 0xa4, 0x9b, 0x34, 0x1a, 0x55, 0xad, 0x93, 0x32, 0x30, 0xf5, 0x8c, 0xb1, 0xe3},
	{0x1d, 0xf6, 0xe2, 0x2e, 0x82, 0x66, 0xca, 0x60, 0xc0, 0x29, 0x23, 0xab, 0x0d, 0x53, 0x4e, 0x6f},
	{0xd5, 0xdb, 0x37, 0x45, 0xde, 0xfd, 0x8e, 0x2f, 0x03, 0xff, 0x6a, 0x72, 0x6d, 0x6c, 0x5b, 0x51},
	{0x8d, 0x1b, 0xaf, 0x92, 0xbb, 0xdd, 0xbc, 0x7f, 0x11, 0xd9, 0x5c, 0x41, 0x1f, 0x10, 0x5a, 0xd8},
	{0x0a, 0xc1, 0x31, 0x88, 0xa5, 0xcd, 0x7b, 0xbd, 0x2d, 0x74, 0xd0, 0x12, 0xb8, 0xe5, 0xb4, 0xb0},
	{0x89, 0x69, 0x97, 0x4a, 0x0c, 0x96, 0x77, 0x7e, 0x65, 0xb9, 0xf1, 0x09, 0xc5, 0x6e, 0xc6, 0x84},
	{0x18, 0xf0, 0x7d, 0xec, 0x3a, 0xdc, 0x4d, 0x20, 0x79, 0xee, 0x5f, 0x3e, 0xd7, 0xcb, 0x39, 0x48}
};


/* System parameter */
static const unsigned long FK[4] = { 0xa3b1bac6, 0x56aa3350, 0x677d9197, 0xb27022dc };

/* fixed parameter */
static const unsigned long CK[32] =
{
	0x00070e15, 0x1c232a31, 0x383f464d, 0x545b6269,
	0x70777e85, 0x8c939aa1, 0xa8afb6bd, 0xc4cbd2d9,
	0xe0e7eef5, 0xfc030a11, 0x181f262d, 0x343b4249,
	0x50575e65, 0x6c737a81, 0x888f969d, 0xa4abb2b9,
	0xc0c7ced5, 0xdce3eaf1, 0xf8ff060d, 0x141b2229,
	0x30373e45, 0x4c535a61, 0x686f767d, 0x848b9299,
	0xa0a7aeb5, 0xbcc3cad1, 0xd8dfe6ed, 0xf4fb0209,
	0x10171e25, 0x2c333a41, 0x484f565d, 0x646b7279
};


// 循环左移 a是源操作数 b是左移位数
u32 cycleShiftLeft(u32 a, u32 b)
{
	b %= 32;
	u32 tmp = a >> (32 - b);
	return (a << b) ^ tmp;
}

//// 循环右移 a是源操作数 b是移位数
//u32 cycleShiftRight(u32 a, u32 b)
//{
//	b %= 32;
//	u32 tmp = a << (32 - b);
//	return (a >> b) ^ tmp;
//}

// 非线性变换t
u32 nonlinear_transform(u32 a)
{
	u32 result = 0;
	u8 tmp = a >> 24;
	result += SboxTable[tmp / 16][tmp % 16];
	result <<= 8;
	tmp = a >> 16;
	result += SboxTable[tmp / 16][tmp % 16];
	result <<= 8;
	tmp = a >> 8;
	result += SboxTable[tmp / 16][tmp % 16];
	result <<= 8;
	tmp = a;
	result += SboxTable[tmp / 16][tmp % 16];
	return result;
}

// 线性变换L
u32 inline L(u32 a)
{
	return a ^ cycleShiftLeft(a, 2) ^ cycleShiftLeft(a, 10) ^ cycleShiftLeft(a, 18) ^ cycleShiftLeft(a, 24);
}

//合成变换 T
u32 inline T(u32 a)
{
	return L(nonlinear_transform(a));
}

// 线性变换L-密钥扩展
u32 inline L_key(u32 a)
{
	return a ^ cycleShiftLeft(a, 13) ^ cycleShiftLeft(a, 23);
}

//合成变换 T-密钥扩展
u32 inline T_key(u32 a)
{
	return L_key(nonlinear_transform(a));
}


// 输入master Key 4*32bits ， K 36 * 32bits， 输出rk的基地址（其实就是K+4）
u32* keyExpansion(u32* masterKey, u32* K)
{
	K[0] = masterKey[0] ^ FK[0];
	K[1] = masterKey[1] ^ FK[1];
	K[2] = masterKey[2] ^ FK[2];
	K[3] = masterKey[3] ^ FK[3];
	for (int i = 0; i < 32; i++)
	{
		K[i + 4] = K[i] ^ T_key(K[i + 1] ^ K[i + 2] ^ K[i + 3] ^ CK[i]);
	}
	return K + 4;
}

// 输入in 4个word rk 32个word 输出 out4个wod
void SM4_block_enc(u32 *in, u32 *out,u32 *rk)
{
	u32 X[36] = { 0 };
	X[0] = in[0];
	X[1] = in[1];
	X[2] = in[2];
	X[3] = in[3];
	for (int i = 0; i < 32; i++)
	{
		X[i + 4] = X[i] ^ T(X[i + 1] ^ X[i + 2] ^ X[i + 3] ^ rk[i]);
	}
	out[0] = X[35];
	out[1] = X[34];
	out[2] = X[33];
	out[3] = X[32];
}


void generateKeyFile(void)
{
	//生成密钥文件
	FILE* keyFile = NULL;
	fopen_s(&keyFile, "SM4_keyfile.key", "wb");
	//打开失败直接退出
	if (keyFile == NULL)
		exit(1);
	time_t t;
	srand((unsigned)time(&t));

	for (int i = 0; i < 16; i++)
	{
		u8 charToBeWritten = rand() % 256;
		fwrite(&charToBeWritten, 1, 1, keyFile);
	}
	fclose(keyFile);
	printf("Your key file has been setup successfully.\n");
}

// 输入in 4个word rk 32个word 输出 out4个wod
void SM4_block_dec(u32* in, u32* out, u32* rk)
{
	u32 X[36] = { 0 };
	X[0] = in[0];
	X[1] = in[1];
	X[2] = in[2];
	X[3] = in[3];
	for (int i = 0; i < 32; i++)
	{
		X[i + 4] = X[i] ^ T(X[i + 1] ^ X[i + 2] ^ X[i + 3] ^ rk[31 - i]);
	}
	out[0] = X[35];
	out[1] = X[34];
	out[2] = X[33];
	out[3] = X[32];
}

// 将16个字节转成4个word
void bytes16_to_word4(u8* in, u32* out)
{
	out[0] = (u32)in[0] << 24 ^ (u32)in[1] << 16 ^ (u32)in[2] << 8 ^ (u32)in[3];
	out[1] = (u32)in[4] << 24 ^ (u32)in[5] << 16 ^ (u32)in[6] << 8 ^ (u32)in[7];
	out[2] = (u32)in[8] << 24 ^ (u32)in[9] << 16 ^ (u32)in[10] << 8 ^ (u32)in[11];
	out[3] = (u32)in[12] << 24 ^ (u32)in[13] << 16 ^ (u32)in[14] << 8 ^ (u32)in[15];
	return;
}

// 将4个word转成16个字节
void word4_to_bytes16(u32* in, u8* out)
{
	out[0] = in[0] >> 24;
	out[1] = in[0] >> 16;
	out[2] = in[0] >> 8;
	out[3] = in[0];
	out[4] = in[1] >> 24;
	out[5] = in[1] >> 16;
	out[6] = in[1] >> 8;
	out[7] = in[1];
	out[8] = in[2] >> 24;
	out[9] = in[2] >> 16;
	out[10] = in[2] >> 8;
	out[11] = in[2];
	out[12] = in[3] >> 24;
	out[13] = in[3] >> 16;
	out[14] = in[3] >> 8;
	out[15] = in[3];
	return;
}


//将4个32bit的信息写入file的函数
void write_u32_4(FILE* file, u32* msg)
{
	u8 bytes16_write[16] = { 0 };
	for (int i = 0; i < 4; i++)
	{
		bytes16_write[i * 4] = (u8)(msg[i] >> 24);
		bytes16_write[i * 4 + 1] = (u8)(msg[i] >> 16);
		bytes16_write[i * 4 + 2] = (u8)(msg[i] >> 8);
		bytes16_write[i * 4 + 3] = (u8)(msg[i]);
	}
	size_t writeNum = fwrite(bytes16_write, 1, 16, file);
	if (writeNum != 16)
	{
		printf("There is something wrong in writing the output.\n");
		exit(1);
	}
}


// mode 为 1 加密， mode 为 0解密
void SM4_UI(int mode)
{
	if (mode)
		//加密文件
		printf("Please enter the name of the file to be encrypted.(Less than 256 bytes)\n");
	else
		//解密文件
		printf("Please enter the name of the file to be decrypted.(Less than 256 bytes)\n");


	char fileName[256] = "";
	scanf_s("%s", fileName, sizeof(fileName));
	FILE* keyFile = NULL;
	fopen_s(&keyFile, "SM4_keyfile.key", "rb");
	FILE* inputFile = NULL;

	fopen_s(&inputFile, fileName, "rb");
	FILE* outputFile = NULL;

	if (mode)
		fopen_s(&outputFile, "Cypher.enc", "wb");
	else
		fopen_s(&outputFile, "Plaintext.dec", "wb");

	//如果文件打开失败退出
	if (keyFile == NULL || inputFile == NULL || outputFile == NULL)
	{
		printf("There is something wrong in opening your file.");
		exit(1);
	}

	//临时变量用于存储取出的word(32bit)
	u32 tempWord = 0;
	int counter = 0;
	u32 MasterKey[4] = { 0 };
	while (fread_s(&tempWord, 4, 4, 1, keyFile) == 1)
	{
		//如果Keyfile大于128Bits退出
		if (counter == 4)
		{
			printf("There is something wrong about your key file.\n");
			exit(1);
		}
		MasterKey[counter] = tempWord;
		counter++;
	}
	//如果读取出来的字数目不对退出
	if (counter != 4)
	{
		printf("There is something wrong about your key file.\n");
		exit(1);
	}

	//密钥扩展
	u32 K[36] = { 0 };
	u32* rk = keyExpansion(MasterKey, K);


	u32 msg_in[4] = { 0 };
	u32 msg_out[4] = { 0 };

	counter = 0;
	u8 bytes_16[16] = { 0 };
	u8 tempByte = 0;

	while (fread_s(&tempByte, 1, 1, 1, inputFile) == 1)
	{
		//读够一块
		if (counter == 16)
		{
			//从读出的字节回复出u16以送入加密解密函数
			bytes16_to_word4(bytes_16, msg_in);

			if (mode)
				SM4_block_enc(msg_in, msg_out,rk);
			else
				SM4_block_dec(msg_in, msg_out, rk);

			write_u32_4(outputFile, msg_out);

			counter -= 16;
		}
		bytes_16[counter] = tempByte;
		counter++;
	}
	// 由于当正好读完16个字节的最后一个字节后上述的循环就会结束 所以要在下面再次判断
	// 注意到 加密时要考虑padding，如果加密到这里 counter=16说明源文件正好是16字节的倍数
	// padding 方案 最后一个字节指示明文字节数  如果正好16个字节 如果是加密检查最后一个字节是否是0x00-0x0f 是的话多加一块空白块

	if (counter == 16)
	{
		//从读出的字节回复出u16以送入加密解密函数
		bytes16_to_word4(bytes_16, msg_in);
		if (mode)
		{
			//加密，检查最后一个字节是不是小于等于0x0f
			if (bytes_16[15] <= 0x0f)
			{
				SM4_block_enc(msg_in, msg_out,rk);
				write_u32_4(outputFile, msg_out);

				for (int i = 0; i < 4; i++)
				{
					msg_in[i] = 0x00000000;
				}
			}
			SM4_block_enc(msg_in, msg_out, rk);
			write_u32_4(outputFile, msg_out);
		}
		//解密
		else
		{
			SM4_block_dec(msg_in, msg_out, rk);
			// 判断最后一个字节是否小于等于0f，如果是说明这个分组有padding。
			if (msg_out[3] % 256 <= 0x0f)
			{
				//声明计数器计算padding数目
				word4_to_bytes16(msg_out, bytes_16);
				int messageBytesCounter = (int)bytes_16[15];

				// 如果padding Counter == 0 ，说明整个分组都是填充，所以干脆跳过，避免因为不写东西导致函数崩溃
				if (messageBytesCounter != 0)
				{
					size_t writeNum = fwrite(bytes_16, 1, messageBytesCounter, outputFile);
					if (writeNum != messageBytesCounter)
					{
						printf("There is something wrong in writing the output.\n");
						exit(1);
					}
				}
			}
			else
				write_u32_4(outputFile, msg_out);

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
			bytes_16[15] = counter;
			while (counter < 15)
			{
				bytes_16[counter] = 0x00;
				counter++;
			}

			bytes16_to_word4(bytes_16, msg_in);
			SM4_block_enc(msg_in, msg_out, rk);
			// 写入密文
			write_u32_4(outputFile, msg_out);
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
	/*u16 msg[4] = { 0,1,2,3 };
	u64 key[2] = { 0x0001000200030004,0x0005000600070008 };*/
	printf("This is a simple SM4 tool implemented by Wu Haolin.\nThe tool will use \"SM4_keyfile.key\" as default. \nIf you don't have one now, Please Generate one first.\nPlease enter a number to choose the function you need.\n1. Generate Key randomly.\n2. Encrypt a file.\n3. Decrypt a file.\n4. Exit\n");
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
		long long int begin, end = 0;
		begin = time(NULL);
		switch (input)
		{
		case 1:
			generateKeyFile();
			break;
		case 2:
			SM4_UI(1);
			break;
		case 3:
			SM4_UI(0);
			break;
		case 4:
			keepLiveFlag = 0;
			break;
		}
		end = time(NULL);
		printf("It takes %f s to finish the work.\n Please enter the next number.\n", (double)(end - begin));
	}
	system("PAUSE");
	return 0;
}

//int main()
//{
//	u32 msg[4] = { 0x01234567,0x89abcdef,0xfedcba98,0x76543210 };
//	u32 masterkey[4] = { 0x01234567,0x89abcdef,0xfedcba98,0x76543210 };
//	u32 K[36] = { 0 };
//	u32* rk;
//	rk = keyExpansion(masterkey, K);
//	u32 cypher[4] = { 0 };
//	u32 dec[4] = { 0 };
//
//	SM4_block_enc(msg, cypher, rk);
//	SM4_block_dec(cypher, dec, rk);
//	return 0;
//}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
