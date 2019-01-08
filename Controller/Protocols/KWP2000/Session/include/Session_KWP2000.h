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
	unsigned char SID0A;
	unsigned char SID10;
};

typedef struct Session_KWP2000_Struct Session_KWP2000;

struct Session_KWP2000_Struct {
	unsigned char headerType;
	unsigned char target;
	unsigned char source;
	unsigned char keybytes[2];
	unsigned char requestSID;
	unsigned char* requestData;
	unsigned char requestDataLength;
	struct supportedRequests_struct supportedRequests;
};

void setActiveSession(Session_KWP2000* currentSession);
Session_KWP2000* getActiveSession();

Session_KWP2000* newSession(unsigned char target);
void destroySession();

unsigned char checkSupport(unsigned char* data, unsigned char nbytes);
unsigned char handle_session_request(unsigned char* data, unsigned char nbytes);

unsigned char* getReplyProtocol_Session_KWP2000();
unsigned char getReplyLengthProtocol_Session_KWP2000();

#endif // !_SESSION_H_