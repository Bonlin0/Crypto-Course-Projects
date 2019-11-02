// DES-whl.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
// Author : Wu haolin
// Date: 2019/11/2 13:20 


#include <stdio.h>
#include <time.h>
#include <stdlib.h>


typedef unsigned long long int u64;
typedef unsigned char u8;

int initial_key_permutaion[] = { 57, 49,  41, 33,  25,  17,  9,
								 1, 58,  50, 42,  34,  26, 18,
								10,  2,  59, 51,  43,  35, 27,
								19, 11,   3, 60,  52,  44, 36,
								63, 55,  47, 39,  31,  23, 15,
								 7, 62,  54, 46,  38,  30, 22,
								14,  6,  61, 53,  45,  37, 29,
								21, 13,   5, 28,  20,  12,  4 };

int key_shift_sizes[] = {  1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1 };

int sub_key_permutation[] = { 14, 17, 11, 24,  1,  5,
								 3, 28, 15,  6, 21, 10,
								23, 19, 12,  4, 26,  8,
								16,  7, 27, 20, 13,  2,
								41, 52, 31, 37, 47, 55,
								30, 40, 51, 45, 33, 48,
								44, 49, 39, 56, 34, 53,
								46, 42, 50, 36, 29, 32 };

int initial_message_permutation[] = { 58, 50, 42, 34, 26, 18, 10, 2,
										60, 52, 44, 36, 28, 20, 12, 4,
										62, 54, 46, 38, 30, 22, 14, 6,
										64, 56, 48, 40, 32, 24, 16, 8,
										57, 49, 41, 33, 25, 17,  9, 1,
										59, 51, 43, 35, 27, 19, 11, 3,
										61, 53, 45, 37, 29, 21, 13, 5,
										63, 55, 47, 39, 31, 23, 15, 7 };

int final_message_permutation[] = { 40,  8, 48, 16, 56, 24, 64, 32,
									39,  7, 47, 15, 55, 23, 63, 31,
									38,  6, 46, 14, 54, 22, 62, 30,
									37,  5, 45, 13, 53, 21, 61, 29,
									36,  4, 44, 12, 52, 20, 60, 28,
									35,  3, 43, 11, 51, 19, 59, 27,
									34,  2, 42, 10, 50, 18, 58, 26,
									33,  1, 41,  9, 49, 17, 57, 25 };

int message_expansion[] = { 32,  1,  2,  3,  4,  5,
							 4,  5,  6,  7,  8,  9,
							 8,  9, 10, 11, 12, 13,
							12, 13, 14, 15, 16, 17,
							16, 17, 18, 19, 20, 21,
							20, 21, 22, 23, 24, 25,
							24, 25, 26, 27, 28, 29,
							28, 29, 30, 31, 32,  1 };
int S[8][64] = { { 14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
			 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
			 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
			15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13 },
				{ 15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
			 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
			 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
			13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9 } ,
				{ 10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
			13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
			13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
			 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12 } ,
				{ 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
			13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
			10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
			 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14 },
				{ 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
			14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
			 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
			11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3 },
				{ 12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
			10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
			 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
			 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13 },
				{ 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
			13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
			 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
			 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12 },
				{ 13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
			 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
			 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
			 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11 }
};


int right_sub_message_permutation[] = { 16,  7, 20, 21,
									29, 12, 28, 17,
									 1, 15, 23, 26,
									 5, 18, 31, 10,
									 2,  8, 24, 14,
									32, 27,  3,  9,
									19, 13, 30,  6,
									22, 11,  4, 25 };


//a是输入 shiftSize是左移的位数 实际只有1和2两种可能
u64 shiftLeft28Bits(u64 a, int shiftSize)
{
	a <<= shiftSize;
	if (shiftSize == 1)
	{
		//检查从右到左第29位是什么 赋到最右边 然后清零(这个在函数最后做）
		if (a & 0x10000000)
			a += 1;
	}
	if (shiftSize == 2)
	{
		//检查从右到左第29位and30位是什么 赋到最右边 然后清零(这个在函数最后做）
		if (a & 0x10000000)
			a += 1;
		if (a & 0x20000000)
			a += 2;
	}
	//将前面移出去的位清空
	a &= 0xfffffff;
	return a;
}


// 子密钥生成算法 传入64bit的主密钥 生成16个子密钥
void subKeyGeneration(u64 masterKey, u64* subkey)
{
	//第一次替换选择得到的左部和右部
	u64 Ci = 0;
	u64 Di = 0;
	//生成C0 D0
	for (int i = 0; i < 28; i++)
	{
		//如果masterkey对应位数为1 填充到C0从左往右第i+1位
		if (masterKey & (1LL << (64 - initial_key_permutaion[i])))
			Ci += 1;
		if (masterKey & (1LL << (64 - initial_key_permutaion[28 + i])))
			Di += 1;
		if (i != 27)
		{
			Ci <<= 1;
			Di <<= 1;
		}
	}
	// 生成16个子密钥
	for (int i = 0; i < 16; i++)
	{
		Ci = shiftLeft28Bits(Ci, key_shift_sizes[i]);
		Di = shiftLeft28Bits(Di, key_shift_sizes[i]);
		// 拼接CD
		u64 tempU64 = Ci << 28 ^ Di;
		//进行 置换选择2
		u64 tempSubKey = 0;
		for (int i = 0; i < 48; i++)
		{
			if (tempU64 & (1LL << (56 - sub_key_permutation[i])))
				tempSubKey += 1;
			if (i != 47)
			{
				tempSubKey <<= 1;
			}
		}
		subkey[i] = tempSubKey;
	}
}

//初始IP置换
u64 IP(u64 a)
{
	u64 result = 0;
	for (int i = 0; i < 64; i++)
	{
		if (a & (1LL << (64 - initial_message_permutation[i])))
			result += 1;
		if (i != 63)
		{
			result <<= 1;
		}
	}
	return result;
}

//逆IP置换
u64 inverseIP(u64 a)
{
	u64 result = 0;
	for (int i = 0; i < 64; i++)
	{
		if (a & (1LL << (64 - final_message_permutation[i])))
			result += 1;
		if (i != 63)
		{
			result <<= 1;
		}
	}
	return result;
}

// f函数中的E 选择运算，将32位消息扩展为48bit
u64 E(u64 a)
{
	u64 result = 0;
	for (int i = 0; i < 48; i++)
	{
		if (a & (1LL << (32 - message_expansion[i])))
			result += 1;
		if (i != 47)
		{
			result <<= 1;
		}
	}
	return result;
}

// S 盒置换 输入是48比特 输出32比特
u64 S_permutation(u64 a)
{
	u64 result = 0;
	//8个S盒做8轮
	for (int i = 0; i < 8; i++)
	{
		int tempIndex = 0;
		// 通过a的高六位构造一个直接查询S盒的索引
		// 如果a的最高位为1
		if (a & 0x800000000000LL)
			tempIndex += 0x20;
		if (a & 0x040000000000LL)
			tempIndex += 0x10;
		u64 tmp = a;
		tmp >>= 43;
		tmp &= 0xf;
		tempIndex ^= tmp;
		// 构造出索引
		result += S[i][tempIndex];
		if (i != 7)
			result <<= 4;
		a <<= 6;
	}
	return result;
}

// f函数 也就是加密函数 DES中的核心 输入一个32bit的半个数据块 和 一个48比特的子密钥 输出32位加密结果
u64 f(u64 a, u64 subkey)
{
	u64 temp = E(a);
	temp ^= subkey;
	//接下来S盒
	temp = S_permutation(temp);
	// 最后的P置换运算
	u64 result = 0;
	for (int i = 0; i < 32; i++)
	{
		if (temp & (1LL << (32 - right_sub_message_permutation[i])))
			result += 1;
		if (i != 31)
		{
			result <<= 1;
		}
	}
	return result;
}

u64 DES_enc(u64 msg, u64 *subkeys)
{
	msg = IP(msg);
	// 获取msg的左部和右部的32位
	u64 Li = msg >> 32;
	u64 Ri = msg & 0xffffffff;
	for (int i = 0; i < 15; i++)
	{
		// 临时变量保存L和R
		u64 tmpL = Li;
		u64 tmpR = Ri;
		Li = tmpR;
		Ri = tmpL ^ f(tmpR, subkeys[i]);
	}
	Li = Li ^ f(Ri, subkeys[15]);
	// 构造输出
	u64 result = Li << 32 ^ Ri;
	result = inverseIP(result);
	return result;
}

u64 DES_dec(u64 msg, u64* subkeys)
{
	msg = IP(msg);
	// 获取msg的左部和右部的32位
	u64 Li = msg >> 32;
	u64 Ri = msg & 0xffffffff;
	for (int i = 15; i >0; i--)
	{
		// 临时变量保存L和R
		u64 tmpL = Li;
		u64 tmpR = Ri;
		Li = tmpR;
		Ri = tmpL ^ f(tmpR, subkeys[i]);
	}
	Li = Li ^ f(Ri, subkeys[0]);
	// 构造输出
	u64 result = Li << 32 ^ Ri;
	result = inverseIP(result);
	return result;
}

void generateKeyFile(void)
{
	//生成密钥文件
	FILE* keyFile = NULL;
	fopen_s(&keyFile, "DES_keyfile.key", "wb");
	//打开失败直接退出
	if (keyFile == NULL)
		exit(1);
	time_t t;
	srand((unsigned)time(&t));
	//DES 密钥为8个字节 有8位奇偶校验位 但实际使用并不做奇偶校验 所以随意生成即可
	for (int i = 0; i < 8; i++)
	{
		u8 charToBeWritten = rand() % 256;
		fwrite(&charToBeWritten, 1, 1, keyFile);
	}
	fclose(keyFile);
	printf("Your key file has been setup successfully.\n");
}

u64 u8_to_u64(u8* in)
{
	u64 result = 0;
	for (int i = 0; i < 7; i++)
	{
		result += in[i];
		result <<= 8;
	}
	result += in[7];
	return result;
}

void u64_to_u8(u64 in, u8* out)
{
	for (int i = 7; i >= 0; i--)
	{
		out[i] = in % 256;
		in >>= 8;
	}
}

void write_u64(u64 in, FILE* file)
{
	u8 bytes_8[8] = { 0 };
	u64_to_u8(in, bytes_8);
	size_t writeNum = fwrite(bytes_8, 1, 8, file);
	if (writeNum != 8)
	{
		printf("There is something wrong in writing the output.\n");
		exit(1);
	}
}

// mode 为 1 加密， mode 为 0解密
void DES_UI(int mode)
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
	fopen_s(&keyFile, "DES_keyfile.key", "rb");
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
	
	// 读取主密钥
	u64 masterKey = { 0 };
	if (fread_s(&masterKey, 8, 8, 1, keyFile) != 1)
	{
		printf("There is something wrong about your key file.\n");
		exit(1);
	}

	u64 subKeys[16] = { 0 };
	subKeyGeneration(masterKey, subKeys);

	int counter = 0;
	u8 bytes_8[8] = { 0 };
	u8 tempByte = 0;
	while (fread_s(&tempByte, 1, 1, 1, inputFile) == 1)
	{
		//读够一块
		if (counter == 8)
		{
			
			//从读出的字节回复出u64以送入加密解密函数
			u64 block_in = u8_to_u64(bytes_8);
			u64 block_out = 0;
			if (mode)
				block_out = DES_enc(block_in, subKeys);
			else
				block_out = DES_dec(block_in, subKeys);
			write_u64(block_out, outputFile);
			counter -= 8;
		}
		bytes_8[counter] = tempByte;
		counter++;
	}
	// 由于当正好读完8个字节的最后一个字节后上述的循环就会结束 所以要在下面再次判断
	// 注意到 加密时要考虑padding，如果加密到这里 counter=8说明源文件正好是8字节的倍数
	// padding 方案 最后一个字节指示明文字节数  如果正好8个字节 如果是加密检查最后一个字节是否是0x00-0x07 是的话多加一块空白块

	if (counter == 8)
	{
		//从读出的字节回复出u16以送入加密解密函数
		u64 block_in = u8_to_u64(bytes_8);
		u64 block_out = 0;
		if (mode)
		{
			//加密，检查最后一个字节是不是小于等于0x07
			if (bytes_8[7] <= 0x07)
			{
				block_out = DES_enc(block_in, subKeys);
				write_u64(block_out, outputFile);
				block_in = 0x0LL;
			}
			block_out = DES_enc(block_in, subKeys);
			write_u64(block_out, outputFile);
		}
		//解密
		else
		{
			block_out = DES_dec(block_in, subKeys);
			u64_to_u8(block_out, bytes_8);
			// 判断最后一个字节是否小于等于07，如果是说明这个分组有padding。
			if (bytes_8[7]  <= 0x07)
			{
				//bytes_8[7]里存储的就是有效块的个数
				int messageBytesCounter = bytes_8[7];
				// 如果padding Counter == 0 ，说明整个分组都是填充，所以干脆跳过，避免因为不写东西导致函数崩溃
				if (messageBytesCounter != 0)
				{
					int writeNum = fwrite(bytes_8, 1, messageBytesCounter, outputFile);
					if (writeNum != messageBytesCounter)
					{
						printf("There is something wrong in writing the output.\n");
						exit(1);
					}
				}
			}
			else
				write_u64(block_out, outputFile);

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
			bytes_8[7] = counter;
			while (counter < 7)
			{
				bytes_8[counter] = 0x00;
				counter++;
			}

			u64 block_in = u8_to_u64(bytes_8);
			u64 block_out = DES_enc(block_in, subKeys);
			// 写入密文
			write_u64(block_out, outputFile);
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
	/*u64 masterKey = 0x3132333435363738;
	u64 msg = 0x3031323334353637;
	*/
	printf("This is a simple DES tool implemented by Wu Haolin.\nThe tool will use \"DES_keyfile.key\" as default. \nIf you don't have one now, Please Generate one first.\nPlease enter a number to choose the function you need.\n1. Generate Key randomly.\n2. Encrypt a file.\n3. Decrypt a file.\n4. Exit\n");
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
			DES_UI(1);
			break;
		case 3:
			DES_UI(0);
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


// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
