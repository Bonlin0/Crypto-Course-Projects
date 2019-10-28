// IDEA_whl.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
// Author: Wu Haolin
// Date : 2019/10/27 14:47


#include <stdio.h>
#include <time.h>
#include <stdlib.h>

typedef unsigned short int u16;
typedef unsigned int u32;
typedef unsigned long long int u64;
typedef unsigned char u8;

#define mulModulus 65537
#define addModulus 65536

//kn【i】表示第n个秘钥在第i-1轮使用
u16 subkey[52] = { 0 };
// 用于解密的解密子密钥
u16 decSubkey[52] = { 0 };
// 输出变换子密钥
u16 k_output[4] = { 0 };

//十六位相乘 利用了溢出 用0来代替65536
 u16 IdeaMultiplication(u16 a, u16 b)
{
	u32 a_t = a;
	u32 b_t = b;
	if (a_t == 0)
		a_t = addModulus;
	if (b_t == 0)
		b_t = addModulus;
	return (u16)((a_t * b_t) % mulModulus);
}

 inline u16 IdeaXor(u16 a, u16 b)
 {
	 return a ^ b;
 }

 inline u16 IdeaAdd(u16 a, u16 b)
 {
	 return a + b;
 }

 //25 bit 循环左移 对于128位的密钥
 static inline void keyRotate(u64* key)
 {
	 u64 temp;
	 temp = (key[0] << 25u) + (key[1] >> 39u);
	 key[1] = (key[1] << 25u) + (key[0] >> 39u);
	 key[0] = temp;
 }

 //生成子密钥
 void subKeyGeneration(u64* key)
 {
	 int counter = 0;
	 // 该变量是用来存储key的，因为keyRote会对key直接进行修改
	 u64 key_temp[2] = { 0 };
	 key_temp[0] = key[0];
	 key_temp[1] = key[1];
	 for (counter = 0; counter < 48; counter += 8)
	 {
		 subkey[counter] = (u16)(key_temp[0] >> 48);
		 subkey[counter + 1] = (u16)(key_temp[0] >> 32);
		 subkey[counter + 2] = (u16)(key_temp[0] >> 16);
		 subkey[counter + 3] = (u16)(key_temp[0]);
		 subkey[counter + 4] = (u16)(key_temp[1] >> 48);
		 subkey[counter + 5] = (u16)(key_temp[1] >> 32);
		 subkey[counter + 6] = (u16)(key_temp[1] >> 16);
		 subkey[counter + 7] = (u16)(key_temp[1]);
		 keyRotate(key_temp);
	 }
	 //最后一次移位后只取4个子密钥单独写
	 subkey[counter] = (u16)(key_temp[0] >> 48);
	 subkey[counter + 1] = (u16)(key_temp[0] >> 32);
	 subkey[counter + 2] = (u16)(key_temp[0] >> 16);
	 subkey[counter + 3] = (u16)(key_temp[0]);
 }

 //求加法逆元 由于变量类型无符号整数，所以很方便
 u16 inline addInverse(u16 a)
 {
	 return  - a;
 }

 //求乘法逆元 用的是扩展欧几里得除法
 u16 MulInverse(u16 number)
 {
	 int j = 1;
	 signed long int result, temp, intermediate;
	 signed long int buffer[35];//35 is an upper bound for intermediate result of a inversion of a 16 bits integer

	 buffer[0] = number;
	 buffer[1] = mulModulus;

	 while (buffer[j] != 0) //find intermediate values of greatest common divisor
	 {
		 j++;
		 buffer[j] = buffer[j - 2] % buffer[j - 1];
	 }

	 result = 1;
	 intermediate = 1;
	 temp = 0;

	 while (j > 1) //inverse calculation from intermediates values
	 {

		 j--;
		 result = temp;
		 temp = intermediate - ((buffer[j - 1] / buffer[j]) * temp);
		 intermediate = result;
	 }

	 if (result > 0)
		 return (u16)result;
	 else return (u16)(mulModulus + result);
 }

 // 前8轮迭代运算
 void roundProccessing(int round, u16* msg, u16* subkey)
 {
	 u16 result[4] = { 0 };
	 // 按照武汉大学出版社密码学引论 77面上方算法编写
	 u16 t1 = IdeaMultiplication(msg[0], subkey[round * 6]);
	 u16 t2 = IdeaAdd(msg[1], subkey[round * 6 + 1]);
	 u16 t3 = IdeaAdd(msg[2], subkey[round * 6 + 2]);
	 u16 t4 = IdeaMultiplication(msg[3], subkey[round * 6 + 3]);
	 u16 t5 = IdeaXor(t1, t3);
	 u16 t6 = IdeaXor(t2, t4);
	 u16 t7 = IdeaMultiplication(t5, subkey[round * 6 + 4]);
	 u16 t8 = IdeaAdd(t6,t7);
	 u16 t9 = IdeaMultiplication(t8, subkey[round * 6 + 5]);
	 u16 t10 = IdeaAdd(t7,t9);
	 msg[0] = IdeaXor(t1, t9);
	 msg[1] = IdeaXor(t3, t9);
	 msg[2] = IdeaXor(t2, t10);
	 msg[3] = IdeaXor(t4, t10);
	 //如果不是第八轮  交换中间2块
	 if (round == 7)
	 {
		 u16 temp = msg[1];
		 msg[1] = msg[2];
		 msg[2] = temp;
	 }

 }

 // 解密密钥生成
 void decSubKeyGeneration(u64* key)
 {
	 int i;
	 subKeyGeneration(key);

	 decSubkey[0] = MulInverse(subkey[48]);
	 decSubkey[1] = addInverse(subkey[49]);
	 decSubkey[2] = addInverse(subkey[50]);
	 decSubkey[3] = MulInverse(subkey[51]);	 

	 //set remaining decSubkeys
	 for (i = 0; i < 48; i += 6)
	 {
		 decSubkey[4 + i] = subkey[46 - i];
		 decSubkey[5 + i] = subkey[47 - i];
		 decSubkey[6 + i] = MulInverse(subkey[42 - i]);
		 decSubkey[7 + i] = addInverse(subkey[44 - i]);
		 decSubkey[8 + i] = addInverse(subkey[43 - i]);
		 decSubkey[9 + i] = MulInverse(subkey[45 - i]);
	 }

	 u16 t = decSubkey[50];
	 decSubkey[50] = decSubkey[49];
	 decSubkey[49] = t;
}



 //输入4个16位块和密钥 输出4个16位块
void IdeaBlockEnc(u16* msg, u16* output)
 {
	 // subKeyGeneration(key);
	 // 密钥生成交到外部
	 for (int i = 0; i < 8; i++)
	 {
		 roundProccessing(i, msg, subkey);
	 }
	 //输出变换
	 output[0] = IdeaMultiplication(msg[0],subkey[48]);
	 output[1] = IdeaAdd(msg[1],subkey[49]);
	 output[2] = IdeaAdd(msg[2],subkey[50]);
	 output[3] = IdeaMultiplication(msg[3],subkey[51]);
 }

//输入4个16位块和密钥 输出4个16位块
void IdeaBlockDec(u16* msg, u16* output)
{
	 // decSubKeyGeneration(key);
	 // 密钥生成交到外部
	for (int i = 0; i < 8; i++)
	{
		roundProccessing(i, msg, decSubkey);
	}
	//输出变换
	output[0] = IdeaMultiplication(msg[0], decSubkey[48]);
	output[1] = IdeaAdd(msg[1], decSubkey[49]);
	output[2] = IdeaAdd(msg[2], decSubkey[50]);
	output[3] = IdeaMultiplication(msg[3], decSubkey[51]);
}


void generateKeyFile(void)
{
	//生成密钥文件
	FILE* keyFile = NULL;
	fopen_s(&keyFile, "IDEA_keyfile.key", "wb");
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

//将4个16bit的信息写入file的函数
void write_u16_4(FILE* file, u16* msg)
{
	u8 bytes8_write[8] = { 0 };
	for (int i = 0; i < 4; i++)
	{
		bytes8_write[i * 2] = (u8)(msg[i] >> 8);
		bytes8_write[i * 2 + 1] = (u8)(msg[i] % 256);
	}
	size_t writeNum = fwrite(bytes8_write, 1, 8, file);
	if (writeNum != 8)
	{
		printf("There is something wrong in writing the output.\n");
		exit(1);
	}
}

// mode 为 1 加密， mode 为 0解密
void IDEA_UI(int mode)
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
	fopen_s(&keyFile, "IDEA_keyfile.key", "rb");
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
	//临时变量用于存储取出的word
	u64 tempHalfKey = 0;
	int counter = 0;
	u64 MasterKey[2] = { 0 };
	while (fread_s(&tempHalfKey, 8, 8, 1, keyFile) == 1)
	{
		//如果Keyfile大于128Bits退出
		if (counter == 2)
		{
			printf("There is something wrong about your key file.\n");
			exit(1);
		}
		MasterKey[counter] = tempHalfKey;
		counter++;
	}
	//如果读取出来的字数目不对退出
	if (counter != 2)
	{
		printf("There is something wrong about your key file.\n");
		exit(1);
	}

	//根据加密或解密 先把密钥扩展做好
	if (mode)
		decSubKeyGeneration(MasterKey);
	else
		subKeyGeneration(MasterKey);

	u16 msg_in[4] = { 0 };
	u16 msg_out[4] = { 0 };
	
	counter = 0;
	u8 bytes_8[8] = { 0 };
	u8 tempByte = 0;
	u8 bytes_8_write = { 0 };
	while (fread_s(&tempByte, 1, 1, 1, inputFile) == 1)
	{
		//读够一块
		if (counter == 8)
		{
			//u16 msg_in[4] = { 0 };
			//从读出的字节回复出u16以送入加密解密函数
			for (int i = 0; i < 4; i++)
			{
				msg_in[i] = (u16)bytes_8[2 * i] << 8 ^ (u16)bytes_8[2 * i + 1];
			}
			//u16 msg_out[4] = { 0 };

			if (mode)
				IdeaBlockEnc(msg_in, msg_out);
			else
				IdeaBlockDec(msg_in, msg_out);
			
			write_u16_4(outputFile, msg_out);

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
		for (int i = 0; i < 4; i++)
		{
			msg_in[i] = (u16)bytes_8[2 * i] << 8 ^ (u16)bytes_8[2 * i + 1];
		}
		if (mode)
		{
			//加密，检查最后一个字节是不是小于等于0x07
			if (bytes_8[7] <= 0x07)
			{
				
				
				
				IdeaBlockEnc(msg_in, msg_out);

				write_u16_4(outputFile, msg_out);

				for (int i = 0; i < 4; i++)
				{
					msg_in[i] = 0x0000;
				}
			}
			IdeaBlockEnc(msg_in, msg_out);
			write_u16_4(outputFile, msg_out);
		}
		//解密
		else
		{
			IdeaBlockDec(msg_in, msg_out);
			// 判断最后一个字节是否小于等于07，如果是说明这个分组有padding。
			if (msg_out[3] % 256 <= 0x07)
			{
				//声明计数器计算padding数目
				u8 bytes8_write[8] = { 0 };
				for (int i = 0; i < 4; i++)
				{
					bytes8_write[i * 2] = (u8)(msg_out[i] >> 8);
					bytes8_write[i * 2 + 1] = (u8)(msg_out[i] % 256);
				}
				int messageBytesCounter = (int)msg_out[3] % 256;

				// 如果padding Counter == 0 ，说明整个分组都是填充，所以干脆跳过，避免因为不写东西导致函数崩溃
				if (messageBytesCounter != 0)
				{
					size_t writeNum = fwrite(bytes8_write, 1, messageBytesCounter, outputFile);
					if (writeNum != messageBytesCounter)
					{
						printf("There is something wrong in writing the output.\n");
						exit(1);
					}
				}
			}
			else
				write_u16_4(outputFile, msg_out);

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

			//从读出的字节回复出u16以送入加密解密函数
			for (int i = 0; i < 4; i++)
			{
				msg_in[i] = (u16)bytes_8[2 * i] << 8 ^ (u16)bytes_8[2 * i + 1];
			}


			IdeaBlockEnc(msg_in, msg_out);
			// 写入密文
			write_u16_4(outputFile, msg_out);
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
	printf("This is a simple IDEA tool implemented by Wu Haolin.\nThe tool will use \"IDEA_keyfile.key\" as default. \nIf you don't have one now, Please Generate one first.\nPlease enter a number to choose the function you need.\n1. Generate Key randomly.\n2. Encrypt a file.\n3. Decrypt a file.\n4. Exit\n");
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
			IDEA_UI(1);
			break;
		case 3:
			IDEA_UI(0);
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

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
