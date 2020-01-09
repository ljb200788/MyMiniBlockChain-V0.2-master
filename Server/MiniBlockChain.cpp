#include "stdafx.h"
#include "MiniBlockChain.h"

#ifdef WIN32
#   include <windows.h>

#else
#   include <sys/time.h>
#endif
#ifdef WIN32
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS 11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS 11644473600000000ULL
#endif

struct timezone
{
	int  tz_minuteswest; // minutes W of Greenwich  
	int  tz_dsttime;     // type of dst correction
};
int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	FILETIME ft;
	unsigned long long tmpres = 0;
	static int tzflag = 0;
	if (tv)
	{
#ifdef _WIN32_WCE
		SYSTEMTIME st;
		GetSystemTime(&st);
		SystemTimeToFileTime(&st, &ft);
#else
		GetSystemTimeAsFileTime(&ft);
#endif
		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;
		/*converting file time to unix epoch*/
		tmpres /= 10;  /*convert into microseconds*/
		tmpres -= DELTA_EPOCH_IN_MICROSECS;
		tv->tv_sec = (long)(tmpres / 1000000UL);
		tv->tv_usec = (long)(tmpres % 1000000UL);
	}
	if (tz) {
		if (!tzflag) {
#if !TSK_UNDER_WINDOWS_RT
			_tzset();
#endif
			tzflag++;
		}
		tz->tz_minuteswest = _timezone / 60;
		tz->tz_dsttime = _daylight;
	}
	return 0;
}
#endif

#define SHA256_ROTL(a,b) (((a>>(32-b))&(0x7fffffff>>(31-b)))|(a<<b))  
#define SHA256_SR(a,b) ((a>>b)&(0x7fffffff>>(b-1)))  
#define SHA256_Ch(x,y,z) ((x&y)^((~x)&z))  
#define SHA256_Maj(x,y,z) ((x&y)^(x&z)^(y&z))  
#define SHA256_E0(x) (SHA256_ROTL(x,30)^SHA256_ROTL(x,19)^SHA256_ROTL(x,10))  
#define SHA256_E1(x) (SHA256_ROTL(x,26)^SHA256_ROTL(x,21)^SHA256_ROTL(x,7))  
#define SHA256_O0(x) (SHA256_ROTL(x,25)^SHA256_ROTL(x,14)^SHA256_SR(x,3))  
#define SHA256_O1(x) (SHA256_ROTL(x,15)^SHA256_ROTL(x,13)^SHA256_SR(x,10))


CMiniBlockChain::CMiniBlockChain()
{
	m_pMyBlock = NULL;
	m_pTransactionInfo = NULL;
	m_strNodeRegister = NULL;
}

CMiniBlockChain::~CMiniBlockChain()
{
	if (m_pMyBlock != NULL)
		free(m_pMyBlock);
	if (m_pTransactionInfo != NULL)
		free(m_pTransactionInfo);
	if (m_strNodeRegister != NULL)
		free(m_strNodeRegister);
	_CrtDumpMemoryLeaks();
}

char*  CMiniBlockChain::getStrSHA256Result(const char* str, long long length, char* sha256)
{
	char *pp, *ppend;
	long l, i, W[64], T1, T2, A, B, C, D, E, F, G, H, H0, H1, H2, H3, H4, H5, H6, H7;
	H0 = 0x6a09e667, H1 = 0xbb67ae85, H2 = 0x3c6ef372, H3 = 0xa54ff53a;
	H4 = 0x510e527f, H5 = 0x9b05688c, H6 = 0x1f83d9ab, H7 = 0x5be0cd19;
	long K[64] = {
		0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
		0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
		0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
		0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
		0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
		0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
		0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
		0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
	};
	l = length + ((length % 64 > 56) ? (128 - length % 64) : (64 - length % 64));
	if (!(pp = (char*)malloc((unsigned long)l))) return 0;
	for (i = 0; i < length; pp[i + 3 - 2 * (i % 4)] = str[i], i++);
	for (pp[i + 3 - 2 * (i % 4)] = 128, i++; i < l; pp[i + 3 - 2 * (i % 4)] = 0, i++);
	*((long*)(pp + l - 4)) = length << 3;
	*((long*)(pp + l - 8)) = length >> 29;
	for (ppend = pp + l; pp < ppend; pp += 64) {
		for (i = 0; i < 16; W[i] = ((long*)pp)[i], i++);
		for (i = 16; i < 64; W[i] = (SHA256_O1(W[i - 2]) + W[i - 7] + SHA256_O0(W[i - 15]) + W[i - 16]), i++);
		A = H0, B = H1, C = H2, D = H3, E = H4, F = H5, G = H6, H = H7;
		for (i = 0; i < 64; i++) {
			T1 = H + SHA256_E1(E) + SHA256_Ch(E, F, G) + K[i] + W[i];
			T2 = SHA256_E0(A) + SHA256_Maj(A, B, C);
			H = G, G = F, F = E, E = D + T1, D = C, C = B, B = A, A = T1 + T2;
		}
		H0 += A, H1 += B, H2 += C, H3 += D, H4 += E, H5 += F, H6 += G, H7 += H;
	}
	free(pp - l);
	sprintf(sha256, "%08X%08X%08X%08X%08X%08X%08X%08X", H0, H1, H2, H3, H4, H5, H6, H7);
	return sha256;
}

int CMiniBlockChain::getBlockLength()
{
	int length = 0;
	Block* pTemp= m_pMyBlock;
	while (pTemp != NULL)
	{
		pTemp = pTemp->next;
		length++;
	}
	return length;
}

BOOL CMiniBlockChain::deleteNode(int index)
{
	if (m_pMyBlock == NULL)
		return false;
	if (index == 0)
		m_pMyBlock = m_pMyBlock->next;
	else
	{
		int i = 0;
		Block* ptr = m_pMyBlock;
		Block* pre = NULL;
		while (ptr != NULL && i < index)
		{
			pre = ptr;
			ptr = ptr->next;
			i++;
		}
		pre->next = ptr->next;
	}
	return true;
}

BOOL   CMiniBlockChain::getTimeStamp(char *ts)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	sprintf(ts, "%d.%d", tv.tv_sec, tv.tv_usec);
	return true;
}

void  CMiniBlockChain::getBlockStr(Block * pb, char **bs)
{
	int length;
	length = strlen(pb->previous_hash) + strlen(pb->timeStamp) + 200;
	if (pb->transactions != NULL)
		length += strlen(pb->transactions);
	*bs = (char*)malloc(sizeof(char)*(length));
	if (*bs == NULL)
	{
		AfxMessageBox(_T("Memory Leak!"));
		return;
	}
	*bs[0] = '\0';
	if(pb->transactions==NULL)
		sprintf(*bs, "{\n    \"index\": %d,\n    \"previous_hash\": \"%s\",\n    \"proof\": %d,\n    \"timestamp\": %s,\n    \"transactions\": [%s]\n}", pb->index, pb->previous_hash, pb->proof, pb->timeStamp,"");
	else
		sprintf(*bs, "{\n    \"index\": %d,\n    \"previous_hash\": \"%s\",\n    \"proof\": %d,\n    \"timestamp\": %s,\n    \"transactions\": [%s\n    ]\n}", pb->index, pb->previous_hash, pb->proof, pb->timeStamp, pb->transactions);
	return;
}

void CMiniBlockChain::getLastBlockStr(char **bs)
{
	Block* pTemp = m_pMyBlock;
	if (pTemp == NULL)
	{
		*bs = NULL;
		return;
	}
	while (pTemp->next != NULL)
		pTemp = pTemp->next;
	getBlockStrForHash(pTemp, bs);
	return;
}

void  CMiniBlockChain::getBlockStrForHash(Block * pb, char **bs)
{
	getBlockStr(pb, bs);
	CString  lastBlockString;
	lastBlockString = *bs;
	lastBlockString.Replace(_T("\n"), _T(""));
	lastBlockString.Replace(_T("    "), _T(" "));
	int iLength = WideCharToMultiByte(CP_ACP, 0, lastBlockString.GetBuffer(0), -1, NULL, 0, NULL, FALSE);
	WideCharToMultiByte(CP_ACP, NULL, lastBlockString.GetBuffer(0), -1, *bs, iLength, NULL, FALSE);
}

Block* CMiniBlockChain::getLastBlock()
{
	Block* pTemp = m_pMyBlock;
	if (pTemp == NULL)
		return pTemp;
	while (pTemp->next != NULL)
		pTemp = pTemp->next;
	return pTemp;
}

BOOL CMiniBlockChain::initBlockChain(char *ts)
{
	//创建创世块
	m_pMyBlock = (Block*)malloc(sizeof(struct Block));
	m_pMyBlock->index = 1;
	strcpy(m_pMyBlock->previous_hash, "1");
	m_pMyBlock->proof = 100;
	strcpy(m_pMyBlock->timeStamp, ts);
	m_pMyBlock->transactions=NULL;
	m_pMyBlock->next = NULL;
	return true;
}

void CMiniBlockChain::newTransaction(int amount, char* recipient, char* sender)
{
	if (m_pTransactionInfo == NULL)
	{
		m_pTransactionInfo =  (char*)malloc(MAX_TRANSACTIONS_INFO_LENGTH);
		sprintf(m_pTransactionInfo, "\n    {\n    \"amount\": %d,\n     \"recipient\" : \"%s\",\n     \"sender\" : \"%s\"\n    }", amount, recipient, sender);
	}
	else
	{
		char* temp= (char*)malloc(strlen(m_pTransactionInfo)+1);
		strcpy(temp, m_pTransactionInfo);
		m_pTransactionInfo = (char*)malloc(strlen(temp) + MAX_TRANSACTIONS_INFO_LENGTH);
		strcpy(m_pTransactionInfo, temp);
		strcat(m_pTransactionInfo, ",\n");
		sprintf(temp, "\n    {\n    \"amount\": %d,\n     \"recipient\" : \"%s\",\n     \"sender\" : \"%s\"\n    }", amount, recipient, sender);
		strcat(m_pTransactionInfo, temp);
		free(temp);
	}

	return;
}

BOOL CMiniBlockChain::newBlock()
{
	Block* pTemp = getLastBlock();
	if (pTemp == NULL)
	{
		AfxMessageBox(_T("必须先创建\"创世块\",然后进行其它区块链操作!"));
		return false;
	}
	Block* qTemp = (Block*)malloc(sizeof(struct Block));
	if (qTemp == NULL)
	{
		AfxMessageBox(_T("Memory Leak!"));
		return false;
	}
	qTemp->index = getBlockLength()+1;
	char  *bs=NULL;
	getLastBlockStr(&bs);
	//获取上一个区块的HASH值
	getStrSHA256Result(bs,strlen(bs), qTemp->previous_hash);
	qTemp->proof = proof_of_work(pTemp->proof);
	getTimeStamp(qTemp->timeStamp);
	if (m_pTransactionInfo != NULL)
	{
		qTemp->transactions = (char*)malloc(sizeof(char) * (strlen(m_pTransactionInfo) + 1));
		strcpy(qTemp->transactions, m_pTransactionInfo);
	}
	else
		qTemp->transactions = NULL;
	free(m_pTransactionInfo);
	m_pTransactionInfo = NULL;
	qTemp->next = NULL;
	pTemp->next = qTemp;
	free(bs);
	return true;
}

BOOL CMiniBlockChain::valid_proof(int last_proof, int proof)
{
	char  proofString[50];
	char  hashValue[65];
	sprintf(proofString, "%d%d", last_proof, proof);
	getStrSHA256Result(proofString, strlen(proofString), hashValue);
	hashValue[4] = '\0';
	if (_stricmp(hashValue, "0000") == 0)
		return true;
	else
		return false;
}

int CMiniBlockChain::proof_of_work(int lastProof)
{
	/*
	简单的工作量证明:
	- 查找一个 p' 使得 hash(pp') 以4个0开头
	- p 是上一个块的证明,  p' 是当前的证明
	*/
	int proof=0;
	while (!valid_proof(lastProof, proof))
		proof++;
	return proof;
}

void CMiniBlockChain::registerNode(char* node)
{
	if (node != NULL)
	{
		//之前注册节点为空
		m_strNodeRegister = (char*)malloc(strlen(node) + 1);
		strcpy(m_strNodeRegister, node);
	}
	else if (strstr(m_strNodeRegister, node) == NULL )
	{
		//注册节点不在已经注册节点之中
		int length = strlen(m_strNodeRegister) + 1;
		char*temp = (char*)malloc(length);
		strcpy(temp, m_strNodeRegister);
		m_strNodeRegister = (char*)malloc(length + strlen(node) + 3);
		strcpy(m_strNodeRegister, temp);
		strcat(m_strNodeRegister, ";");
		strcat(m_strNodeRegister, node);
		free(temp);
	}
	return;
}

/*
0：链为空
-1:链数据有错误
1：链数据完整
*/
int CMiniBlockChain::valid_chain()
{
	Block* pTemp = m_pMyBlock;
	if (pTemp == NULL)
		return 0;
	while (pTemp->next != NULL)
	{
		Block* qTemp = pTemp->next;
		char* bs;
		char previous_hash[65];
		getBlockStrForHash(pTemp, &bs);
		getStrSHA256Result(bs, strlen(bs), previous_hash);
		if (_stricmp(previous_hash, qTemp->previous_hash) != 0)
			return -1;
		if (!valid_proof(pTemp->proof, qTemp->proof))
			return -1;
		pTemp = pTemp->next;
	}
	return 1;
}

BOOL  CMiniBlockChain::copyChain(Block* src, int length)
{
	if (m_pMyBlock != NULL)
		free(m_pMyBlock);
	Block* pTemp = src;
	Block** qTemp = &m_pMyBlock;
	while (pTemp != NULL)
	{
		*qTemp = (Block*)malloc(sizeof(struct Block));
		**qTemp = *pTemp;
		pTemp = pTemp->next;
		qTemp = &((*qTemp)->next);
	}
	return true;
}