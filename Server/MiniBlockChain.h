#pragma once
#define MAX_TRANSACTIONS_INFO_LENGTH 255
typedef struct Block
{
	int index;
	char previous_hash[65];
	int  proof;
	char timeStamp[18];
	char* transactions;
	Block * next;
}*pBlock;

class CMiniBlockChain
{
public:
	CMiniBlockChain();
	~CMiniBlockChain();
	char*  getStrSHA256Result(const char* str, long long length, char* sha256);
	Block* getBlockChain() { return m_pMyBlock; };
	BOOL   initBlockChain(char *ts);
	int    getBlockLength();
	BOOL   getTimeStamp(char *ts);
	void   getLastBlockStr(char **bs);
	void   getBlockStr(Block * pb,char **bs);
	void   getBlockStrForHash(Block * pb, char **bs);
	Block* getLastBlock();
	void   newTransaction(int amount, char* recipient, char* sender);
	BOOL   newBlock();
	int    proof_of_work(int lastProof);
	BOOL   valid_proof(int last_proof, int proof);
	BOOL   deleteNode(int index);
	void   registerNode(char* node);
	int    valid_chain();
	char*  getNodeRegister() { return m_strNodeRegister; };
	BOOL   copyChain(Block* src, int length);
private:
	Block* m_pMyBlock;
	char*  m_pTransactionInfo;
	char*  m_strNodeRegister;
};

