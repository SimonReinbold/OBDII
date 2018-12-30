#ifndef _SESSION_H_
#define _SESSION_H_

#ifndef NULL
#define NULL ((void *)0)
#endif

struct supportedRequests_struct {
	unsigned char SID01[6 * 4]; // 6 PID requests with 4 bytes each
	unsigned char SID02[6 * 4];
	unsigned char SID03;
	unsigned char SID04;
	unsigned char SID05[1 * 4];
	unsigned char SID06;
	unsigned char SID07;
	unsigned char SID08;
	unsigned char SID09[1 * 4];
	unsigned char SID10;
};

typedef struct Session_KWP2000_Struct Session_KWP2000;

struct Session_KWP2000_Struct {
	unsigned char headerType;
	unsigned char target;
	unsigned char keybytes[2];
	struct supportedRequests_struct supportedRequests;
};

Session_KWP2000* newSession(unsigned char target);
void destroySession();
unsigned char request_Session_KWP2000(Session_KWP2000* session, unsigned char* data, unsigned char nbytes);
unsigned char* getReplyProtocol_Session_KWP2000();
unsigned char getReplyLengthProtocol_Session_KWP2000();

#endif // !_SESSION_H_